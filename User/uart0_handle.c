#include "uart0_handle.h"
#include "user_config.h"

#include "uart0.h"
#include "pwm_handle.h"

// 接收器
static volatile uart_receiver_t uart_receiver;

/**
 * @brief 重置接收器
 * @note 在上电后、接收超时、接收出错、接收完成后调用
 *
 */
void uart_receiver_reset(void)
{
	uart_receiver.index = 0;
	uart_receiver.recv_len = 0;
	uart_receiver.expect_recv_len = 0;

	uart_receiver.status = UART_RECV_STATUS_IDLE;

	uart_receiver.timeout_enable = 0;
	uart_receiver.timeout_cnt = 0;
}

/**
 * @brief 接收器超时使能时，累计超时时间，在定时器中断中调用
 *
 */
void uart_receiver_timeout_add(void)
{
	if (uart_receiver.timeout_enable)
	{
		if (uart_receiver.timeout_cnt < ((u16)-1))
		{
			uart_receiver.timeout_cnt++;
		}
	}
}

// 接收超时处理函数
void uart_receiver_timeout_handler(void)
{
#if USER_DEBUG_ENABLE
	u8 i;
#endif

	if (uart_receiver.timeout_cnt >= UART_DATA_HANDLE_TIMEOUT)
	{
#if USER_DEBUG_ENABLE
		printf("UART receive timeout, current status: %u\n", (u16)uart_receiver.status);

		// 打印当前缓冲区内容
		printf("Buffer content: \n");
		for (i = 0; i < uart_receiver.index; i++)
		{
			printf("0x%02x ", (u16)uart_receiver.buffer[i]);
		}
		printf("\n");
#endif

		// 重置接收器
		uart_receiver_reset();
	}
}

/**
 * @brief 接收器处理当前接收到的1字节数据
 *
 */
u8 uart_receiver_process_byte(u8 byte)
{
	u8 ret = 1; // 0: 处理成功，1: 处理失败
	volatile u8 i;
	volatile u8 check_val = 0; // 校验值

	switch (uart_receiver.status)
	{
	case UART_RECV_STATUS_IDLE:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_0 == byte)
		{
			ret = 0;
			uart_receiver.status++;
			uart_receiver.recv_len++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_0:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_1 == byte)
		{
			ret = 0;
			uart_receiver.status++;
			uart_receiver.recv_len++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_1:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_2 == byte)
		{
			ret = 0;
			uart_receiver.status++;
			uart_receiver.recv_len++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_2:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_3 == byte)
		{
			ret = 0;
			uart_receiver.status++;
			uart_receiver.recv_len++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_3:
		// 接收完所有格式头，现在接收控制命令

		switch (byte)
		{
		case UART_CMD_DEV_ON:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 4 + 5;
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_DEV_OFF:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 3 + 5;
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_PAIR:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 4 + 5;
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_CANCEL_PAIRING:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 4 + 5;
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_SET_COLOR:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 6 + 5;
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_SET_BRIGHTNESS_LEV:
			uart_receiver.recv_len++;
			uart_receiver.expect_recv_len = 5 + 5;
			ret = 0;
			uart_receiver.status++;
			break;

		default:
			break;
		}

		break;

	case UART_RECV_STATUS_CMD:
		// 接收完控制命令，现在接收数据
		ret = 0; // 接收数据时不关心是否成功
		uart_receiver.recv_len++;

		if (uart_receiver.recv_len >=
			uart_receiver.expect_recv_len)
		{
			// 接收完所有数据后，判断最后一个字节的校验值是否正确
			for (i = 0; i < uart_receiver.recv_len - 1; i++)
			{
				check_val ^= uart_receiver.buffer[i];
			}

#if USER_DEBUG_ENABLE
			// printf("check val == %02x \n", (u16)check_val);
			// printf("recv check val == %02x\n", (u16)byte);
#endif

			if (check_val != byte)
			{
#if USER_DEBUG_ENABLE
				// printf("check val err\n");
#endif
				ret = 1;
			}
			else
			{
				uart_receiver.status++;
			}
		}

#if USER_DEBUG_ENABLE
		// printf("uart_receiver.recv_len == %u\n",
		// 	   (u16)uart_receiver.recv_len);
		// printf("uart_receiver.expect_recv_len == %u\n",
		// 	   (u16)uart_receiver.expect_recv_len);
#endif
		break;

	default:
#if USER_DEBUG_ENABLE
		// printf("recv sta err\n");
#endif
		break;
	}

	if (0 == ret)
	{
		uart_receiver.buffer[uart_receiver.index] = byte;
		uart_receiver.index++;
	}

	return ret;
}

