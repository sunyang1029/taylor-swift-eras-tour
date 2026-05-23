# Install python-docx if needed and create the docx file
$ErrorActionPreference = "Stop"

# Check if python-docx is installed
$docxInstalled = $false
try {
    python -c "import docx" 2>$null
    $docxInstalled = $?
} catch {
    $docxInstalled = $false
}

if (-not $docxInstalled) {
    Write-Host "Installing python-docx..."
    pip install python-docx --quiet
}

# Create Python script to generate docx
$pythonScript = @'
from docx import Document
from docx.shared import Inches, Pt, Cm
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.oxml.ns import qn
from docx.oxml import OxmlElement

def set_cell_shading(cell, color):
    """Set cell background color"""
    shading_elm = OxmlElement('w:shd')
    shading_elm.set(qn('w:fill'), color)
    cell._tc.get_or_add_tcPr().append(shading_elm)

def add_table_row(table, col1, col2, header=False):
    row = table.add_row()
    cell1 = row.cells[0]
    cell2 = row.cells[1]
    cell1.text = col1
    cell2.text = col2
    if header:
        set_cell_shading(cell1, "D5E8F0")
        set_cell_shading(cell2, "D5E8F0")
        for cell in [cell1, cell2]:
            for paragraph in cell.paragraphs:
                for run in paragraph.runs:
                    run.bold = True
    return row

doc = Document()

# Set default font for Chinese text
style = doc.styles['Normal']
style.font.name = 'Microsoft YaHei'
style._element.rPr.rFonts.set(qn('w:eastAsia'), 'Microsoft YaHei')
style.font.size = Pt(12)

# Title
title = doc.add_heading('长征数字地图', level=1)
title.alignment = WD_ALIGN_PARAGRAPH.CENTER

subtitle = doc.add_heading('作品说明文档', level=2)
subtitle.alignment = WD_ALIGN_PARAGRAPH.CENTER

# Horizontal line (using paragraph border)
doc.add_paragraph()

# Section 1: 项目概述
doc.add_heading('一、项目概述', level=2)

doc.add_heading('1.1 项目背景', level=3)
doc.add_paragraph('长征是中国共产党领导的红军为摆脱国民党军队的围剿，于1934年至1936年间进行的战略转移行动。红军行程约两万五千里，跨越了中国西部的雪山、草地等恶劣地区，是人类历史上的伟大壮举。为了让更多人了解这段历史，我们开发了"长征数字地图"这一可视化项目。')

doc.add_heading('1.2 项目目标', level=3)
doc.add_paragraph('本项目旨在通过数字化地图的形式，直观展示长征的路线、各主要事件的发生地点、以及沿途的地理环境，帮助用户更好地理解长征的历史意义和地理分布。')

doc.add_heading('1.3 主要功能', level=3)
items = [
    '交互式地图展示：用户可以通过缩放、拖拽等操作查看长征路线',
    '关键事件标注：在地图上标注重要的历史事件发生地点',
    '时间线浏览：按时间顺序展示长征过程中的关键节点',
    '多媒体内容：结合图片、文字说明，提供丰富的历史资料',
    '路线详情：显示各段路程的距离、地形特征等信息'
]
for item in items:
    p = doc.add_paragraph(style='List Bullet')
    p.add_run(item)

# Section 2: 技术架构
doc.add_heading('二、技术架构', level=2)

doc.add_heading('2.1 前端技术栈', level=3)
table = doc.add_table(rows=1, cols=2)
table.style = 'Table Grid'
table.alignment = WD_TABLE_ALIGNMENT.CENTER

# Header row
hdr_cells = table.rows[0].cells
hdr_cells[0].text = '技术'
hdr_cells[1].text = '用途'
set_cell_shading(hdr_cells[0], "D5E8F0")
set_cell_shading(hdr_cells[1], "D5E8F0")
for cell in hdr_cells:
    for paragraph in cell.paragraphs:
        for run in paragraph.runs:
            run.bold = True

