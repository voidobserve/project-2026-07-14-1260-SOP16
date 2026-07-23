#ifndef __PWM_HANDLE_H__
#define __PWM_HANDLE_H__

#include "typedef.h"

/*
	在遥控器控制开关灯时，是否要立即调节
	0：不立即调节，而是通过定时中断缓慢调节
	1：立即调节，开灯的时候立即调节至期望值，关灯的时候立即调节至0
*/
#define PWM_ON_OR_OFF_IMMEDIATELY_ENABLE 1

/*
	呼吸时，呼吸的范围，0 ~ PWM_BREATH_PROGRESS_MAX
	数值越小，呼吸时间越短
*/
#define PWM_BREATH_PROGRESS_MAX ((u16)1000 * 5 / 10)

#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
/*
	缓慢开机时（不是刚上电时的开机）或者关机时，其中的范围，0 ~ PWM_ON_OR_OFF_PROGRESS_MAX
	数值越小，关机时间越短
*/
#define PWM_ON_OR_OFF_PROGRESS_MAX ((u16)1000 * 5)
#endif

enum
{
	PWM_MODE_OFF = 0,	  // 遥控器控制的关灯
	PWM_MODE_ON,		  // 遥控器控制的开灯
	PWM_MODE_PWR_ON_ANIM, // 正在 上电的开机缓启动 动画中
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

// 缓慢开机、缓慢关机动画的状态
enum
{
	PWM_ON_OR_OFF_STA_INIT = 0, // 刚进入开机或者关机动画，需要初始化
	PWM_ON_OR_OFF_PROCESSING,	// 正在开机或关机动画中
};

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

	// 正常工作模式下，对应的参数：
	u8 brightness_lev; // 亮度等级
	u8 color_idx;	   // 颜色索引 

	// 呼吸、缓慢开机、缓慢关机模式下，使用的状态机
	u8 cur_mode_sta; 
} pwm_handle_param_t;
extern volatile pwm_handle_param_t pwm_handle_param;

// void pwm_handle_param_init(void);
void pwm_handle_100us_isr(void); // 100us中断调用

void pwm_handle_update_dest_pwm_val(void);
void pwm_handle_update_expect_pwm_val(void);

#endif
