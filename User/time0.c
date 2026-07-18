#include "time0.h"
#include "power_on.h"
#include "adc.h"
#include "uart0_handle.h"

/**
 * @brief 配置定时器TMR0
 */
void tmr0_config(void)
{
    __EnableIRQ(TMR0_IRQn); // 使能timer0中断
    IE_EA = 1;              // 使能总中断

#define PEROID_VAL (SYSCLK / 128 / 1000 - 1) // 周期值=系统时钟/分频/频率 - 1
    // 设置timer0的计数功能，配置一个频率为1kHz的中断
    TMR_ALLCON = TMR0_CNT_CLR(0x1);                        // 清除计数值
    TMR0_PRH = TMR_PERIOD_VAL_H((PEROID_VAL >> 8) & 0xFF); // 周期值
    TMR0_PRL = TMR_PERIOD_VAL_L((PEROID_VAL >> 0) & 0xFF);
    TMR0_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                          // 计数等于周期时允许发生中断
    TMR0_CONL = TMR_SOURCE_SEL(0x7) | TMR_PRESCALE_SEL(0x7) | TMR_MODE_SEL(0x1); // 选择系统时钟，128分频，计数模式
}

// extern void fun(void);
// 定时器TMR0中断服务函数
void TIMR0_IRQHandler(void) interrupt TMR0_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR0_IRQn);

    // ---------------- 用户函数处理 -------------------

    // 周期中断
    if (TMR0_CONH & TMR_PRD_PND(0x1))
    {
        TMR0_CONH |= TMR_PRD_PND(0x1); // 清除pending

        // cnt_during_power_on++;
        // if (cnt_during_power_on >= 13) // 13ms
        // {
        //     cnt_during_power_on = 0;
        //     flag_time_comes_during_power_on = 1; // 开机缓启动期间，控制每次调节PWM占空比的时间
        // }

        flag_time_comes_during_power_on = 1; // 开机缓启动期间，控制每次调节PWM占空比的时间

        { // 风扇状态检测，累计一段时间后更新状态
            static u16 fan_normal_cnt = 0;
            static u16 fan_err_cnt = 0;

            if (flag_tim_scan_fan_is_err) // 如果检测到了风扇异常
            {
                fan_err_cnt++;
                fan_normal_cnt = 0;

                if (fan_err_cnt >= FAN_SCAN_TIMES_WHEN_ERR)
                {
                    fan_err_cnt = 0;
                    cur_fan_status = FAN_STATUS_ERROR;
                }
            }
            else
            {
                fan_normal_cnt++;
                fan_err_cnt = 0;

                if (fan_normal_cnt >= FAN_SCAN_TIMES_WHEN_NORMAL)
                {
                    fan_normal_cnt = 0;
                    cur_fan_status = FAN_STATUS_NORMAL;
                }
            }
        } // 风扇状态检测，累计一段时间后更新状态

        temperature_overheat_check_1ms_isr(); // 温度过热检测，1ms调用一次
        uart_receiver_timeout_add();          // 串口接收超时检测，1ms调用一次
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR0_IRQn);
}