# Data rows
tech_data = [
    ('HTML5', '页面结构'),
    ('CSS3', '样式设计与布局'),
    ('JavaScript', '交互逻辑'),
    ('Leaflet.js', '地图库'),
    ('OpenStreetMap', '底图数据')
]
for tech, use in tech_data:
    row_cells = table.add_row().cells
    row_cells[0].text = tech
    row_cells[1].text = use

doc.add_paragraph()

doc.add_heading('2.2 数据存储', level=3)
storage_items = [
    '采用JSON格式存储长征路线点数据',
    '图片资源使用WebP格式优化加载速度',
    '历史事件数据存储于独立的JSON文件中'
]
for item in storage_items:
    doc.add_paragraph(item, style='List Bullet')

doc.add_heading('2.3 响应式设计', level=3)
doc.add_paragraph('项目支持多种设备访问，包括：')
devices = ['桌面电脑', '平板电脑', '智能手机']
for device in devices:
    doc.add_paragraph(device, style='List Bullet')

# Section 3: 核心功能实现
doc.add_heading('三、核心功能实现', level=2)

doc.add_heading('3.1 地图初始化', level=3)
doc.add_paragraph('地图采用Leaflet开源库进行开发，首先需要初始化地图实例，设置中心点和缩放级别：')
p = doc.add_paragraph()
run = p.add_run('var map = L.map(\'map\').setView([26.0, 105.0], 5);')
run.font.name = 'Consolas'
run.font.size = Pt(10)

doc.add_heading('3.2 路线绘制', level=3)
doc.add_paragraph('使用Polyline将长征的主要路线绘制在地图上，不同阶段使用不同颜色区分：')
p = doc.add_paragraph()
run = p.add_run("var route = L.polyline(coordinates, {color: 'red'}).addTo(map);")
run.font.name = 'Consolas'
run.font.size = Pt(10)

doc.add_heading('3.3 事件标记', level=3)
doc.add_paragraph('使用Marker在地图上标记重要事件，点击可查看详细信息：')
p = doc.add_paragraph()
run = p.add_run("L.marker([25.0, 110.0]).addTo(map)")
run.font.name = 'Consolas'
run.font.size = Pt(10)
p = doc.add_paragraph()
run = p.add_run("    .bindPopup('<b>湘江战役</b><br>1934年11月');")
run.font.name = 'Consolas'
run.font.size = Pt(10)

# Section 4: 长征路线详解
doc.add_heading('四、长征路线详解', level=2)

doc.add_heading('4.1 第一阶段：中央苏区出发（1934年10月）', level=3)
doc.add_paragraph('中央红军从江西瑞金出发，开始战略转移。此时红军约有八万六千人。')
p = doc.add_paragraph()
p.add_run('主要地点：').bold = True
for loc in ['瑞金（江西）', '于都（江西）', '突破第一道封锁线']:
    doc.add_paragraph(loc, style='List Bullet')

doc.add_heading('4.2 第二阶段：湘江战役（1934年11月）', level=3)
doc.add_paragraph('湘江战役是长征中最惨烈的战役之一，红军损失过半，从八万人锐减至三万余人。')
p = doc.add_paragraph()
p.add_run('战役意义：').bold = True
battle_items = [
    '突破了国民党军队的第四道封锁线',
    '确立了毛泽东在红军中的领导地位',
    '为后续的遵义会议埋下伏笔'
]
for item in battle_items:
    doc.add_paragraph(item, style='List Bullet')

doc.add_heading('4.3 第三阶段：遵义会议（1935年1月）', level=3)
doc.add_paragraph('遵义会议是长征的转折点，确立了以毛泽东为代表的正确领导路线。')
p = doc.add_paragraph()
p.add_run('会议成果：').bold = True
meeting_items = [
    '结束了"左"倾教条主义在中央的统治',
    '确立了毛泽东在党和红军中的领导地位',
    '制定了正确的军事路线'
]
for item in meeting_items:
    doc.add_paragraph(item, style='List Bullet')

doc.add_heading('4.4 第四阶段：四渡赤水（1935年1月-3月）', level=3)
doc.add_paragraph('四渡赤水是毛泽东军事指挥艺术的杰作，成功摆脱了敌军的围追堵截。')

