#ifndef __USER_INCLUDE_H__
#define __USER_INCLUDE_H__

#include "include.h" // 芯片官方提供的头文件


#define ARRAY_SIZE(n) (sizeof(n) / sizeof(n[0]))

/*
    1脚电压低于4.3V时，14，15脚输出25%占空比，
    1脚电压高于4.5V时，14，15脚输出100%占空比

    检测时使用VCCA作为参考电压，客户测试VCC对GND的电压为4.87V
    那么，使用VCCA作为参考电压，adc 12位精度
    4.30 V 对应的ad值是  4300 / 4870 * 4096  == 3616.59
    4.50 V 对应的ad值是  4500 / 4870 * 4096  == 3784.80
*/
#define ADC_REF_VOLTAGE_FAN ((u16)4870)                                       // 检测风扇一侧的ad值时，使用的参考电压，单位：mV
#define ADC_VAL_WHEN_FAN_ERR (u16)((u32)4300 * 4096 / ADC_REF_VOLTAGE_FAN)    // 风扇异常时，对应的ad值
#define ADC_VAL_WHEN_FAN_NORMAL (u16)((u32)4500 * 4096 / ADC_REF_VOLTAGE_FAN) // 风扇正常时，对应的ad值

// 累计检测风扇工作正常多少时间，才认为是真的工作正常，并更新对应的状态，单位：ms （例如，累计检测风扇正常工作5s，才将风扇的状态更新为工作正常）
#define FAN_SCAN_TIMES_WHEN_NORMAL (8000)
// 累计检测风扇工作异常多少时间，才认为是真的工作异常，并更新对应的状态，单位：ms （例如，累计检测风扇工作异常5s，才将风扇的状态更新为工作异常）
#define FAN_SCAN_TIMES_WHEN_ERR (50)

// 定义风扇状态
enum FAN_STATUS
{
    FAN_STATUS_NORMAL = 0,
    FAN_STATUS_ERROR,
};

#include "flash.h"
#include "pwm.h"
#include "adc.h"
#include "time0.h" // 定时器0

#include "timer2.h"
#include "timer3.h"
#include "knob_dimming.h" // 旋钮调光头文件
// #include "rf_recv.h"
// #include "key_driver.h"
#include "fan_ctl.h"

#include "engine.h" // 发动机

#endif
