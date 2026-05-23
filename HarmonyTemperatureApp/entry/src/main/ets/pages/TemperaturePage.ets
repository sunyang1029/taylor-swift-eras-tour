/**
 * 温度监控主页面 — ArkUI 实现
 *
 * 模拟 DS18B20 传感器采集 + 滑动平均滤波
 * 实际部署时替换 TempSimulator.read() 为真实硬件驱动（UART/I2C/GPIO）
 */

import { TemperatureFilter } from '../algorithm/TemperatureFilter';

// ── 类型定义 ──
interface LogEntry {
  time: string;
  raw: number;
  filtered: number | null;
  anomaly: boolean;
}

interface WindowState {
  readings: number[];
  readIndex: number;
  average: number;
}

// ── 温度模拟器（实际设备上替换为传感器驱动） ──
class TempSimulator {
  private base: number = 25;
  private trend: number = 0;

  read(): number {
    this.trend += (Math.random() - 0.5) * 0.03;
    this.trend = Math.max(-2, Math.min(2, this.trend));
    const noise = (Math.random() - 0.5) * 3.0;
    if (Math.random() < 0.015) {
      return Math.random() < 0.5 ? -127.0 : 85.0;
    }
    return +(this.base + this.trend + noise).toFixed(2);
  }
}

// ── 页面入口 ──
@Entry
@Component
struct TemperaturePage {
  @State filteredTemp: number = 0;
  @State rawTemp: number = 0;
  @State avgTemp: number = 0;
  @State anomalies: number = 0;
  @State samples: number = 0;
  @State isRunning: boolean = true;
  @State logs: LogEntry[] = [];
  @State windowState: WindowState = {
    readings: [0, 0, 0, 0, 0],
    readIndex: 0,
    average: 0
  };

  private filter: TemperatureFilter = new TemperatureFilter(5);
  private simulator: TempSimulator = new TempSimulator();
  private timer: number = -1;

  aboutToAppear(): void { this.start(); }
  aboutToDisappear(): void { clearInterval(this.timer); }

  start(): void {
    this.timer = setInterval(() => { this.tick(); }, 1000);
  }
  stop(): void { clearInterval(this.timer); }

  tick(): void {
    const now = new Date();
    const timeStr = now.toTimeString().slice(0, 8);
    const raw = this.simulator.read();
    const result = this.filter.filter(raw);
    this.samples++;

    if (result.anomaly) {
      this.anomalies++;
      this.logs.unshift({ time: timeStr, raw, filtered: null, anomaly: true });
    } else {
      this.filteredTemp = result.filtered;
      this.avgTemp = result.filtered;
      this.logs.unshift({ time: timeStr, raw, filtered: result.filtered, anomaly: false });
    }

    this.rawTemp = raw;
    this.windowState = this.filter.getWindow();
    if (this.logs.length > 50) this.logs = this.logs.slice(0, 50);
  }

  resetAll(): void {
    this.filter.reset();
    this.samples = 0;
    this.anomalies = 0;
    this.logs = [];
    this.filteredTemp = 0;
    this.rawTemp = 0;
    this.avgTemp = 0;
    this.windowState = this.filter.getWindow();
  }

  toggleRunning(): void {
    if (this.isRunning) { this.stop(); } else { this.start(); }
    this.isRunning = !this.isRunning;
  }

  tempStatusColor(): string {
    if (this.filteredTemp > 30) return '#FF9500';
    if (this.filteredTemp < 15) return '#0A59F7';
    return '#34C759';
  }

  tempStatusText(): string {
    if (this.filteredTemp > 30) return '温度偏高';
    if (this.filteredTemp < 15) return '温度偏低';
    return '正常范围';
  }