doc.add_heading('4.5 第五阶段：强渡大渡河、飞夺泸定桥（1935年5月）', level=3)
doc.add_paragraph('红军克服重重困难，成功渡过大渡河并夺取泸定桥。')

doc.add_heading('4.6 第六阶段：过雪山草地（1935年6月-8月）', level=3)
doc.add_paragraph('红军穿越了夹金山、梦笔山等雪山，以及若尔盖等草地，付出了巨大牺牲。')

doc.add_heading('4.7 第七阶段：胜利会师（1935年10月-1936年10月）', level=3)
doc.add_paragraph('1935年10月：红一方面军与陕北红军会师')
doc.add_paragraph('1936年10月：三大主力红军会师，长征胜利结束')

# Section 5: 数据来源
doc.add_heading('五、数据来源', level=2)
doc.add_paragraph('本项目使用的数据来源包括：')
data_sources = [
    '历史文献：相关长征历史研究著作',
    '地理数据：公开的地理信息数据',
    '学术研究：相关学术论文和研究报告'
]
for i, item in enumerate(data_sources, 1):
    p = doc.add_paragraph(style='List Number')
    p.add_run(item)

doc.add_paragraph('所有数据均经过核实，力求准确反映历史事实。')

# Section 6: 使用说明
doc.add_heading('六、使用说明', level=2)

doc.add_heading('6.1 地图操作', level=3)
operations = [
    '缩放：使用鼠标滚轮或地图右上角的加减按钮',
    '平移：点击并拖动地图',
    '查看详情：点击地图上的标记点'
]
for item in operations:
    doc.add_paragraph(item, style='List Bullet')

doc.add_heading('6.2 时间线功能', level=3)
doc.add_paragraph('点击页面上的时间线部分，可以快速跳转到特定时间点。')

# Section 7: 项目团队
doc.add_heading('七、项目团队', level=2)
table = doc.add_table(rows=1, cols=2)
table.style = 'Table Grid'
table.alignment = WD_TABLE_ALIGNMENT.CENTER

hdr_cells = table.rows[0].cells
hdr_cells[0].text = '姓名'
hdr_cells[1].text = '职责'
set_cell_shading(hdr_cells[0], "D5E8F0")
set_cell_shading(hdr_cells[1], "D5E8F0")
for cell in hdr_cells:
    for paragraph in cell.paragraphs:
        for run in paragraph.runs:
            run.bold = True

team_data = [
    ('项目负责人', '项目整体规划'),
    ('前端开发', '地图功能开发'),
    ('内容编辑', '历史资料整理'),
    ('UI设计', '界面设计')
]
for name, role in team_data:
    row_cells = table.add_row().cells
    row_cells[0].text = name
    row_cells[1].text = role

# Section 8: 总结
doc.add_heading('八、总结', level=2)
doc.add_paragraph('长征数字地图项目通过现代信息技术手段，将长征这一重大历史事件以直观、生动的方式呈现给观众。项目不仅具有教育意义，也为历史研究提供了新的可视化工具。我们希望通过这个项目，让更多人了解长征历史，传承长征精神。')

# Footer
doc.add_paragraph()
doc.add_paragraph()
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
run = p.add_run('文档更新时间：2026年5月')
run.italic = True
run.font.size = Pt(11)

# Save
doc.save(r'E:/近代史大作业/长征数字地图-作品说明文档.docx')
print('DOCX file created successfully!')
'@

# Write the Python script
$scriptPath = "E:/近代史大作业/create_docx.py"
$pythonScript | Out-File -FilePath $scriptPath -Encoding UTF8

Write-Host "Python script created at: $scriptPath"
Write-Host "Now executing the script..."

# Execute using Start-Process with -FilePath (doesn't show window)
try {
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = "python"
    $psi.Arguments = "`"$scriptPath`""
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $process = [System.Diagnostics.Process]::Start($psi)
    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()
    
    Write-Host "Output: $stdout"
    if ($stderr) { Write-Host "Errors: $stderr" }
    Write-Host "Exit code: $($process.ExitCode)"
} catch {
    Write-Host "Error: $_"
}
