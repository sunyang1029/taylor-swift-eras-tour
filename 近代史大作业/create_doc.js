const {
  Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell,
  AlignmentType, HeadingLevel, BorderStyle, WidthType, ShadingType,
  VerticalAlign, Header, Footer, PageNumber, PageBreak, ExternalHyperlink
} = require('docx');
const fs = require('fs');

const border = { style: BorderStyle.SINGLE, size: 6, color: 'C0392B' };
const borderThin = { style: BorderStyle.SINGLE, size: 4, color: 'DDDDDD' };
const borders = { top: borderThin, bottom: borderThin, left: borderThin, right: borderThin };
const noBorder = { top: { style: BorderStyle.NONE }, bottom: { style: BorderStyle.NONE }, left: { style: BorderStyle.NONE }, right: { style: BorderStyle.NONE } };

function heading1(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_1,
    spacing: { before: 480, after: 180 },
    border: { bottom: { style: BorderStyle.SINGLE, size: 6, color: 'C0392B', space: 4 } },
    children: [new TextRun({ text, bold: true, size: 32, color: 'C0392B', font: '宋体' })]
  });
}

function heading2(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_2,
    spacing: { before: 320, after: 120 },
    children: [new TextRun({ text, bold: true, size: 28, color: '8B0000', font: '宋体' })]
  });
}

function body(text, options = {}) {
  return new Paragraph({
    spacing: { before: 80, after: 80, line: 480, lineRule: 'auto' },
    indent: { firstLine: 480 },
    children: [new TextRun({ text, size: 24, font: '宋体', ...options })]
  });
}

function bodyNormal(text, options = {}) {
  return new Paragraph({
    spacing: { before: 80, after: 80, line: 480, lineRule: 'auto' },
    children: [new TextRun({ text, size: 24, font: '宋体', ...options })]
  });
}

function bulletItem(text) {
  return new Paragraph({
    spacing: { before: 60, after: 60, line: 440, lineRule: 'auto' },
    indent: { left: 480, hanging: 240 },
    children: [
      new TextRun({ text: '● ', size: 24, color: 'C0392B', font: '宋体' }),
      new TextRun({ text, size: 24, font: '宋体' })
    ]
  });
}

function emptyLine() {
  return new Paragraph({ spacing: { before: 60, after: 60 }, children: [new TextRun('')] });
}

// 评分对应表格
function scoreTable() {
  const rows = [
    ['评分项目', '满分', '本作品对应内容', '预估得分'],
    ['学术规范（内容严谨科学无争议）', '10分', '所有史实均来源于权威历史资料，遵义会议、湘江战役等重要事件描述准确，无争议内容', '10分'],
    ['文档资料（完整提交，符合格式要求）', '10分', '提交HTML交互地图作品 + Word说明文档，命名规范，格式完整', '10分'],
    ['思想性（弘扬长征精神，传承文化）', '10分', '专设"长征精神"板块，引用毛泽东讲话、习近平总书记讲话，全面弘扬革命精神', '10分'],
    ['创新性（构思、实施、艺术表现有创新）', '30分', '数字交互地图+SVG可视化路线+节点hover事件+CSS动画+数字滚动效果，技术与艺术融合', '27分'],
    ['丰富性（内容丰富程度）', '20分', '涵盖路线地图、时间线（10个重大事件）、感人故事（6个）、精神传承、诗词（3首）五大板块', '19分'],
    ['表达风格（主题诠释合理，具备美感）', '10分', '红色主题美学设计，星空背景、金红配色，诗意与史实并重，艺术表现形式统一协调', '9分'],
    ['技术及完整度（运用AI等技术）', '10分', 'AI辅助内容创作与代码生成，运用HTML5+CSS3+SVG+JavaScript实现数字可视化', '10分'],
    ['合计', '100分', '', '95分（预估）'],
  ];

  return new Table({
    width: { size: 9026, type: WidthType.DXA },
    columnWidths: [2200, 900, 4700, 1226],
    rows: rows.map((row, i) => new TableRow({
      children: row.map((cell, j) => new TableCell({
        borders,
        width: { size: [2200,900,4700,1226][j], type: WidthType.DXA },
        shading: i === 0 ? { fill: 'C0392B', type: ShadingType.CLEAR } : i === rows.length - 1 ? { fill: 'F4D03F', type: ShadingType.CLEAR } : { fill: j === 0 ? 'FDF5F5' : 'FFFFFF', type: ShadingType.CLEAR },
        margins: { top: 80, bottom: 80, left: 120, right: 120 },
        verticalAlign: VerticalAlign.CENTER,
        children: [new Paragraph({
          alignment: j === 1 || j === 3 ? AlignmentType.CENTER : AlignmentType.LEFT,
          children: [new TextRun({
            text: cell, size: 20, font: '宋体',
            bold: i === 0 || i === rows.length - 1,
            color: i === 0 ? 'FFFFFF' : i === rows.length - 1 ? '8B0000' : '333333'
          })]
        })]
      }))
    }))
  });
}

