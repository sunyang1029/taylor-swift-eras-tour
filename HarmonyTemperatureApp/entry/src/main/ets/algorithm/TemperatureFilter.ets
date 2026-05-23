/**
 * 滑动平均滤波算法 — DS18B20 温度传感器
 *
 * 完全复刻 Arduino 逻辑：
 *  - 异常值拦截：-127.00（传感器未连接）和 85.00（上电默认值）
 *  - 滑动窗口平均滤波：窗口大小可配置，默认 5
 *
 * 用法：
 *   const filter = new TemperatureFilter(5);
 *   const result = filter.filter(rawValue);
 *   if (result.anomaly) { ... 处理异常 ... }
 */

export interface FilterResult {
  filtered: number;
  anomaly: boolean;
}

export interface WindowState {
  readings: number[];
  readIndex: number;
  average: number;
}

export class TemperatureFilter {
  private numReadings: number;
  private readings: number[];
  private readIndex: number = 0;
  private total: number = 0;

  constructor(windowSize: number = 5) {
    this.numReadings = windowSize;
    this.readings = new Array(windowSize).fill(0);
  }

  /**
   * 滤波一个原始温度读数
   * @param rawValue — DS18B20 原始输出（℃）
   * @returns 滤波结果；异常值返回 anomaly=true
   */
  filter(rawValue: number): FilterResult {
    // 核心 1：异常值拦截
    if (rawValue === -127.0 || rawValue === 85.0) {
      return { filtered: -1, anomaly: true };
    }

    // 核心 2：滑动平均滤波
    this.total -= this.readings[this.readIndex];
    this.readings[this.readIndex] = rawValue;
    this.total += this.readings[this.readIndex];
    this.readIndex++;
    if (this.readIndex >= this.numReadings) {
      this.readIndex = 0;
    }

    return {
      filtered: this.total / this.numReadings,
      anomaly: false
    };
  }

  /** 获取当前滑动窗口快照 */
  getWindow(): WindowState {
    return {
      readings: [...this.readings],
      readIndex: this.readIndex,
      average: this.numReadings > 0 ? this.total / this.numReadings : 0
    };
  }

  /** 重置滤波器 */
  reset(): void {
    this.readings = new Array(this.numReadings).fill(0);
    this.readIndex = 0;
    this.total = 0;
  }
}