static void uart_cmd_dev_on_handle(void)
{
	if (pwm_handle_param.cur_mode == PWM_MODE_PWR_ON_ANIM ||
		pwm_handle_param.cur_mode == PWM_MODE_NORMAL_WORK ||
		pwm_handle_param.cur_mode == PWM_MODE_BREATH_ANIM ||
		pwm_handle_param.cur_mode == PWM_MODE_ON)
	{
		/*
			如果要执行开灯动画，但是目前模式处于以下情况，不处理：
			1. 当前处于第一次上电的开机缓启动模式
			2. 当前处于正常工作模式
			3. 当前处于呼吸灯动画模式
			4. 当前已经是开灯模式
		*/
		return;
	}

// REVIEW 开灯完成后，需要注意有没有回到 正常工作 模式
#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
	pwm_handle_param.cur_mode = PWM_MODE_ON;
	pwm_handle_param.cur_mode_sta = PWM_ON_OR_OFF_STA_INIT;
#else

	// TODO 立即设置PWM占空比
	pwm_handle_param.cur_mode = PWM_MODE_ON;
#endif
}

static void uart_cmd_dev_off_handle(void)
{
	if (pwm_handle_param.cur_mode == PWM_MODE_PWR_ON_ANIM ||
		pwm_handle_param.cur_mode == PWM_MODE_BREATH_ANIM ||
		pwm_handle_param.cur_mode == PWM_MODE_OFF)
	{
		/*
			如果要执行关灯动画，但是目前模式处于以下情况，不处理：
			1. 当前处于第一次上电的开机缓启动模式
			2. 当前处于呼吸灯动画模式
			3. 当前已经是关灯模式
		*/
		return;
	}

	#if (0 == PWM_ON_OR_OFF_IMMEDIATELY_ENABLE)
	pwm_handle_param.cur_mode = PWM_MODE_OFF;
	pwm_handle_param.cur_mode_sta = PWM_ON_OR_OFF_STA_INIT;
	#else

	// TODO 立即设置PWM占空比
	pwm_handle_param.cur_mode = PWM_MODE_OFF;

	
	
	#endif
}

static void uart_cmd_pair_handle(void)
{
#if USER_DEBUG_ENABLE
	printf("func : uart_cmd_pair_handle\n");
#endif

	pwm_handle_param.last_mode = pwm_handle_param.cur_mode;
	pwm_handle_param.cur_mode = PWM_MODE_BREATH_ANIM;
	pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_INIT;
}

static void uart_cmd_cancel_pairing_handle(void)
{
#if USER_DEBUG_ENABLE
	printf("func : uart_cmd_cancel_pairing_handle\n");
#endif

	pwm_handle_param.last_mode = pwm_handle_param.cur_mode;
	pwm_handle_param.cur_mode = PWM_MODE_BREATH_ANIM;
	pwm_handle_param.cur_mode_sta = PWM_BREATH_ANIM_STA_INIT;
}

