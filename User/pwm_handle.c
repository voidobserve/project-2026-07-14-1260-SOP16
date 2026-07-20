#include "pwm_handle.h"
#include "power_on.h"

volatile pwm_handle_param_t pwm_handle_param;

// void pwm_handle_param_init(void)
// {
// 	pwm_handle_param.dest_mode = PWM_MODE_NORMAL_WORK;
// 	pwm_handle_param.cur_mode = PWM_MODE_PWR_ON_ANIM;

// 	pwm_handle_param.color_idx = PWM_COLOR_IDX_CYAN;
// 	pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_MAX;
// }

#if 0
/**
 * @brief 设置某个通道的占空比值
 *
 * @param pwm_idx PWM通道索引
 *
 * @param pwm_duty_val PWM占空比值
 */
void pwm_handle_set_cur_pwm_x_duty_val(u8 pwm_idx, u16 pwm_duty_val)
{
}

/**
 * @brief 设置所有PWM通道的占空比值
 *
 * @param pwm_duty_val PWM占空比值
 */
void pwm_handle_set_all_cur_pwm_duty_val(u16 pwm_duty_val) // 函数名表示设置所有PWM的当前占空值
{
}

void pwm_mode_set(u8 mode)
{
}
#endif

/**
 * @brief  根据 pwm_handle_param 中的参数 color_idx 和 brightness_lev ，
 * 		设置期望的pwm占空比值
 *
 * @details 根据颜色索引（ color_idx ）和亮度级别（ brightness_lev ）设置PWM占空比
 */
void pwm_handle_refresh_expect_pwm_duty_val(void)
{
	switch (pwm_handle_param.color_idx)
	{
	case PWM_COLOR_IDX_TEMPERATURE_1:
	{
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_1;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_1;
	}
	break;
	// ===========================================================
	case PWM_COLOR_IDX_TEMPERATURE_2:
	{
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_2;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_2;
	}
	break;
	// ===========================================================
	case PWM_COLOR_IDX_TEMPERATURE_3:
	{
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_TEMPERATURE_3;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_TEMPERATURE_3;
	}
	break;
	// ===========================================================
	case PWM_COLOR_IDX_BLUE:
	{
		// 获取最终的目标占空比
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_BLUE;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_BLUE;
	}
	break;
	// ===========================================================
	case PWM_COLOR_IDX_CYAN:
	{
		// 获取最终的目标占空比
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_CYAN;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_CYAN;
	}
	break;
	// ===========================================================
	case PWM_COLOR_IDX_GREEN:
	{
		// 获取最终的目标占空比
		pwm_handle_param.expect_pwm_0_duty_val = PWM_0_DUTY_VAL_IN_COLOR_GREEN;
		pwm_handle_param.expect_pwm_1_duty_val = PWM_1_DUTY_VAL_IN_COLOR_GREEN;
	}
	break;

		// ===========================================================
	default:
	{
		return;
	}
	break;
	}

	switch (pwm_handle_param.brightness_lev)
	{
	case PWM_BRIGHTNESS_LEV_1:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 5 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 5 / 100;
		break;

	case PWM_BRIGHTNESS_LEV_2:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 10 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 10 / 100;
		break;

	case PWM_BRIGHTNESS_LEV_3:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 25 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 25 / 100;
		break;

	case PWM_BRIGHTNESS_LEV_4:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 50 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 50 / 100;
		break;

	case PWM_BRIGHTNESS_LEV_5:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 75 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 75 / 100;
		break;

		// 目前 亮度等级 6 和 最高亮度等级 一样
	case PWM_BRIGHTNESS_LEV_6:
		pwm_handle_param.expect_pwm_0_duty_val = (u32)pwm_handle_param.expect_pwm_0_duty_val * 100 / 100;
		pwm_handle_param.expect_pwm_1_duty_val = (u32)pwm_handle_param.expect_pwm_1_duty_val * 100 / 100;
		break;
	}

	// 根据期望值，得到目标值，再立即当前值设置为目标值，最后立即设置pwm占空比值
	pwm_handle_param.dest_pwm_0_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_0_duty_val);
	pwm_handle_param.dest_pwm_1_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_1_duty_val);
	pwm_handle_param.cur_pwm_0_duty_val = pwm_handle_param.dest_pwm_0_duty_val;
	pwm_handle_param.cur_pwm_1_duty_val = pwm_handle_param.dest_pwm_1_duty_val;
	set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
	set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}

static void pwm_handle_in_normal_work(void)
{
	// =================================================================
	// pwm_channel_0                                               //
	// =================================================================
	if (pwm_handle_param.dest_pwm_0_duty_val > pwm_handle_param.cur_pwm_0_duty_val)
	{
		pwm_handle_param.cur_pwm_0_duty_val++;
	}
	else if (pwm_handle_param.dest_pwm_0_duty_val < pwm_handle_param.cur_pwm_0_duty_val)
	{
		pwm_handle_param.cur_pwm_0_duty_val--;
	}

	// =================================================================
	// pwm_channel_1                                               //
	// =================================================================
	if (pwm_handle_param.dest_pwm_1_duty_val > pwm_handle_param.cur_pwm_1_duty_val)
	{
		pwm_handle_param.cur_pwm_1_duty_val++;
	}
	else if (pwm_handle_param.dest_pwm_1_duty_val < pwm_handle_param.cur_pwm_1_duty_val)
	{
		pwm_handle_param.cur_pwm_1_duty_val--;
	}

	set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
	set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}