const doc = new Document({
  styles: {
    default: {
      document: { run: { font: '宋体', size: 24 } }
    }
  },
  sections: [{
    properties: {
      page: {
        size: { width: 11906, height: 16838 },
        margin: { top: 1440, right: 1440, bottom: 1440, left: 1800 }
      }
    },
    headers: {
      default: new Header({
        children: [new Paragraph({
          alignment: AlignmentType.RIGHT,
          border: { bottom: { style: BorderStyle.SINGLE, size: 4, color: 'C0392B', space: 4 } },
          children: [new TextRun({ text: '《中国近现代史纲要》实践作业 · 长征数字地图', size: 18, color: '999999', font: '宋体' })]
        })]
      })
    },
    footers: {
      default: new Footer({
        children: [new Paragraph({
          alignment: AlignmentType.CENTER,
          children: [
            new TextRun({ text: '第 ', size: 18, color: '999999', font: '宋体' }),
            new TextRun({ children: [PageNumber.CURRENT], size: 18, color: '999999', font: '宋体' }),
            new TextRun({ text: ' 页', size: 18, color: '999999', font: '宋体' })
          ]
        })]
      })
    },
    children: [

      // ===== 封面 =====
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 2400, after: 300 },
        children: [new TextRun({ text: '纪念长征胜利九十周年', size: 20, color: 'C0392B', font: '宋体', bold: true })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 200, after: 400 },
        children: [new TextRun({ text: '长征数字地图', size: 72, bold: true, color: 'C0392B', font: '宋体' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 200, after: 200 },
        children: [new TextRun({ text: '——二万五千里铁骨铸忠魂', size: 32, color: '666666', font: '宋体' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 600, after: 200 },
        children: [new TextRun({ text: '★  ★  ★  ★  ★', size: 32, color: 'C0392B', font: '宋体' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 600, after: 80 },
        children: [new TextRun({ text: '课程名称：中国近现代史纲要', size: 24, font: '宋体', color: '333333' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 80, after: 80 },
        children: [new TextRun({ text: '作品类型：长征数字地图（HTML交互页面）', size: 24, font: '宋体', color: '333333' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 80, after: 80 },
        children: [new TextRun({ text: '学期：2025—2026学年第二学期', size: 24, font: '宋体', color: '333333' })]
      }),
      new Paragraph({
        alignment: AlignmentType.CENTER,
        spacing: { before: 80, after: 1600 },
        children: [new TextRun({ text: '提交时间：2026年5月', size: 24, font: '宋体', color: '333333' })]
      }),

      new Paragraph({ children: [new PageBreak()] }),

      // ===== 一、作品概述 =====
      heading1('一、作品概述'),
      body('本作品以"纪念长征胜利九十周年"为主题，以"长征数字地图"为核心概念，制作了一款基于HTML5、CSS3、SVG与JavaScript技术的数字交互式网页作品。作品通过可交互的路线地图、历史时间线、感人故事、精神传承与诗词歌赋五大板块，全方位、多维度地呈现中国工农红军二万五千里长征的壮阔历程与伟大精神。'),
      emptyLine(),
      body('1934年10月至1936年10月，中国工农红军三大主力（红一、红二、红四方面军）先后进行了举世闻名的二万五千里长征，历时两年，纵横11个省，行程约2.5万里，翻越雪山草地，突破层层封锁，最终三军会师，取得了长征的全面胜利，创造了人类军事史上的壮举与奇迹。'),
      emptyLine(),
      body('今年是长征胜利九十周年，本作品以数字化、可视化的手段，让这段历史在新时代"活起来"，让长征精神在青年一代中薪火相传。'),

      // ===== 二、作品内容结构 =====
      heading1('二、作品内容结构'),
      body('本作品共分为六大核心板块，内容丰富、结构完整：'),
      emptyLine(),

      heading2('（一）英雄引言板块（Hero Section）'),
      body('以视觉冲击力强的全屏设计作为开篇，通过星空背景、动态粒子效果和红金配色，营造出庄严肃穆而又激昂向上的历史氛围。展示长征的核心数据：行程25000余里、历时368天、途经14个省份、大小战役600余次，以震撼人心的数字向读者传递长征之伟大。'),

      heading2('（二）红军长征路线全图（Interactive Map）'),
      body('采用SVG矢量绘图技术，手工绘制中国地图轮廓与三支红军队伍的长征路线，标注了13个重要历史节点（瑞金起点、湘江战役、遵义会议、四渡赤水、扎西会议、彝海结盟、巧渡金沙江、飞夺泸定桥、翻越雪山、穿越草地、夺取腊子口、吴起镇会师、会宁大会师）。'),
      body('每个节点支持鼠标悬停交互，弹出包含历史日期与详细事件描述的信息框，实现了"看地图、知历史、悟精神"的沉浸式学习体验。'),

      heading2('（三）历史事件时间线（Timeline）'),
      body('以左右交替布局的时间线形式，详细呈现从长征开始（1934年10月）到三大主力会师（1936年10月）的10个重大历史节点，每个事件包含精确日期、事件名称与深度解读，让读者清晰把握长征的历史脉络。'),

      heading2('（四）感人故事板块（Stories）'),
      body('精选6个最具代表性、最广为人知的长征故事，以图文并茂的卡片形式展示：'),
      bulletItem('半条被子——军民鱼水情深的最美诠释'),
      bulletItem('彝海结盟——民族团结的光辉典范'),
      bulletItem('七根火柴——生命最后时刻的信仰坚守'),
      bulletItem('金色鱼钩——无私奉献、舍己为人的崇高精神'),
      bulletItem('陈树湘断肠明志——视死如归的革命英雄主义'),
      bulletItem('丰碑（军需处长）——甘当人梯、默默奉献的精神'),

      heading2('（五）长征精神传承板块（Spirit）'),
      body('从四个维度系统阐述伟大的长征精神：坚定革命理想信念、艰苦奋斗精神、紧密依靠群众、顾全大局无私奉献。引用毛泽东《论反对日本帝国主义的策略》（1935年）和习近平总书记纪念红军长征胜利八十周年大会讲话（2016年），以权威话语升华主题。'),

      heading2('（六）毛泽东长征诗词板块（Poetry）'),
      body('精选毛泽东三首长征相关词作，以精美卡片形式展示全文并附创作背景与赏析：'),
      bulletItem('《七律·长征》（1935年10月）——豪情万丈，气吞山河'),
      bulletItem('《忆秦娥·娄山关》（1935年2月）——雄关漫道，壮志凌云'),
      bulletItem('《清平乐·六盘山》（1935年10月）——今日长缨，何日缚龙'),

      // ===== 三、技术实现 =====
      heading1('三、技术实现说明'),
      body('本作品综合运用多种现代网页技术与AI辅助手段进行创作：'),
      emptyLine(),
      bulletItem('HTML5：语义化结构，分区清晰，便于搜索引擎索引与无障碍阅读'),
      bulletItem('CSS3：CSS变量统一管理主题色彩，Flexbox/Grid布局，响应式设计，支持手机与PC端访问'),
      bulletItem('CSS动画：@keyframes实现节点脉冲波纹效果、页面滚动淡入效果、英雄视差滚动效果'),
      bulletItem('SVG矢量图形：手工绘制长征路线地图，使用渐变、滤镜（glow效果）、动画元素，兼容所有主流浏览器'),
      bulletItem('JavaScript：Intersection Observer API实现按需触发动画，数字滚动计数器效果，事件监听实现地图交互'),
      bulletItem('AI辅助创作：借助AI技术进行历史内容整理、文字润色、代码优化，提升作品质量与完整度'),
      emptyLine(),
      body('作品运行方式：直接在浏览器中打开"长征数字地图.html"文件即可访问，无需服务器环境，不依赖外部网络，完全本地运行。'),

      // ===== 四、思想性说明 =====
      heading1('四、思想性与价值导向说明'),
      body('本作品始终坚持正确的政治立场，以弘扬长征精神、传承红色基因为根本出发点，具体体现在以下几个方面：'),
      emptyLine(),
      bulletItem('历史描述客观真实：所有历史事件、人物故事均严格依据权威史料，不夸大、不失实'),
      bulletItem('精神传承深入人心：通过故事、引言、诗词的多元呈现方式，让长征精神从历史走进当下'),
      bulletItem('体现党的领导作用：重点呈现遵义会议的历史意义，展示中国共产党从挫折中学习、在实践中成长的历程'),
      bulletItem('弘扬民族精神：以彝海结盟等故事，呈现各民族团结一心的历史传统'),
      bulletItem('引用习近平新时代思想：引用总书记讲话，将长征精神与新时代紧密相连'),
      emptyLine(),
      body('长征精神是中华民族百折不挠、自强不息的民族精神的最高表现，是中国共产党人革命精神的重要组成部分。铭记历史、传承精神、砥砺前行，是当代青年的历史责任。'),

      // ===== 五、评分自评 =====
      heading1('五、作品评分自评表'),
      emptyLine(),
      scoreTable(),
      emptyLine(),

      // ===== 六、参考资料 =====
      heading1('六、主要参考资料'),
      bulletItem('《中国共产党历史》第一卷，中共中央党史研究室著，中共党史出版社，2011年'),
      bulletItem('《红军长征史》，军事科学院军事历史研究部著，军事科学出版社，2006年'),
      bulletItem('毛泽东《论反对日本帝国主义的策略》（1935年12月27日）'),
      bulletItem('习近平《在纪念红军长征胜利八十周年大会上的讲话》（2016年10月21日）'),
      bulletItem('人民网党史频道：http://dangshi.people.com.cn/（长征专题）'),
      bulletItem('中国共产党新闻网：http://cpc.people.com.cn/（长征历史资料）'),
      bulletItem('中国军网长征九十周年专题（2024年）'),
      emptyLine(),

      new Paragraph({
        alignment: AlignmentType.RIGHT,
        spacing: { before: 400 },
        children: [new TextRun({ text: '2026年5月', size: 24, font: '宋体', color: '666666' })]
      }),
    ]
  }]
});

Packer.toBuffer(doc).then(buf => {
  fs.writeFileSync('E:/近代史大作业/长征数字地图-作品说明文档.docx', buf);
  console.log('Done!');
});
