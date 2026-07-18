#ifndef __TIMER1_H__
#define __TIMER1_H__

// #include "user_include.h"
// #include "include.h"



void tmr1_config(void);  // 配置定时器，定时器默认关闭
void tmr1_enable(void);  // 开启定时器，开始计时
void tmr1_disable(void); // 关闭定时器，清空计数值

#endif