static void pwm_handle_in_breath_anim(void)
{
	// static volatile u8 breath_cycle_cnt = 0; // 呼吸灯的周期计数
	static volatile u8 breath_step_cnt = 0;	 // 步进计数

	switch (pwm_handle_param.breath_anim_sta)
	{
	case PWM_BREATH_ANIM_STA_INIT:
		// 刚进入呼吸灯动画

		// breath_cycle_cnt = 0;
		breath_step_cnt = 0;

		if (pwm_handle_param.cur_pwm_0_duty_val || pwm_handle_param.cur_pwm_1_duty_val)
		{
			// 当前灯光是打开的，接下来呼吸渐灭
			pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_DOWN;
		}
		else
		{
			// 当前灯光是关闭的，接下来呼吸渐亮
			pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_UP;
		}
		break;

	case PWM_BREATH_ANIM_STA_UP:
		// 呼吸灯渐亮

		if (pwm_handle_param.cur_pwm_0_duty_val <
			pwm_handle_param.dest_pwm_0_duty_val)
		{
			pwm_handle_param.cur_pwm_0_duty_val++;
		}

		if (pwm_handle_param.cur_pwm_1_duty_val <
			pwm_handle_param.dest_pwm_1_duty_val)
		{
			pwm_handle_param.cur_pwm_1_duty_val++;
		}

		if ((pwm_handle_param.cur_pwm_0_duty_val ==
			 pwm_handle_param.dest_pwm_0_duty_val) &&
			(pwm_handle_param.cur_pwm_1_duty_val ==
			 pwm_handle_param.dest_pwm_1_duty_val))
		{
			// 呼吸灯渐亮完成，接下来呼吸渐灭
			pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_DOWN;
			breath_step_cnt++;
		}

		break;

	case PWM_BREATH_ANIM_STA_DOWN:
		// 呼吸灯渐灭

		if (pwm_handle_param.cur_pwm_0_duty_val > 0)
		{
			pwm_handle_param.cur_pwm_0_duty_val--;
		}

		if (pwm_handle_param.cur_pwm_1_duty_val > 0)
		{
			pwm_handle_param.cur_pwm_1_duty_val--;
		}

		if ((pwm_handle_param.cur_pwm_0_duty_val == 0) &&
			(pwm_handle_param.cur_pwm_1_duty_val == 0))
		{
			// 呼吸灯渐灭完成，接下来呼吸渐亮
			pwm_handle_param.breath_anim_sta = PWM_BREATH_ANIM_STA_UP;
			breath_step_cnt++;
		}

		break;
	}

	if (breath_step_cnt >= (5 * 2))
	{
		// 呼吸灯动画执行完 5次 呼吸，退出呼吸灯动画

		breath_step_cnt = 0;
		// breath_cycle_cnt = 0;

		// 可能是从开机缓启动进入的呼吸灯动画，也可能是从正常工作模式进入的呼吸灯动画
		pwm_handle_param.cur_mode = pwm_handle_param.last_mode;
	}

	// 实时更新PWM硬件输出
	// pwm_handle_param.dest_pwm_0_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_0_duty_val);
	// pwm_handle_param.dest_pwm_1_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_1_duty_val);
	set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
	set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}

void pwm_handle_100us_isr(void)
{
	// 用于控制pwm缓慢变化的时间计数
	static volatile u16 pwm_duty_change_cnt = 0;
	pwm_duty_change_cnt++;

	switch (pwm_handle_param.cur_mode)
	{
	// ===========================================================
	// PWM_MODE_OFF
	// ===========================================================
	case PWM_MODE_OFF:
		// TODO 需要确认关机的时候是要马上关闭，还是缓慢关闭
		break;

	// ===========================================================
	// PWM_MODE_PWR_ON_ANIM
	// ===========================================================
	case PWM_MODE_PWR_ON_ANIM:
		//
		break;

	// ===========================================================
	// PWM_MODE_NORMAL_WORK 正常工作模式
	// ===========================================================
	case PWM_MODE_NORMAL_WORK:

		// if (pwm_duty_change_cnt >= 10) // 1000us,1ms
		// if (pwm_duty_change_cnt >= 1) // 100us（用遥控器调节，在50%以上调节pwm占空比的时候，灯光会有抖动）
		// if (pwm_duty_change_cnt >= 5) // 500us
		// if (pwm_duty_change_cnt >= 10) // x * 100us （用遥控器调节到50%以下pwm占空比的时候，灯光会有抖动）
		if (pwm_duty_change_cnt >= 20) // x * 100us （ 用遥控器调节时，灯光不会有抖动，样机最高功率为870W--加上风扇）
		// if (pwm_duty_change_cnt >= 30) // x * 100us （用遥控器调节时，灯光不会有抖动，但是调节时间过长，感觉不跟手）
		{
			pwm_duty_change_cnt = 0;
			pwm_handle_in_normal_work();
		}
		break;

	// ===========================================================
	// PWM_MODE_BREATH_ANIM
	// ===========================================================
	case PWM_MODE_BREATH_ANIM:

		pwm_handle_in_breath_anim();
 
		break;

	default:

		break;
	}
}