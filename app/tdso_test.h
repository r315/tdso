#ifndef _tdso_test_h_
#define _tdso_test_h_


void TEST_BlinkLed(uint8_t times);
void TEST_SoftPower(void);
void TEST_FrontendSelector(uint8_t *selector);
void TEST_MultiSwitchSlider(int32_t percent);
void TEST_MultiSwitchSliderBorder(uint16_t color);
void TEST_MultiSwitch(void);
void TEST_BatteryVoltage(void);
void TEST_Run(void);
#endif