  // ═══════════════════════════════════════════
  // UI
  // ═══════════════════════════════════════════
  build() {
    Column() {
      this.buildHeader();
      Scroll() {
        Column() {
          this.buildTempHero();
          this.buildStatsRow();
          this.buildWindowView();
          this.buildLogView();
        }.padding({ left: 16, right: 16, bottom: 32 })
      }
      .layoutWeight(1)
      .scrollBar(BarState.Off)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#0a0e1a')
  }

  @Builder buildHeader() {
    Row() {
      Row() {
        Text('🌡️').fontSize(20).margin({ right: 8 })
        Column() {
          Text('DS18B20 温度监控')
            .fontSize(17).fontWeight(FontWeight.Bold).fontColor('#e8eaed')
          Text('ESP32 · GPIO 5 · 滑动平均滤波')
            .fontSize(11).fontColor('#9aa0a6')
        }.alignItems(HorizontalAlign.Start)
      }
      Blank()
      Row() {
        Circle({ width: 7, height: 7 }).fill('#34C759')
        Text('运行中').fontSize(12).fontColor('#34C759').margin({ left: 4 })
      }
      .padding({ left: 10, right: 14, top: 6, bottom: 6 })
      .borderRadius(20)
      .backgroundColor('rgba(52,199,89,0.12)')
      .border({ width: 1, color: 'rgba(52,199,89,0.2)' })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 12, bottom: 12 })
    .backgroundColor('rgba(255,255,255,0.03)')
    .borderRadius(16)
    .margin({ top: 16, left: 16, right: 16 })
  }

