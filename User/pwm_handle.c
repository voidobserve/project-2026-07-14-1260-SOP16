#include "pwm_handle.h"
#include "power_on.h"

#include "user_config.h"

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
 * @brief  根据 pwm_handle_param 中的参数 expect_pwm_x_duty_val ，以及部分限制条件，
 * 		设置 pwm_handle_param 的 dest_pwm_x_duty_val
 *
 */
void pwm_handle_update_dest_pwm_val(void)
{
	pwm_handle_param.dest_pwm_0_duty_val =
		get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_0_duty_val);
	pwm_handle_param.dest_pwm_1_duty_val =
		get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_1_duty_val);
}

/**
 * @brief  根据 pwm_handle_param 中的参数 color_idx 和 brightness_lev ，
 * 		设置期望的pwm占空比值
 *
 * @details 根据颜色索引（ color_idx ）和亮度级别（ brightness_lev ）设置PWM占空比
 */
void pwm_handle_update_expect_pwm_val(void)
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
	pwm_handle_update_dest_pwm_val();
	// pwm_handle_param.dest_pwm_0_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_0_duty_val);
	// pwm_handle_param.dest_pwm_1_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_1_duty_val);
	pwm_handle_param.cur_pwm_0_duty_val = pwm_handle_param.dest_pwm_0_duty_val;
	pwm_handle_param.cur_pwm_1_duty_val = pwm_handle_param.dest_pwm_1_duty_val;
	set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
	set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}

