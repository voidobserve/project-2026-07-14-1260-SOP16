#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define USER_DEBUG_ENABLE 0
/*
	是否使用开发板上的测试引脚

	串口数据接收，原本用 P03 , 在demo板上使用 P25

	pwm_channel_1 输出引脚，用demo板的 P05 代替 P15
*/
#define USER_DEBUG_PIN_ENABLE 0 // TEST_ONLY ，实际给到客户的版本，应该设置为0
#if ((0 == USER_DEBUG_ENABLE) && USER_DEBUG_PIN_ENABLE)
#error "debug is not enable"
#endif

#if USER_DEBUG_ENABLE
#include <stdio.h>
#endif

#endif