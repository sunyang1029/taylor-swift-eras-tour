const { Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell,
        HeadingLevel, AlignmentType, BorderStyle, WidthType, ShadingType,
        LevelFormat, PageBreak } = require('docx');
const fs = require('fs');

const border = { style: BorderStyle.SINGLE, size: 1, color: "CCCCCC" };
const borders = { top: border, bottom: border, left: border, right: border };

const doc = new Document({
  styles: {
    default: { document: { run: { font: "Microsoft YaHei", size: 24 } } },
    paragraphStyles: [
      { id: "Heading1", name: "Heading 1", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 36, bold: true, font: "Microsoft YaHei" },
        paragraph: { spacing: { before: 360, after: 240 }, outlineLevel: 0 } },
      { id: "Heading2", name: "Heading 2", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 32, bold: true, font: "Microsoft YaHei" },
        paragraph: { spacing: { before: 300, after: 180 }, outlineLevel: 1 } },
      { id: "Heading3", name: "Heading 3", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 28, bold: true, font: "Microsoft YaHei" },
        paragraph: { spacing: { before: 240, after: 120 }, outlineLevel: 2 } },
    ]
  },
  numbering: {
    config: [
      { reference: "bullets",
        levels: [{ level: 0, format: LevelFormat.BULLET, text: "•", alignment: AlignmentType.LEFT,
          style: { paragraph: { indent: { left: 720, hanging: 360 } } } }] },
      { reference: "numbers",
        levels: [{ level: 0, format: LevelFormat.DECIMAL, text: "%1.", alignment: AlignmentType.LEFT,
          style: { paragraph: { indent: { left: 720, hanging: 360 } } } }] },
    ]
  },
  sections: [{
    properties: {
      page: {
        size: { width: 11906, height: 16838 },
        margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 }
      }
    },
    children: [
      // Title
      new Paragraph({ heading: HeadingLevel.HEADING_1, alignment: AlignmentType.CENTER,
        children: [new TextRun({ text: "长征数字地图", bold: true })] }),
      new Paragraph({ heading: HeadingLevel.HEADING_2, alignment: AlignmentType.CENTER,
        children: [new TextRun({ text: "作品说明文档", bold: true })] }),
      new Paragraph({}),

      // Horizontal line
      new Paragraph({ border: { bottom: { style: BorderStyle.SINGLE, size: 6, color: "000000", space: 1 } }, children: [] }),
      new Paragraph({}),

      // Section 1
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("一、项目概述")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("1.1 项目背景")] }),
      new Paragraph({ children: [new TextRun("长征是中国共产党领导的红军为摆脱国民党军队的围剿，于1934年至1936年间进行的战略转移行动。红军行程约两万五千里，跨越了中国西部的雪山、草地等恶劣地区，是人类历史上的伟大壮举。为了让更多人了解这段历史，我们开发了"长征数字地图"这一可视化项目。")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("1.2 项目目标")] }),
      new Paragraph({ children: [new TextRun("本项目旨在通过数字化地图的形式，直观展示长征的路线、各主要事件的发生地点、以及沿途的地理环境，帮助用户更好地理解长征的历史意义和地理分布。")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("1.3 主要功能")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "交互式地图展示", bold: true }), new TextRun("：用户可以通过缩放、拖拽等操作查看长征路线")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "关键事件标注", bold: true }), new TextRun("：在地图上标注重要的历史事件发生地点")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "时间线浏览", bold: true }), new TextRun("：按时间顺序展示长征过程中的关键节点")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "多媒体内容", bold: true }), new TextRun("：结合图片、文字说明，提供丰富的历史资料")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "路线详情", bold: true }), new TextRun("：显示各段路程的距离、地形特征等信息")] }),
      new Paragraph({}),

      // Section 2
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("二、技术架构")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("2.1 前端技术栈")] }),
      new Table({
        width: { size: 9026, type: WidthType.DXA },
        columnWidths: [4513, 4513],
        rows: [
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, shading: { fill: "D5E8F0", type: ShadingType.CLEAR },
              margins: { top: 80, bottom: 80, left: 120, right: 120 },
              children: [new Paragraph({ children: [new TextRun({ text: "技术", bold: true })] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, shading: { fill: "D5E8F0", type: ShadingType.CLEAR },
              margins: { top: 80, bottom: 80, left: 120, right: 120 },
              children: [new Paragraph({ children: [new TextRun({ text: "用途", bold: true })] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("HTML5")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("页面结构")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("CSS3")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("样式设计与布局")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("JavaScript")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("交互逻辑")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("Leaflet.js")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("地图库")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("OpenStreetMap")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("底图数据")] })] }),
          ]}),
        ]
      }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("2.2 数据存储")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("采用JSON格式存储长征路线点数据")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("图片资源使用WebP格式优化加载速度")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("历史事件数据存储于独立的JSON文件中")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("2.3 响应式设计")] }),
      new Paragraph({ children: [new TextRun("项目支持多种设备访问，包括：")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("桌面电脑")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("平板电脑")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("智能手机")] }),
      new Paragraph({}),

      // Section 3
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("三、核心功能实现")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("3.1 地图初始化")] }),
      new Paragraph({ children: [new TextRun("地图采用Leaflet开源库进行开发，首先需要初始化地图实例，设置中心点和缩放级别：")] }),
      new Paragraph({ border: { left: { style: BorderStyle.SINGLE, size: 12, color: "666666" } },
        indent: { left: 360 },
        children: [new TextRun({ text: "var map = L.map('map').setView([26.0, 105.0], 5);", font: "Consolas", size: 20 })] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("3.2 路线绘制")] }),
      new Paragraph({ children: [new TextRun("使用Polyline将长征的主要路线绘制在地图上，不同阶段使用不同颜色区分：")] }),
      new Paragraph({ border: { left: { style: BorderStyle.SINGLE, size: 12, color: "666666" } },
        indent: { left: 360 },
        children: [new TextRun({ text: "var route = L.polyline(coordinates, {color: 'red'}).addTo(map);", font: "Consolas", size: 20 })] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("3.3 事件标记")] }),
      new Paragraph({ children: [new TextRun("使用Marker在地图上标记重要事件，点击可查看详细信息：")] }),
      new Paragraph({ border: { left: { style: BorderStyle.SINGLE, size: 12, color: "666666" } },
        indent: { left: 360 },
        children: [new TextRun({ text: "L.marker([25.0, 110.0]).addTo(map)", font: "Consolas", size: 20 })] }),
      new Paragraph({ border: { left: { style: BorderStyle.SINGLE, size: 12, color: "666666" } },
        indent: { left: 360 },
        children: [new TextRun({ text: "    .bindPopup('<b>湘江战役</b><br>1934年11月');", font: "Consolas", size: 20 })] }),
      new Paragraph({}),

      // Section 4
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("四、长征路线详解")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.1 第一阶段：中央苏区出发（1934年10月）")] }),
      new Paragraph({ children: [new TextRun("中央红军从江西瑞金出发，开始战略转移。此时红军约有八万六千人。")] }),
      new Paragraph({ children: [new TextRun({ text: "主要地点", bold: true }), new TextRun("：")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("瑞金（江西）")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("于都（江西）")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("突破第一道封锁线")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.2 第二阶段：湘江战役（1934年11月）")] }),
      new Paragraph({ children: [new TextRun("湘江战役是长征中最惨烈的战役之一，红军损失过半，从八万人锐减至三万余人。")] }),
      new Paragraph({ children: [new TextRun({ text: "战役意义", bold: true }), new TextRun("：")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("突破了国民党军队的第四道封锁线")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("确立了毛泽东在红军中的领导地位")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("为后续的遵义会议埋下伏笔")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.3 第三阶段：遵义会议（1935年1月）")] }),
      new Paragraph({ children: [new TextRun("遵义会议是长征的转折点，确立了以毛泽东为代表的正确领导路线。")] }),
      new Paragraph({ children: [new TextRun({ text: "会议成果", bold: true }), new TextRun("：")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("结束了"左"倾教条主义在中央的统治")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("确立了毛泽东在党和红军中的领导地位")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("制定了正确的军事路线")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.4 第四阶段：四渡赤水（1935年1月-3月）")] }),
      new Paragraph({ children: [new TextRun("四渡赤水是毛泽东军事指挥艺术的杰作，成功摆脱了敌军的围追堵截。")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.5 第五阶段：强渡大渡河、飞夺泸定桥（1935年5月）")] }),
      new Paragraph({ children: [new TextRun("红军克服重重困难，成功渡过大渡河并夺取泸定桥。")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.6 第六阶段：过雪山草地（1935年6月-8月）")] }),
      new Paragraph({ children: [new TextRun("红军穿越了夹金山、梦笔山等雪山，以及若尔盖等草地，付出了巨大牺牲。")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("4.7 第七阶段：胜利会师（1935年10月-1936年10月）")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("1935年10月：红一方面军与陕北红军会师")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun("1936年10月：三大主力红军会师，长征胜利结束")] }),
      new Paragraph({}),

      // Section 5
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("五、数据来源")] }),
      new Paragraph({ children: [new TextRun("本项目使用的数据来源包括：")] }),
      new Paragraph({ numbering: { reference: "numbers", level: 0 }, children: [new TextRun({ text: "历史文献", bold: true }), new TextRun("：相关长征历史研究著作")] }),
      new Paragraph({ numbering: { reference: "numbers", level: 0 }, children: [new TextRun({ text: "地理数据", bold: true }), new TextRun("：公开的地理信息数据")] }),
      new Paragraph({ numbering: { reference: "numbers", level: 0 }, children: [new TextRun({ text: "学术研究", bold: true }), new TextRun("：相关学术论文和研究报告")] }),
      new Paragraph({ children: [new TextRun("所有数据均经过核实，力求准确反映历史事实。")] }),
      new Paragraph({}),

      // Section 6
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("六、使用说明")] }),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("6.1 地图操作")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "缩放", bold: true }), new TextRun("：使用鼠标滚轮或地图右上角的加减按钮")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "平移", bold: true }), new TextRun("：点击并拖动地图")] }),
      new Paragraph({ numbering: { reference: "bullets", level: 0 }, children: [new TextRun({ text: "查看详情", bold: true }), new TextRun("：点击地图上的标记点")] }),
      new Paragraph({}),

      new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun("6.2 时间线功能")] }),
      new Paragraph({ children: [new TextRun("点击页面上的时间线部分，可以快速跳转到特定时间点。")] }),
      new Paragraph({}),

      // Section 7
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("七、项目团队")] }),
      new Table({
        width: { size: 9026, type: WidthType.DXA },
        columnWidths: [4513, 4513],
        rows: [
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, shading: { fill: "D5E8F0", type: ShadingType.CLEAR },
              margins: { top: 80, bottom: 80, left: 120, right: 120 },
              children: [new Paragraph({ children: [new TextRun({ text: "姓名", bold: true })] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, shading: { fill: "D5E8F0", type: ShadingType.CLEAR },
              margins: { top: 80, bottom: 80, left: 120, right: 120 },
              children: [new Paragraph({ children: [new TextRun({ text: "职责", bold: true })] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("项目负责人")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("项目整体规划")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("前端开发")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("地图功能开发")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("内容编辑")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("历史资料整理")] })] }),
          ]}),
          new TableRow({ children: [
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("UI设计")] })] }),
            new TableCell({ borders, width: { size: 4513, type: WidthType.DXA }, margins: { top: 80, bottom: 80, left: 120, right: 120 }, children: [new Paragraph({ children: [new TextRun("界面设计")] })] }),
          ]}),
        ]
      }),
      new Paragraph({}),

      // Section 8
      new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun("八、总结")] }),
      new Paragraph({ children: [new TextRun("长征数字地图项目通过现代信息技术手段，将长征这一重大历史事件以直观、生动的方式呈现给观众。项目不仅具有教育意义，也为历史研究提供了新的可视化工具。我们希望通过这个项目，让更多人了解长征历史，传承长征精神。")] }),
      new Paragraph({}),
      new Paragraph({}),
      new Paragraph({ alignment: AlignmentType.RIGHT, children: [new TextRun({ text: "文档更新时间：2026年5月", italics: true, size: 22 })] }),
    ]
  }]
});

Packer.toBuffer(doc).then(buffer => {
  fs.writeFileSync("E:/近代史大作业/长征数字地图-作品说明文档.docx", buffer);
  console.log("DOCX file created successfully!");
});
