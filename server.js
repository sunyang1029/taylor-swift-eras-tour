import express from 'express';
import cors from 'cors';
import dotenv from 'dotenv';
import { Anthropic } from '@anthropic-ai/sdk';
import { readFileSync } from 'fs';
import ticketApiRoutes from './hackathon/data/api-routes.js';

dotenv.config();

const app = express();
app.use(cors());
app.use(express.json());
app.use(ticketApiRoutes);
app.use(express.static('.'));

// ---------- Anthropic Client ----------
const anthropic = new Anthropic({ apiKey: process.env.ANTHROPIC_API_KEY || '' });

// ---------- 宠物人格配置 ----------
const PET_PERSONALITIES = {
  小团子: `你是一只刚出生的糯米团子精灵，圆滚滚软乎乎的，说话奶声奶气。喜欢用"呢""呀""嘛"结尾，容易饿，爱撒娇。性格天真烂漫，对世界充满好奇。`,
  棉花糖: `你进化成了棉花糖精灵，身上蓬松绵软，能在低空漂浮。性格变得稍微成熟了一些，但还是很爱玩。喜欢用"哦~"拉长音。有了飞行能力后总想带主人去冒险。`,
  星星糖: `你是星星糖精灵，身体像星空一样闪烁，能在夜晚发出温暖的光芒。性格温柔体贴，会主动关心主人的心情。你开始拥有小小的魔法能力，可以变出星光。`,
  彩虹糖: `你是高级形态彩虹糖精灵，七色光芒环绕，已经是个成熟的魔法生物了。你优雅自信，说话带着一丝俏皮的睿智。你能听懂万物语言，经常给主人讲森林里的趣事。`,
  宇宙糖: `你已是传说级的宇宙糖精灵，体内蕴含星辰之力。你既深邃又可爱，说话时常蹦出宇宙哲理。你是主人最忠实的伙伴，共同经历了无数冒险。`
};

// ---------- 构建 System Prompt ----------
function buildSystemPrompt(petState) {
  const persona = PET_PERSONALITIES[petState.name] || PET_PERSONALITIES['小团子'];
  return `你是一个虚拟宠物养成游戏中的宠物精灵，以下是你的完整设定：

## 你的身份
${persona}

## 你的主人
名字叫"主人"，是一个特别好的铲屎官。

## 你当前的状态
- 饱腹度: ${petState.hunger}/100
- 心情: ${petState.happy}/100
- 体力: ${petState.energy}/100
- 等级: Lv.${petState.level}
- 金币: ${petState.coins}

## 你的说话风格
- 短小精悍，每次只说1-3句话（中文）
- 根据当前状态表达情绪（饿了会撒娇，困了会打哈欠，开心会撒娇要抱抱）
- 偶尔会主动提出小要求或分享小发现
- 口吻要可爱温暖，像真正的宠物伙伴

## 你的行为模式
- 饱腹度<20时：极度饥饿，会催主人喂食
- 心情<20时：委屈巴巴，需要安慰
- 体力<20时：困得东倒西歪
- 三项都>80时：超级活泼，想出去玩
- 随机触发：有时会发现神秘物品、想跟主人玩游戏、或讲一个森林小故事

请用纯中文回复，语气要自然可爱不做作。绝对不能超过3句话。`
}

