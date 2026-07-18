#include "fan_ctl.h"
#include "pwm_handle.h"

void fan_ctl_config(void)
{
    // 初始化控制风扇开关的引脚：
    P1_MD0 &= ~(GPIO_P12_MODE_SEL(0x3));
    P1_MD0 |= (GPIO_P12_MODE_SEL(0x1)); // 输出模式
    FOUT_S12 = GPIO_FOUT_AF_FUNC;       // 选择AF功能输出

    P12 = 0; // 低电平，表示开启风扇
}

//
void fan_ctl(void)
{
    // 如果两路PWM都低于 10%， 控制脚 输出高电平
    if (pwm_handle_param.cur_pwm_0_duty_val < ((u16)MAX_PWM_DUTY * 10 / 100) &&
        pwm_handle_param.cur_pwm_1_duty_val < ((u16)MAX_PWM_DUTY * 10 / 100))
    {
        P12 = 1; // 高电平，表示关闭风扇
    }
    else
    {
        /* 如果有一路PWM大于10%占空比 */
        P12 = 0; // 低电平，表示开启风扇
    }
}
