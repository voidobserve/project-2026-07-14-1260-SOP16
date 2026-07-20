#ifndef __PWM_H__
#define __PWM_H__

#include "user_include.h"
#include "include.h"
#include <stdio.h>

// 定义 pwm 占空比(硬件上是反相之后再点亮灯光，客户那边描述的10%占空比，到了程序上是90%占空比)
#define MAX_PWM_DUTY (6000) // 100%占空比   (SYSCLK 4800 0000 /  8000  == 6000)
enum
{
    PWM_DUTY_100_PERCENT = (u16)MAX_PWM_DUTY,
    PWM_DUTY_82_PERCENT = (u16)((u32)MAX_PWM_DUTY * 82 / 100),
    PWM_DUTY_80_PERCENT = (u16)((u32)MAX_PWM_DUTY * 80 / 100),
    PWM_DUTY_65_PERCENT = (u16)((u32)MAX_PWM_DUTY * 65 / 100),
    PWM_DUTY_60_PERCENT = (u16)((u32)MAX_PWM_DUTY * 60 / 100),
    PWM_DUTY_55_PERCENT = (u16)((u32)MAX_PWM_DUTY * 55 / 100),
    PWM_DUTY_54_PERCENT = (u16)((u32)MAX_PWM_DUTY * 54 / 100),
    PWM_DUTY_50_PERCENT = (u16)((u32)MAX_PWM_DUTY * 50 / 100),
    PWM_DUTY_45_PERCENT = (u16)((u32)MAX_PWM_DUTY * 45 / 100),
    PWM_DUTY_42_PERCENT = (u16)((u32)MAX_PWM_DUTY * 42 / 100),
    PWM_DUTY_40_PERCENT = (u16)((u32)MAX_PWM_DUTY * 40 / 100),
    PWM_DUTY_30_PERCENT = (u16)((u32)MAX_PWM_DUTY * 30 / 100),
    PWM_DUTY_25_PERCENT = (u16)((u32)MAX_PWM_DUTY * 25 / 100),
    PWM_DUTY_20_PERCENT = (u16)((u32)MAX_PWM_DUTY * 20 / 100),
    PWM_DUTY_0_PERCENT = (u16)((u32)MAX_PWM_DUTY * 0 / 100),
};
// 传参 x 为百分比，范围：0 ~ 100
#define PWM_DUTY_X_PERCENT(x) ((u16)((u32)MAX_PWM_DUTY * (x) / 100))

enum
{
    PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_1 = PWM_DUTY_X_PERCENT(26),
    PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_1 = PWM_DUTY_X_PERCENT(83),

    PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_2 = PWM_DUTY_X_PERCENT(65),
    PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_2 = PWM_DUTY_X_PERCENT(45),

    PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_3 = PWM_DUTY_X_PERCENT(82),
    PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_3 = PWM_DUTY_X_PERCENT(25),

    PWM_0_DUTY_VAL_IN_COLOR_BLUE = PWM_DUTY_X_PERCENT(100),
    PWM_1_DUTY_VAL_IN_COLOR_BLUE = PWM_DUTY_X_PERCENT(0),

    PWM_0_DUTY_VAL_IN_COLOR_CYAN = PWM_DUTY_X_PERCENT(42),
    PWM_1_DUTY_VAL_IN_COLOR_CYAN = PWM_DUTY_X_PERCENT(65),

    PWM_0_DUTY_VAL_IN_COLOR_GREEN = PWM_DUTY_X_PERCENT(0),
    PWM_1_DUTY_VAL_IN_COLOR_GREEN = PWM_DUTY_X_PERCENT(100),
};

#if 0
// 定义 pwm 模式
enum
{
    PWM_MODE_COLOR_TEMPERATURE_1, // 色温1
    PWM_MODE_COLOR_TEMPERATURE_2,
    PWM_MODE_COLOR_TEMPERATURE_3, 

    PWM_MODE_COLOR_BLUE,  // 蓝光
    PWM_MODE_COLOR_CYAN,  // 青光
    PWM_MODE_COLOR_GREEN, // 绿光

    // 亮度等级
    PWM_MODE_BRIGHTNESS_LEV_1,
    PWM_MODE_BRIGHTNESS_LEV_2,
    PWM_MODE_BRIGHTNESS_LEV_3,
    PWM_MODE_BRIGHTNESS_LEV_4,
    PWM_MODE_BRIGHTNESS_LEV_5,
    PWM_MODE_BRIGHTNESS_LEV_6,
};
#endif

// 由温度限制的PWM占空比 （对所有PWM通道都生效）
extern volatile u16 limited_pwm_duty_due_to_temp;
// 由于发动机不稳定，而限制的可以调节到的占空比（对所有PWM通道都生效，默认为最大占空比）
extern volatile u16 limited_pwm_duty_due_to_unstable_engine;
// 由于风扇异常，限制的可以调节到的最大占空比（对所有PWM通道都生效，默认为最大占空比）
extern volatile u16 limited_pwm_duty_due_to_fan_err;

// extern volatile u8 pwm_mode;
// extern volatile u8 pwm_brightness_lev;

void pwm_init(void);

// 电源电压低于170V-AC,启动低压保护，电源电压高于170V-AC，关闭低压保护
void according_pin9_to_adjust_pwm(void);

extern u8 get_pwm_channel_0_status(void); // 获取第一路PWM的运行状态
extern u8 get_pwm_channel_1_status(void); // 获取第二路PWM的运行状态

extern void pwm_channel_0_enable(void);
extern void pwm_channel_0_disable(void);

extern void pwm_channel_1_enable(void);
extern void pwm_channel_1_disable(void);

void set_pwm_channel_0_duty(u16 channel_duty);
void set_pwm_channel_1_duty(u16 channel_duty);

u16 get_pwm_channel_x_adjust_duty(const u16 pwm_adjust_duty);

// void pwm_mode_handle(void);

#endif