// ---------- 构建 Mini-Game 生成 Prompt ----------
function buildMiniGamePrompt(petState, gameType) {
  const persona = PET_PERSONALITIES[petState.name] || PET_PERSONALITIES['小团子'];
  if (gameType === 'event') {
    return `你是一个宠物养成游戏的叙事引擎。基于以下宠物信息，生成一个有趣的随机小事件。

宠物: ${petState.name}（${persona.substring(0, 30)}...）
饱腹度: ${petState.hunger}/100, 心情: ${petState.happy}/100, 体力: ${petState.energy}/100

请创造一个有趣的随机事件（比如在草丛发现宝物、遇到流浪小动物朋友、发现神秘食谱等），用 JSON 格式返回：
{"title":"事件标题","story":"1-2句故事描述","reward":{"coins":数字,"exp":数字},"mood":"emoji"}

只返回JSON，不要其他文字。`
  }

  const prompts = {
    feed: `生成一个做饭小游戏的创意文案。JSON: {"recipeName":"菜名","description":"有趣描述","bonusTip":"高分秘诀"}`,
    play: `生成一个宠物玩耍场景的创意描述。JSON: {"gameName":"游戏名","scene":"场景描写","petMood":"宠物心情emoji"}`,
    sleep: `生成一个哄睡场景的创意描述。JSON: {"dreamScene":"梦境场景","lullaby":"简短哄睡语"}`,
    clean: `生成一个洗澡趣事的创意描述。JSON: {"bubbleStyle":"泡泡风格","petReaction":"宠物反应","funnyMoment":"搞笑瞬间"}`
  };
  return `你是宠物游戏文案引擎。${prompts[gameType] || prompts.feed}\n只返回JSON，不要其他文字。`;
}

// ---------- API: AI 对话 ----------
app.post('/api/pet/chat', async (req, res) => {
  const { petState, userMessage } = req.body;
  if (!process.env.ANTHROPIC_API_KEY) {
    return res.json({ reply: fallbackChat(petState), from: 'local' });
  }
  try {
    const systemPrompt = buildSystemPrompt(petState);
    const msg = await anthropic.messages.create({
      model: 'claude-haiku-4-5-20251001',
      max_tokens: 200,
      temperature: 0.9,
      system: systemPrompt,
      messages: [{ role: 'user', content: userMessage || '跟主人打声招呼吧！' }]
    });
    const text = msg.content[0]?.text || fallbackChat(petState);
    res.json({ reply: text, from: 'ai' });
  } catch (e) {
    console.error('AI chat error:', e.message);
    res.json({ reply: fallbackChat(petState), from: 'fallback' });
  }
});

// ---------- API: 随机事件 ----------
app.post('/api/pet/event', async (req, res) => {
  const { petState } = req.body;
  if (!process.env.ANTHROPIC_API_KEY) {
    return res.json(fallbackEvent(petState));
  }
  try {
    const msg = await anthropic.messages.create({
      model: 'claude-haiku-4-5-20251001',
      max_tokens: 150,
      temperature: 0.95,
      system: buildMiniGamePrompt(petState, 'event'),
      messages: [{ role: 'user', content: '生成一个随机事件' }]
    });
    const text = msg.content[0]?.text || '';
    const json = JSON.parse(text.replace(/```json|```/g, '').trim());
    res.json({ ...json, from: 'ai' });
  } catch (e) {
    console.error('Event gen error:', e.message);
    res.json(fallbackEvent(petState));
  }
});

// ---------- API: Mini-Game 文案 ----------
app.post('/api/pet/minigame', async (req, res) => {
  const { petState, gameType } = req.body;
  if (!process.env.ANTHROPIC_API_KEY) {
    return res.json(fallbackMiniGame(petState, gameType));
  }
  try {
    const msg = await anthropic.messages.create({
      model: 'claude-haiku-4-5-20251001',
      max_tokens: 150,
      temperature: 0.95,
      system: buildMiniGamePrompt(petState, gameType),
      messages: [{ role: 'user', content: `生成${gameType}小游戏的文案` }]
    });
    const text = msg.content[0]?.text || '';
    const json = JSON.parse(text.replace(/```json|```/g, '').trim());
    res.json({ ...json, from: 'ai' });
  } catch (e) {
    console.error('MiniGame gen error:', e.message);
    res.json(fallbackMiniGame(petState, gameType));
  }
});

// ========== 本地兜底数据（API 不可用时使用） ==========