static void uart_cmd_set_color_handle(void)
{
#if USER_DEBUG_ENABLE
	u8 i;
	printf("func : uart_cmd_set_color_handle\n");
#endif

	if (pwm_handle_param.cur_mode == PWM_MODE_PWR_ON_ANIM ||
		uart_receiver.recv_len < 11)
	{
		/*
			1. 当前处于开机动画模式，不处理
			2. 接收到的数据长度不足，不处理
		*/
#if USER_DEBUG_ENABLE
		// printf("func : uart_cmd_set_color_handle\n");
		printf("uart_receiver.recv_len == %u\n", (u16)uart_receiver.recv_len);
		printf("err\n");
#endif
		return;
	}

	if (uart_receiver.buffer[8] == 0xD3 &&
		uart_receiver.buffer[9] == 0x42)
	{
#if USER_DEBUG_ENABLE
		printf("set color temp idx 1\n"); // 设置色温1
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_TEMPERATURE_1;
	}
	else if (uart_receiver.buffer[8] == 0x72 &&
			 uart_receiver.buffer[9] == 0xA5)
	{
#if USER_DEBUG_ENABLE
		printf("set color temp idx 2\n");
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_TEMPERATURE_2;
	}
	else if (uart_receiver.buffer[8] == 0x3F &&
			 uart_receiver.buffer[9] == 0xD1)
	{
#if USER_DEBUG_ENABLE
		printf("set color temp idx 3\n");
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_TEMPERATURE_3;
	}
	else if (uart_receiver.buffer[8] == 0x00 &&
			 uart_receiver.buffer[9] == 0xFF)
	{
#if USER_DEBUG_ENABLE
		printf("set color to blue\n");
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_BLUE;
	}
	else if (uart_receiver.buffer[8] == 0x3F &&
			 uart_receiver.buffer[9] == 0xA5)
	{
#if USER_DEBUG_ENABLE
		printf("set color to cyan\n");
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_CYAN;
	}
	else if (uart_receiver.buffer[8] == 0xFF &&
			 uart_receiver.buffer[9] == 0x00)
	{
#if USER_DEBUG_ENABLE
		printf("set color to green\n");
#endif

		pwm_handle_param.color_idx = PWM_COLOR_IDX_GREEN;
	}
	else
	{
#if USER_DEBUG_ENABLE

		printf("set color mode err\n");
		for (i = 0; i < uart_receiver.recv_len; i++)
		{
			printf("0x%02x ", (u16)uart_receiver.buffer[i]);
		}

#endif
	}

	pwm_handle_refresh_expect_pwm_duty_val();
}

void uart_cmd_set_brightness_lev_handle(void)
{
#if USER_DEBUG_ENABLE
	printf("func : uart_cmd_set_brightness_lev_handle\n");
#endif

	if (pwm_handle_param.cur_mode == PWM_MODE_PWR_ON_ANIM ||
		uart_receiver.recv_len < 10)
	{
		/*
			1. 当前处于开机动画模式，不处理
			2. 接收到的数据长度不足，不处理
		*/
#if USER_DEBUG_ENABLE
		printf("uart_receiver.recv_len == %u\n", (u16)uart_receiver.recv_len);
		printf("err\n");
#endif
		return;
	}

	if (uart_receiver.buffer[7] == 0x05)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 1\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_1;
	}
	else if (uart_receiver.buffer[7] == 0x0A)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 2\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_2;
	}
	else if (uart_receiver.buffer[7] == 0x19)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 3\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_3;
	}
	else if (uart_receiver.buffer[7] == 0x32)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 4\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_4;
	}
	else if (uart_receiver.buffer[7] == 0x4B)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 5\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_5;
	}
	else if (uart_receiver.buffer[7] == 0x64)
	{
#if USER_DEBUG_ENABLE
		printf("set brightness lev 6\n"); //
#endif

		pwm_handle_param.brightness_lev = PWM_BRIGHTNESS_LEV_MAX;
	}

	pwm_handle_refresh_expect_pwm_duty_val();
}

