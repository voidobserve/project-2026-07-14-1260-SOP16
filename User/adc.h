#ifndef _ADC_H
#define _ADC_H

#include <stdio.h>
#include <include.h>
#include "pwm.h"
#include <string.h>

// ADC 参考电压 VCCA，单位：mV ，客户测得是4.87V
#define ADC_REF_VOLTAGE_VAL ((u16)4870) // 单位：mV
// #define ADC_REF_VOLTAGE_VAL ((u16)4200) // 只在测试时使用

// tmr1配置成每10ms产生一次中断，计数值加一，
// 这里定义时间对应的计数值
#define TMR1_CNT_5_MINUTES 30000UL // 5min

// 在热敏电阻端检测的电压值与温度对应的关系，电压值单位：mV
#define VOLTAGE_TEMP_75 (3050) // 这一个值在客户那边测试出来是74摄氏度,对应的电压是3.1V（实际测得是在2.73V左右）

// 温度状态定义
enum
{
    TEMP_NORMAL, // 正常温度
    TEMP_75,     // 超过75摄氏度（±5摄氏度）
    // TEMP_75_30MIN, // 超过75摄氏度（±5摄氏度）30min
    TEMP_75_5_MIN, // 超过75摄氏度（±5摄氏度）5min
};

#define ADC_VAL_WHEN_UNSTABLE (2243) // 9脚检测到电压不稳定、发动机功率不足时，对应的的阈值，大于该值就认为不稳定
// #define ADC_VAL_WHEN_UNSTABLE (2438) // 9脚检测到电压不稳定、发动机功率不足时，对应的的阈值，大于该值就认为不稳定
// #define ADC_VAL_WHEN_UNSTABLE (2600) // 9脚检测到电压不稳定、发动机功率不足时，对应的的阈值，大于该值就认为不稳定
// #define ADC_VAL_WHEN_UNSTABLE (2730) // 9脚检测到电压不稳定、发动机功率不足时，对应的的阈值，大于该值就认为不稳定

// 定义检测adc的通道:
enum
{
    ADC_SEL_PIN_NONE = 0,

    ADC_SEL_PIN_ENGINE,
    // ADC_SEL_PIN_KNOB,
    ADC_SEL_PIN_TEMP,
    ADC_SEL_PIN_FAN,
};

enum
{
    ADC_STATUS_NONE = 0,

    ADC_STATUS_SEL_ENGINE_WAITING, // 等待adc稳定
    ADC_STATUS_SEL_ENGINE,         // 切换至检测发动机的通道

    ADC_STATUS_SEL_ENGINE_DONE, // 检测发动机的通道期间要连续检测，这里表示连续检测完成

    // ADC_STATUS_SEL_KNOB_WAITING, // 等待adc稳定
    // ADC_STATUS_SEL_KNOB,         // 切换至检测旋钮的通道

    ADC_STATUS_SEL_GET_TEMP_WAITING, // 等待adc稳定
    ADC_STATUS_SEL_GET_TEMP,         // 切换至检测热敏电阻的通道

    ADC_STATUS_SEL_FAN_DETECT_WAITING, // 等待adc稳定
    ADC_STATUS_SEL_FAN_DETECT,         // 切换至检测风扇的通道
};

extern volatile u8 cur_adc_status; // 状态机，表示当前adc的状态

// 存放温度状态的变量
extern volatile u8 temp_status;

// 标志位，由定时器扫描并累计时间，表示当前风扇是否异常
extern volatile bit flag_tim_scan_fan_is_err;
extern volatile u8 cur_fan_status; // 当前风扇状态

extern volatile u16 adc_val_from_engine; // 存放 从发动机一侧 检测到的ad值
extern volatile u16 adc_val_from_knob;   // 存放 从旋钮一侧 采集到的ad值
extern volatile u16 adc_val_from_temp;   // 存放 从热敏电阻一侧 采集到的ad值
extern volatile u16 adc_val_from_fan;    // 存放 检测风扇一侧 采集到的ad值

void adc_pin_config(void); // adc相关的引脚配置，调用完成后，还未能使用adc

u32 get_voltage_from_pin(void); // 从引脚上采集滤波后的电压值

void temperature_scan(void);
void set_duty(void);

void fan_scan(void);

void adc_config(void);
void adc_channel_sel(u8 adc_sel_pin);

void temperature_overheat_check_1ms_isr(void);

#endif