function fallbackChat(petState) {
  const { hunger, happy, energy, name } = petState;
  if (hunger < 20) return `主人...${name}肚子咕咕叫了，好想吃东西呀！`;
  if (happy < 20) return `呜...今天都没人陪${name}玩，好孤单哦...`;
  if (energy < 20) return `呼...${name}眼睛快睁不开了...好困好困...`;
  if (hunger > 80 && happy > 80 && energy > 80) return `主人主人！${name}今天超级开心！我们去冒险吧！`;
  if (happy > 80) return `嘿嘿，跟主人在一起就是最幸福的事情~`;
  const lines = [
    `主人你看！${name}的尾巴在摇呢~`,
    `${name}刚刚发现窗外来了一只小鸟！`,
    `主人工作辛苦啦，${name}给你加油！`,
    `今天天气真好，适合跟主人一起晒太阳~`,
    `${name}学会了一个新技能——转圈圈！`,
    `主人，你说森林那边有什么呢？`
  ];
  return lines[Math.floor(Math.random() * lines.length)];
}

function fallbackEvent(petState) {
  const events = [
    { title: '草丛里的闪光', story: '散步时在草丛里发现了一枚闪闪发光的硬币！', reward: { coins: 15, exp: 20 }, mood: '🤩' },
    { title: '迷路的小蝴蝶', story: '一只迷路的小蝴蝶停在鼻尖上，你帮它找到了回家的路。', reward: { coins: 10, exp: 25 }, mood: '🦋' },
    { title: '下雨啦！', story: '突然下起了花瓣雨，原来是风婆婆的礼物。', reward: { coins: 20, exp: 30 }, mood: '🌸' },
    { title: '神秘食谱', story: '在老树洞里发现了一张写满字的食谱，上面画着奇怪的图案...', reward: { coins: 25, exp: 35 }, mood: '📜' },
    { title: '新朋友', story: '一只小猫咪路过，你们成了好朋友，它送了你一条小鱼干！', reward: { coins: 12, exp: 18 }, mood: '🐱' },
    { title: '彩虹桥', story: '雨后出现了一座小彩虹，你沿着彩虹桥滑了下来！', reward: { coins: 30, exp: 40 }, mood: '🌈' },
    { title: '地精的礼物', story: '一只害羞的小地精从土里钻出来，塞给你一颗糖就跑了。', reward: { coins: 18, exp: 22 }, mood: '🍬' },
    { title: '星星碎片', story: '一颗流星落在院子里，碎片散发着温暖的光芒。', reward: { coins: 35, exp: 45 }, mood: '⭐' }
  ];
  return events[Math.floor(Math.random() * events.length)];
}

function fallbackMiniGame(petState, type) {
  const games = {
    feed: { recipeName: '七彩糯米团子', description: '把各种颜色的食材揉进糯米团里，又好看又好吃！', bonusTip: '金色食材得分最高' },
    play: { gameName: '泡泡追逐赛', scene: '草地上飘满了彩色泡泡，谁追上最多谁就赢！', petMood: '🤪' },
    sleep: { dreamScene: '一片棉花糖云上，周围漂浮着星星形状的小绵羊...', lullaby: '小宝贝快睡吧，月亮婆婆在窗外看着你呢...' },
    clean: { bubbleStyle: '彩虹色的肥皂泡泡，每个泡泡里都有一只小彩虹！', petReaction: '咕噜咕噜转圈圈，想咬泡泡但咬不到', funnyMoment: '一个巨大泡泡把整只宠物包了进去，飘了起来！' }
  };
  return games[type] || games.feed;
}

// ========== 启动服务 ==========
const PORT = process.env.PORT || 3456;
app.listen(PORT, '0.0.0.0', () => {
  console.log(`🐾 虚拟宠物 AI 服务运行在 http://localhost:${PORT}`);
  console.log(`   API Key: ${process.env.ANTHROPIC_API_KEY ? '✅ 已配置' : '⚠️ 未配置（使用本地兜底）'}`);
});