  @Builder buildTempHero() {
    Row() {
      Column() {
        Text('算法滤波温度').fontSize(12).fontColor('#9aa0a6').margin({ bottom: 8 })
        Text(this.filteredTemp.toFixed(1))
          .fontSize(64).fontWeight(FontWeight.Bold).fontColor('#e8eaed')
        Text('℃').fontSize(24).fontColor('#9aa0a6').margin({ top: 4 })
        Text(this.tempStatusText())
          .fontSize(13)
          .fontColor(this.tempStatusColor())
          .margin({ top: 12 })
          .padding({ left: 16, right: 16, top: 6, bottom: 6 })
          .borderRadius(20)
          .backgroundColor(`${this.tempStatusColor()}18`)
          .border({ width: 1, color: `${this.tempStatusColor()}30` })
      }
      .width('46%').justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center).padding(24)

      Column() {
        Text('📈 实时数据').fontSize(13).fontWeight(FontWeight.Medium)
          .fontColor('#e8eaed').margin({ bottom: 12 })
        Text(`原始: ${this.rawTemp.toFixed(2)}℃`)
          .fontSize(14).fontColor('#ff9500').margin({ top: 4 })
        Text(`滤波: ${this.filteredTemp.toFixed(2)}℃`)
          .fontSize(14).fontColor('#0A59F7').margin({ top: 4 })
        Text(`差值: ${(Math.abs(this.rawTemp - this.filteredTemp)).toFixed(2)}℃`)
          .fontSize(13).fontColor('#9aa0a6').margin({ top: 12 })
      }
      .width('54%').padding(20)
    }
    .width('100%')
    .backgroundColor('rgba(255,255,255,0.03)')
    .borderRadius(16)
    .border({ width: 1, color: 'rgba(255,255,255,0.06)' })
    .margin({ top: 16 })
  }

  @Builder buildStatsRow() {
    Row() {
      this.statCard('📊', this.avgTemp.toFixed(2) + ' ℃', '滑动平均 (窗口=5)')
      this.statCard('⚡', this.rawTemp.toFixed(2) + ' ℃', '最新原始采集')
    }.width('100%').margin({ top: 12 })
    Row() {
      this.statCard('🛡️', `${this.anomalies}`, '异常拦截次数', '#FF3B30')
      this.statCard('📡', `${this.samples}`, '总采集次数')
    }.width('100%').margin({ top: 12 })
  }

  @Builder statCard(icon: string, value: string, label: string, valueColor: string = '#e8eaed') {
    Column() {
      Text(icon).fontSize(20).margin({ bottom: 6 })
      Text(value).fontSize(24).fontWeight(FontWeight.Bold).fontColor(valueColor)
      Text(label).fontSize(11).fontColor('#9aa0a6').margin({ top: 4 })
    }
    .width('48%').padding(18)
    .backgroundColor('rgba(255,255,255,0.03)')
    .borderRadius(14)
    .border({ width: 1, color: 'rgba(255,255,255,0.06)' })
    .alignItems(HorizontalAlign.Center)
    .margin({ left: '1%', right: '1%' })
  }

  @Builder buildWindowView() {
    Column() {
      Text('🪟 滑动窗口 (窗口大小 = 5)')
        .fontSize(13).fontWeight(FontWeight.Medium).fontColor('#e8eaed').margin({ bottom: 14 })
      Row() {
        ForEach(this.windowState.readings, (val: number, idx: number) => {
          Column() {
            Text(`[${idx}]`).fontSize(9).fontColor('#9aa0a6').margin({ bottom: 4 })
            Text(val.toFixed(1))
              .fontSize(16).fontWeight(FontWeight.Bold)
              .fontColor(idx === this.windowState.readIndex ? '#60a5fa' : '#e8eaed')
            Text('℃').fontSize(10).fontColor('#9aa0a6')
          }
          .width(52).height(72).borderRadius(10)
          .backgroundColor(idx === this.windowState.readIndex ?
            'rgba(10,89,247,0.12)' : 'rgba(255,255,255,0.03)')
          .border({
            width: 1.5,
            color: idx === this.windowState.readIndex ?
              '#0A59F7' : 'rgba(255,255,255,0.06)'
          })
        })
      }
      .width('100%').justifyContent(FlexAlign.SpaceEvenly)
      Text(`${this.windowState.average.toFixed(2)} ℃`)
        .fontSize(28).fontWeight(FontWeight.Bold).fontColor('#60a5fa').margin({ top: 14 })
      Text('当前滑动平均值').fontSize(11).fontColor('#9aa0a6').margin({ top: 4 })
    }
    .width('100%').padding(18)
    .backgroundColor('rgba(255,255,255,0.03)')
    .borderRadius(16)
    .border({ width: 1, color: 'rgba(255,255,255,0.06)' })
    .margin({ top: 12 })
    .alignItems(HorizontalAlign.Center)
  }

  @Builder buildLogView() {
    Column() {
      Row() {
        Text('📋 数据日志')
          .fontSize(13).fontWeight(FontWeight.Medium).fontColor('#e8eaed')
        Blank()
        Button('🔄 重置')
          .fontSize(11).backgroundColor('rgba(255,255,255,0.05)')
          .fontColor('#e8eaed').borderRadius(6).height(32)
          .onClick(() => { this.resetAll(); })
        Button(this.isRunning ? '⏸ 暂停' : '▶ 启动')
          .fontSize(11)
          .backgroundColor(this.isRunning ?
            'rgba(255,59,48,0.1)' : 'rgba(52,199,89,0.1)')
          .fontColor(this.isRunning ? '#FF3B30' : '#34C759')
          .borderRadius(6).height(32).margin({ left: 8 })
          .onClick(() => { this.toggleRunning(); })
      }.width('100%').margin({ bottom: 10 })

      List() {
        ForEach(this.logs, (log: LogEntry) => {
          ListItem() {
            Row() {
              Text(log.time).fontSize(11).fontColor('#9aa0a6').width(70)
              if (log.anomaly) {
                Text(`⚠️ 异常 ${log.raw}℃ — 已拦截`)
                  .fontSize(12).fontColor('#FF3B30').fontWeight(FontWeight.Medium)
              } else {
                Text(`原始:${log.raw.toFixed(2)}℃ → 滤波:${log.filtered!.toFixed(2)}℃`)
                  .fontSize(12).fontColor('#e8eaed')
              }
            }
            .padding({ top: 8, bottom: 8 })
            .border({ width: { bottom: 0.5 }, color: 'rgba(255,255,255,0.04)' })
          }
          .backgroundColor(log.anomaly ? 'rgba(255,59,48,0.06)' : 'transparent')
        })
      }
      .height(240).borderRadius(8).backgroundColor('rgba(0,0,0,0.15)')
    }
    .width('100%').padding(18)
    .backgroundColor('rgba(255,255,255,0.03)')
    .borderRadius(16)
    .border({ width: 1, color: 'rgba(255,255,255,0.06)' })
    .margin({ top: 12 })
  }
}
