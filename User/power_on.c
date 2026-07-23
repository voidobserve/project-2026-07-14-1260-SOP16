#include "power_on.h"
#include "include.h"
#include "pwm.h"
#include "pwm_handle.h"

#include "uart0_handle.h" // 开机缓启动期间，也需要接收串口信号

volatile bit flag_time_comes_during_power_on = 0; // 标志位，开机缓启动期间，调节时间到来（由定时器置位）

/**
 * @brief 上电后的开机动画处理函数
 * 
 */
void power_on_handle(void)
{
    static volatile u32 pwm0_power_on_step = 0;
    static volatile u32 pwm1_power_on_step = 0;

    pwm_handle_param.cur_pwm_0_duty_val = 0;
    pwm_handle_param.cur_pwm_1_duty_val = 0;
    pwm_handle_param.cur_mode = PWM_MODE_PWR_ON_ANIM;

    // 这里要先设置 dest_pwm_x_duty_val，如果在缓启动期间进入呼吸灯动画，要根据这个值来调节
    pwm_handle_param.dest_pwm_0_duty_val = PWM0_DEST_POWER_ON_DUTY_VAL;
    pwm_handle_param.dest_pwm_1_duty_val = PWM1_DEST_POWER_ON_DUTY_VAL;

    while (1)
    {
        if (pwm_handle_param.cur_pwm_0_duty_val >=
                PWM0_DEST_POWER_ON_DUTY_VAL &&
            pwm_handle_param.cur_pwm_1_duty_val >=
                PWM1_DEST_POWER_ON_DUTY_VAL)
        {
            // 当两路pwm都到对应的占空比值之后，才退出开机缓启动
            break;
        }

        if (flag_time_comes_during_power_on) // 如果调节时间到来
        {
            flag_time_comes_during_power_on = 0;
            pwm0_power_on_step += PWM0_POWER_ON_ADJUST_STEP; // 累计步长
            if (pwm0_power_on_step >= 1000)
            {
                pwm0_power_on_step -= 1000;

                if (pwm_handle_param.cur_pwm_0_duty_val < PWM0_DEST_POWER_ON_DUTY_VAL)
                {
                    pwm_handle_param.cur_pwm_0_duty_val++;
                }
            }

            pwm1_power_on_step += PWM1_POWER_ON_ADJUST_STEP;
            if (pwm1_power_on_step >= 1000)
            {
                pwm1_power_on_step -= 1000;

                if (pwm_handle_param.cur_pwm_1_duty_val < PWM1_DEST_POWER_ON_DUTY_VAL)
                {
                    pwm_handle_param.cur_pwm_1_duty_val++;
                }
            }
        }

        set_pwm_channel_0_duty(pwm_handle_param.cur_pwm_0_duty_val);
        set_pwm_channel_1_duty(pwm_handle_param.cur_pwm_1_duty_val);

        // 如果开机期间，需要转到呼吸模式
        // 开机缓启动期间，只处理 配对 和 取消配对 这两种情况
        // 如果从呼吸动画退出，到了缓启动期间， cur_pwm_x_duty_val 的值应该是确定值
        uart_handle();
    }

    // REVIEW 缓启动后，需要确认 pwm_handle_param 中的各项参数是否正确
    // 缓启动后，立即更新pwm占空比对应的值：（ 要给下面这些变量赋值，上电后会根据这些变量的值来调节 ）

    pwm_handle_param.expect_pwm_0_duty_val = pwm_handle_param.cur_pwm_0_duty_val;
    pwm_handle_param.expect_pwm_1_duty_val = pwm_handle_param.cur_pwm_1_duty_val;
    pwm_handle_param.cur_mode = PWM_MODE_NORMAL_WORK; // 设置为正常模式（表示退出了开机缓启动）
    // pwm_handle_param.dest_mode = PWM_MODE_NORMAL_WORK;
    pwm_handle_param.color_idx = PWM_COLOR_IDX_CYAN;
    pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_MAX;
}