#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
static void pwm_handle_in_mode_on_or_off(void)
{
	static volatile u16 pwm_progress = 0;

	switch (pwm_handle_param.cur_mode_sta)
	{
	case PWM_ON_OR_OFF_STA_INIT:
		// 初始化呼吸灯动画
		pwm_handle_param.cur_mode_sta = PWM_ON_OR_OFF_PROCESSING;

		// 根据当前最大的目标值，设置进度
		if (pwm_handle_param.dest_pwm_0_duty_val >= pwm_handle_param.dest_pwm_1_duty_val)
		{
			pwm_progress =
				(u32)pwm_handle_param.cur_pwm_0_duty_val *
				PWM_ON_OR_OFF_PROGRESS_MAX /
				pwm_handle_param.dest_pwm_0_duty_val;
		}
		else
		{
			pwm_progress =
				(u32)pwm_handle_param.cur_pwm_1_duty_val *
				PWM_ON_OR_OFF_PROGRESS_MAX /
				pwm_handle_param.dest_pwm_1_duty_val;
		}
		break;

	case PWM_ON_OR_OFF_PROCESSING:

		if (pwm_handle_param.cur_mode == PWM_MODE_ON)
		{
			if (pwm_progress < PWM_ON_OR_OFF_PROGRESS_MAX)
			{
				pwm_progress++;
			}
		}
		else // (pwm_handle_param.cur_mode == PWM_MODE_OFF)
		{
			if (pwm_progress > 0)
			{
				pwm_progress--;
			}
		}

		pwm_handle_param.cur_pwm_0_duty_val =
			(u32)pwm_handle_param.dest_pwm_0_duty_val *
			pwm_progress /
			PWM_ON_OR_OFF_PROGRESS_MAX;
		pwm_handle_param.cur_pwm_1_duty_val =
			(u32)pwm_handle_param.dest_pwm_1_duty_val *
			pwm_progress /
			PWM_ON_OR_OFF_PROGRESS_MAX;

		break;

	default:
		break;
	}

	if ((pwm_handle_param.cur_mode == PWM_MODE_ON) &&
		(pwm_progress >= PWM_ON_OR_OFF_PROGRESS_MAX))
	{
		// 开灯动画结束，转到 正常工作模式
		pwm_handle_param.cur_mode = PWM_MODE_NORMAL_WORK;
	}

	set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
	set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);
}
#endif

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
	static volatile u8 breath_step_cnt = 0; // 呼吸步骤计数，每完成一次呼吸会加2
	/*
		呼吸灯的进度，
		范围：0 ~ PWM_BREATH_PROGRESS_MAX ，映射到 dest_pwm_x_duty_val 占空比值的 0 ~ 100%
	*/
	static volatile u16 breath_progress = 0;

	static volatile u8 call_dly_cnt = 0;

	call_dly_cnt++;
	if (call_dly_cnt < 10)
	{
		return;
	}
	call_dly_cnt = 0;

	switch (pwm_handle_param.cur_mode_sta)
	{
	case PWM_BREATH_ANIM_STA_INIT:
		// 刚进入呼吸灯动画
		breath_step_cnt = 0;
		breath_progress = 0; // 初始化进度为0

		// 判断当前是否已经点亮，如果已经点亮，则从中间打断进入呼吸灯动画
		if (pwm_handle_param.cur_pwm_0_duty_val || pwm_handle_param.cur_pwm_1_duty_val)
		{
			pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_DOWN;

			// 根据当前最大的目标值，得到当前呼吸的进度
			if (pwm_handle_param.dest_pwm_0_duty_val >= pwm_handle_param.dest_pwm_1_duty_val)
			{
				breath_progress =
					(u32)pwm_handle_param.cur_pwm_0_duty_val *
					PWM_BREATH_PROGRESS_MAX /
					pwm_handle_param.dest_pwm_0_duty_val;
			}
			else
			{
				breath_progress =
					(u32)pwm_handle_param.cur_pwm_1_duty_val *
					PWM_BREATH_PROGRESS_MAX /
					pwm_handle_param.dest_pwm_1_duty_val;
			}
		}
		else
		{
			// 当前灯光是关闭的，接下来呼吸渐亮
			pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_UP;
		}

#if USER_DEBUG_ENABLE
		printf("breath sta init\n");
		printf("breath progress = %u\n", (u16)breath_progress);
		// P26 = ~P26;
#endif
		break;

	case PWM_BREATH_ANIM_STA_UP:
		// 呼吸灯渐亮
		if (breath_progress < PWM_BREATH_PROGRESS_MAX)
		{
			breath_progress++;
#if USER_DEBUG_ENABLE
			// printf("breath progress = %u\n", (u16)breath_progress);
#endif
		}

		// 根据统一进度，同步计算两路 PWM 的当前占空比
		pwm_handle_param.cur_pwm_0_duty_val =
			(u16)((u32)pwm_handle_param.dest_pwm_0_duty_val *
				  breath_progress / PWM_BREATH_PROGRESS_MAX);
		pwm_handle_param.cur_pwm_1_duty_val =
			(u16)((u32)pwm_handle_param.dest_pwm_1_duty_val *
				  breath_progress / PWM_BREATH_PROGRESS_MAX);

		if (breath_progress >= PWM_BREATH_PROGRESS_MAX)
		{
			// 呼吸灯渐亮完成，接下来呼吸渐灭
			pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_DOWN;
			breath_step_cnt++;

#if USER_DEBUG_ENABLE
			// P26 = ~P26;
#endif
		}

		break;

	case PWM_BREATH_ANIM_STA_DOWN:
		// 呼吸灯渐灭
		if (breath_progress > 0)
		{
			breath_progress--;
#if USER_DEBUG_ENABLE
			// printf("breath progress = %u\n", (u16)breath_progress);
#endif
		}

		// 根据统一进度，同步计算两路 PWM 的当前占空比
		pwm_handle_param.cur_pwm_0_duty_val =
			(u16)((u32)pwm_handle_param.dest_pwm_0_duty_val *
				  breath_progress / PWM_BREATH_PROGRESS_MAX);
		pwm_handle_param.cur_pwm_1_duty_val =
			(u16)((u32)pwm_handle_param.dest_pwm_1_duty_val *
				  breath_progress / PWM_BREATH_PROGRESS_MAX);

		if (breath_progress == 0)
		{
			// 呼吸灯渐灭完成，接下来呼吸渐亮
			pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_UP;
			breath_step_cnt++;

#if USER_DEBUG_ENABLE
			// P26 = ~P26;
#endif
		}

		break;
	}

	if (breath_step_cnt >= (5 * 2))
	{
		// 呼吸灯动画执行完 5次 呼吸，退出呼吸灯动画

		breath_step_cnt = 0;

		// 可能是从开机缓启动进入的呼吸灯动画，也可能是从正常工作模式进入的呼吸灯动画
		pwm_handle_param.cur_mode = pwm_handle_param.last_mode;

#if USER_DEBUG_ENABLE
		printf("breath finish\n");
		// P26 = ~P26;
#endif
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

#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
		pwm_handle_in_mode_on_or_off();
#endif
		break;

	// ===========================================================
	// PWM_MODE_ON
	// ===========================================================
	case PWM_MODE_ON:
#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
		pwm_handle_in_mode_on_or_off();
#endif
		break;

	// ===========================================================
	// PWM_MODE_PWR_ON_ANIM
	// ===========================================================
	case PWM_MODE_PWR_ON_ANIM:
		// 上电对应的开机动画，由主循环处理
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

		P26 = 1;					 // TEST_ONLY
		pwm_handle_in_breath_anim(); // 函数内每次调节刚好会超过100us
		P26 = 0;					 // TEST_ONLY

		break;

	default:

		break;
	}
}