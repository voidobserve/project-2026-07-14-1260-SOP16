/**
 ******************************************************************************
 * @file    main.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    02-09-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
 *
 * 版权说明后续补上
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "user_include.h"
#include "user_config.h"
#include "include.h"

#include <stdio.h>
#include "uart0.h"

#include "uart0_handle.h"
#include "power_on.h"
#include "pwm_handle.h"


void main(void)
{
    // 看门狗默认打开, 复位时间2s
    WDT_KEY = WDT_KEY_VAL(0xDD); // 关闭看门狗 (如需配置看门狗请查看“WDT\WDT_Reset”示例)

    system_init();

    // 关闭HCK和HDA的调试功能
    WDT_KEY = 0x55;  // 解除写保护
    IO_MAP &= ~0x01; // 清除这个寄存器的值，实现关闭HCK和HDA引脚的调试功能（解除映射）
    WDT_KEY = 0xBB;  // 写一个无效的数据，触发写保护

    uart0_config();
    uart_receiver_reset();

#if USER_DEBUG_ENABLE

    printf("sys reset\n");

    // 配置为输出模式，在逻辑分析仪上看IO翻转
    P2_MD1 &= ~(GPIO_P26_MODE_SEL(0x3));
    P2_MD1 |= GPIO_P26_MODE_SEL(0x01); // 输出模式
    FOUT_S26 = GPIO_FOUT_AF_FUNC;
#endif

    adc_pin_config(); // 配置使用到adc的引脚
    adc_config();

    tmr0_config(); //
    pwm_init();    // 配置pwm输出的引脚

    timer2_config();
    timer3_config(); // 要等adc完成初始化，再调用timer3的初始化

    // rf_recv_init(); // rf功能初始化
    fan_ctl_config();

    P1_MD1 &= ~GPIO_P14_MODE_SEL(0x03); // P14 16脚
    P1_MD1 |= GPIO_P14_MODE_SEL(0x01);
    FOUT_S14 = GPIO_FOUT_AF_FUNC; // AF功能输出
    P14 = 0;                      //

    // limited_max_pwm_duty = MAX_PWM_DUTY;
    limited_pwm_duty_due_to_fan_err = MAX_PWM_DUTY;
    limited_pwm_duty_due_to_temp = MAX_PWM_DUTY;
    limited_pwm_duty_due_to_unstable_engine = MAX_PWM_DUTY;

    power_on_handle(); 

    while (1)
    {
#if 1

        // TODO 测试时屏蔽，实际使用时要恢复
        temperature_scan(); // 检测热敏电阻一端的电压值
        fan_scan();         // 检测风扇的状态是否异常，并根据结果来限制pwm占空比
        set_duty();         // 设定到要调节到的脉宽 (设置adjust_duty)

        // 如果 expect_adjust_pwm_channel_x_duty 有变化，可以在这里修改 adjust_pwm_channel_x_duty
        // pwm_handle_param.dest_pwm_0_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_0_duty_val);
        // pwm_handle_param.dest_pwm_1_duty_val = get_pwm_channel_x_adjust_duty(pwm_handle_param.expect_pwm_1_duty_val);
        pwm_handle_update_dest_pwm_val();

        uart_handle(); // 处理串口接收到的数据

        // 风扇控制：
        // fan_ctl(); // 没有了线控调光，这里目前用不上，没有关灯的功能
#endif
    }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2022 HUGE-IC ***** END OF FILE *****/
