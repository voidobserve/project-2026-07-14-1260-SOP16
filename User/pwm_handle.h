#ifndef __PWM_HANDLE_H__
#define __PWM_HANDLE_H__

#include "typedef.h"

enum
{
	PWM_MODE_OFF = 0,
	PWM_MODE_PWR_ON_ANIM, // 正在开机缓启动的动画中
	PWM_MODE_NORMAL_WORK, // 正常工作模式
	PWM_MODE_BREATH_ANIM, // 正在呼吸灯动画中
};
typedef u8 pwm_mode_t;

enum
{
	PWM_COLOR_IDX_TEMPERATURE_1, // 色温1
	PWM_COLOR_IDX_TEMPERATURE_2,
	PWM_COLOR_IDX_TEMPERATURE_3,

	PWM_COLOR_IDX_BLUE,	 // 蓝光
	PWM_COLOR_IDX_CYAN,	 // 青光
	PWM_COLOR_IDX_GREEN, // 绿光
};
typedef u8 pwm_color_idx_t;

// 亮度等级
enum
{
	PWM_BRIGHTNESS_LEV_1,
	PWM_BRIGHTNESS_LEV_2,
	PWM_BRIGHTNESS_LEV_3,
	PWM_BRIGHTNESS_LEV_4,
	PWM_BRIGHTNESS_LEV_5,
	PWM_BRIGHTNESS_LEV_6,

	PWM_BRIGHTNESS_LEV_MAX = PWM_BRIGHTNESS_LEV_6,
};
typedef u8 pwm_brightness_lev_t;

// 呼吸灯动画的状态
enum
{
	PWM_BREATH_ANIM_STA_INIT = 0, // 刚进入呼吸灯动画，需要初始化
	PWM_BREATH_ANIM_STA_UP,		  // 渐亮
	PWM_BREATH_ANIM_STA_DOWN,	  // 渐暗
};
typedef u8 pwm_breath_anim_sta_t;

typedef struct
{
	// u8 dest_mode; // 记录最终要恢复到的模式，在开机缓启动后、呼吸灯动画结束后，恢复到这个模式
	u8 last_mode; // 记录上一次的模式
	u8 cur_mode;

	u16 cur_pwm_0_duty_val;
	u16 cur_pwm_1_duty_val;

	u16 expect_pwm_0_duty_val;
	u16 expect_pwm_1_duty_val;

	u16 dest_pwm_0_duty_val;
	u16 dest_pwm_1_duty_val;

	u8 brightness_lev; // 亮度等级
	u8 color_idx;	   // 颜色索引

	pwm_breath_anim_sta_t breath_anim_sta; // 呼吸灯动画状态
} pwm_handle_param_t;
extern volatile pwm_handle_param_t pwm_handle_param;

// void pwm_handle_param_init(void);
void pwm_handle_100us_isr(void); // 100us中断调用

void pwm_handle_refresh_expect_pwm_duty_val(void);

#endif
