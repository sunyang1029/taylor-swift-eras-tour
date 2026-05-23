import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'TemperatureApp', 'Ability onCreate');
  }

  onDestroy(): void {
    hilog.info(0x0000, 'TemperatureApp', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(0x0000, 'TemperatureApp', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/TemperaturePage', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'TemperatureApp',
          'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(0x0000, 'TemperatureApp', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    hilog.info(0x0000, 'TemperatureApp', 'Ability onWindowStageDestroy');
  }
}
