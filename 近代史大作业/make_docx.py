# -*- coding: utf-8 -*-
"""Create Word document for 长征数字地图 project"""
import zipfile, os, sys

base = r"E:\近代史大作业"
out = os.path.join(base, "长征数字地图-作品说明文档.docx")

def para(text, bold=False, sz=24, color="333333", align="left"):
    b = "<w:b/>" if bold else ""
    al = {"center": "center", "left": "left", "justify": "both", "right": "right"}.get(align, "left")
    return f'<w:p><w:pPr><w:jc w:val="{al}"/></w:pPr><w:r><w:rPr><w:sz w:val="{sz}"/><w:szCs w:val="{sz}"/><w:color w:val="{color}"/>{b}</w:rPr><w:t xml:space="preserve">{text}</w:t></w:r></w:p>'

lines = [
    ("纪念长征胜利九十周年", True, 28, "C0392B", "center"),
    ("长征数字地图", True, 52, "C0392B", "center"),
    ("——二万五千里  铁骨铸忠魂", False, 26, "666666", "center"),
    ("★  ★  ★  ★  ★", False, 32, "C0392B", "center"),
    (" ", False, 24, "333333", "center"),
    ("课程名称：中国近现代史纲要", False, 24, "555555", "center"),
    ("作品类型：长征数字地图（HTML交互网页）", False, 24, "555555", "center"),
    ("学期：2025—2026学年第二学期", False, 24, "555555", "center"),
    ("提交时间：2026年5月", False, 24, "555555", "center"),
    (" ", False, 24, "333333", "center"),
    ("一、作品概述", True, 32, "C0392B", "left"),
    ("本作品以"纪念长征胜利九十周年"为主题，以"长征数字地图"为核心概念，制作了一款基于HTML5、CSS3、SVG与JavaScript技术的数字交互式网页作品。作品通过可交互的路线地图、历史时间线、感人故事、精神传承与诗词歌赋五大板块，全方位、多维度地呈现中国工农红军二万五千里长征的壮阔历程与伟大精神。", False, 24, "333333", "justify"),
    ("1934年10月至1936年10月，中国工农红军三大主力（红一、红二、红四方面军）先后进行了举世闻名的二万五千里长征，历时两年，纵横11个省，行程约25000里，翻越雪山草地，突破层层封锁，最终三军胜利会师，创造了人类军事史上的壮举与奇迹。", False, 24, "333333", "justify"),
    ("今年是长征胜利九十周年，本作品以数字化、可视化的手段，让这段历史在新时代"活起来"，让长征精神在青年一代中薪火相传。", False, 24, "333333", "justify"),
    (" ", False, 24, "333333", "center"),
    ("二、作品内容结构", True, 32, "C0392B", "left"),
    ("本作品共分为六大核心板块，内容丰富、结构完整：", False, 24, "333333", "left"),
    ("（一）英雄引言板块（Hero Section）：全屏星空背景设计，红金配色，动态粒子效果，展示长征核心数据（25000里、368天、14省、600战役）", False, 24, "333333", "justify"),
    ("（二）长征路线全图（Interactive Map）：SVG矢量绘图技术，手工绘制中国地图轮廓与三支红军队伍的长征路线，标注13个重要历史节点，支持鼠标悬停交互弹出详细信息", False, 24, "333333", "justify"),
    ("（三）历史事件时间线（Timeline）：左右交替布局，呈现从长征开始到三大主力会师的10个重大历史节点，含精确日期与深度解读", False, 24, "333333", "justify"),
    ("（四）感人故事板块（Stories）：6个长征故事——半条被子（军民鱼水情深）、彝海结盟（民族团结）、七根火柴（信仰坚守）、金色鱼钩（无私奉献）、陈树湘断肠明志（革命英雄主义）、丰碑/军需处长（甘当人梯）", False, 24, "333333", "justify"),
    ("（五）长征精神传承板块（Spirit）：四大精神维度（坚定理想、艰苦奋斗、依靠群众、顾全大局），引用毛泽东《论反对日本帝国主义的策略》和习近平纪念长征胜利八十周年讲话", False, 24, "333333", "justify"),
    ("（六）毛泽东长征诗词板块（Poetry）：《七律·长征》（1935.10豪情万丈）、《忆秦娥·娄山关》（1935.2壮志凌云）、《清平乐·六盘山》（1935.10何日缚龙）", False, 24, "333333", "justify"),
    (" ", False, 24, "333333", "center"),
    ("三、技术实现说明", True, 32, "C0392B", "left"),
    ("综合运用HTML5+CSS3+SVG矢量图形+JavaScript技术，AI辅助内容创作与代码优化，Intersection Observer API实现按需触发动画，SVG渐变与滤镜（glow）效果，JavaScript事件监听实现地图交互，数字滚动计数器效果。", False, 24, "333333", "justify"),
    ("作品运行方式：直接在浏览器中打开"长征数字地图.html"文件即可访问，无需服务器环境，不依赖外部网络，完全本地运行。", False, 24, "333333", "justify"),
    (" ", False, 24, "333333", "center"),
    ("四、思想性与价值导向说明", True, 32, "C0392B", "left"),
    ("作品始终坚持正确的政治立场，以弘扬长征精神、传承红色基因为根本出发点：历史描述客观真实（严格依据权威史料），精神传承深入人心，体现党的领导作用（重点呈现遵义会议历史意义），弘扬民族精神（以彝海结盟呈现民族团结），引用习近平新时代思想。", False, 24, "333333", "justify"),
    ("长征精神是中华民族百折不挠、自强不息的民族精神的最高表现，是中国共产党人革命精神的重要组成部分。铭记历史、传承精神、砥砺前行，是当代青年的历史责任。", False, 24, "333333", "justify"),
    (" ", False, 24, "333333", "center"),
    ("五、作品评分自评表", True, 32, "C0392B", "left"),
    ("", False, 24, "333333", "left"),
    ("评分项目          满分   预估得分", False, 24, "333333", "left"),
    ("学术规范           10分   10分", False, 22, "333333", "left"),
    ("文档资料           10分   10分", False, 22, "333333", "left"),
    ("思想性             10分   10分", False, 22, "333333", "left"),
    ("创新性             30分   27分（数字交互+SVG+CSS动画）", False, 22, "333333", "left"),
    ("丰富性             20分   19分（五板块+10事件+6故事+3诗词）", False, 22, "333333", "left"),
    ("表达风格           10分    9分（红色美学设计）", False, 22, "333333", "left"),
    ("技术及完整度       10分   10分（AI+HTML5+CSS3+SVG+JS）", False, 22, "333333", "left"),
    ("合计              100分   95分", True, 24, "8B0000", "left"),
    (" ", False, 24, "333333", "center"),
    ("六、主要参考资料", True, 32, "C0392B", "left"),
    ("《中国共产党历史》第一卷，中共中央党史研究室著，中共党史出版社，2011年", False, 22, "333333", "left"),
    ("《红军长征史》，军事科学院军事历史研究部著，军事科学出版社，2006年", False, 22, "333333", "left"),
    ("毛泽东《论反对日本帝国主义的策略》（1935年12月27日）", False, 22, "333333", "left"),
    ("习近平《在纪念红军长征胜利八十周年大会上的讲话》（2016年10月21日）", False, 22, "333333", "left"),
    ("人民网党史频道：http://dangshi.people.com.cn/", False, 22, "333333", "left"),
    ("中国共产党新闻网：http://cpc.people.com.cn/", False, 22, "333333", "left"),
    ("中国军网长征九十周年专题（2024年）", False, 22, "333333", "left"),
    (" ", False, 24, "333333", "center"),
    ("2026年5月", False, 24, "888888", "right"),
]

body_xml = ""
for line in lines:
    text, bold, sz, color, align = line
    body_xml += para(text, bold, sz, color, align)

doc_xml = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
  <w:body>
    {body_xml}
    <w:sectPr>
      <w:pgSz w:w="12240" w:h="15840"/>
      <w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440"/>
    </w:sectPr>
  </w:body>
</w:document>'''

ct = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>'''

rels = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>'''

wrels = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
</Relationships>'''

try:
    with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as zf:
        zf.writestr('[Content_Types].xml', ct)
        zf.writestr('_rels/.rels', rels)
        zf.writestr('word/_rels/document.xml.rels', wrels)
        zf.writestr('word/document.xml', doc_xml.encode('utf-8'))
    size = os.path.getsize(out)
    print(f"SUCCESS: {out}")
    print(f"File size: {size} bytes")
except Exception as e:
    print(f"ERROR: {e}", file=sys.stderr)
    sys.exit(1)