void uart_handle(void)
{
	u8 recv_byte;
	u8 ret; // 存放串口处理数据函数的返回值，0: 处理成功，1: 处理失败
#if USER_DEBUG_ENABLE
	u8 i;
#endif

	// 检查是否接收超时
	if (uart_receiver.timeout_enable)
	{
		uart_receiver_timeout_handler();
	}

	// 处理接收缓冲区中的数据
	while (1)
	{
		if (0 == uart0_rxbuffer_get_count() ||
			uart_receiver.status == UART_RECV_STATUS_DATA)
		{
			// 缓冲区为空或者接收完成，退出循环

#if USER_DEBUG_ENABLE
#if 0
			if (0 == uart0_rxbuffer_get_count() &&
				uart_receiver.status != UART_RECV_STATUS_DATA)
			{
				printf("0 == uart0_rxbuffer_get_count()\n");
			}

			if (uart_receiver.status == UART_RECV_STATUS_DATA)
			{
				printf("recv complete \n");
			}
#endif
#endif
			break;
		}

		recv_byte = uart0_rxbuffer_get_byte();

		// 启用超时计数
		uart_receiver.timeout_enable = 1;
		uart_receiver.timeout_cnt = 0;

		// 处理字节
		ret = uart_receiver_process_byte(recv_byte);
		if (ret)
		{
#if USER_DEBUG_ENABLE
			// 处理失败
			// 如果连格式头都没有接收，不打印错误信息
			if (uart_receiver.status != 0)
			{
				printf("Byte processing failed\n");
				printf("cur uart receiver status: %02d\n", (u16)uart_receiver.status);
			}
#endif
			uart_receiver_reset();
		}
	}

	if (uart_receiver.status != UART_RECV_STATUS_DATA)
	{
		// 接收未完成，函数直接返回
		return;
	}

	// ===========================================================
	// ===========================================================
	// 接收完成，处理数据

#if USER_DEBUG_ENABLE
#if 0
	// 打印接收到的一帧数据
	printf("================================>\n");
	printf("Received complete frame: \n");
	for (i = 0; i < uart_receiver.index; i++)
	{
		printf("0x%02x ", (u16)uart_receiver.buffer[i]);
	}
	printf("\n");
	printf("================================^\n");
#endif
#endif

	// 根据接收到的指令，执行对应的操作
	switch (uart_receiver.buffer[4])
	{
	case UART_CMD_DEV_ON:
// 开启设备
#if USER_DEBUG_ENABLE
		printf("recv cmd: dev on\n");
#endif

		uart_cmd_dev_on_handle();

		break;
		// ============================================================
		// ============================================================
	case UART_CMD_DEV_OFF:
// 关闭设备
#if USER_DEBUG_ENABLE
		printf("recv cmd: dev off\n");
#endif

		uart_cmd_dev_off_handle();

		break;
		// ============================================================
		// ============================================================
	case UART_CMD_PAIR:
// 开始配对
#if USER_DEBUG_ENABLE
		printf("recv cmd: pair\n");
#endif
		uart_cmd_pair_handle();
		break;
		// ============================================================
		// ============================================================
	case UART_CMD_CANCEL_PAIRING:
		// 取消配对
#if USER_DEBUG_ENABLE
		printf("recv cmd: cancel pairing\n");
#endif

		uart_cmd_cancel_pairing_handle();
		break;
		// ============================================================
		// ============================================================
	case UART_CMD_SET_COLOR:
		// 设置颜色（颜色、色温）
#if USER_DEBUG_ENABLE
		printf("recv cmd: set color\n");
#endif
		uart_cmd_set_color_handle();
		break;
		// ============================================================
		// ============================================================
	case UART_CMD_SET_BRIGHTNESS_LEV:
		// 设置亮度等级
#if USER_DEBUG_ENABLE
		printf("recv cmd: set brightness lev\n");
#endif
		uart_cmd_set_brightness_lev_handle();
		break;

	default:
		break;
	}

	// 重置接收器，准备下一次接收
	uart_receiver_reset();
}