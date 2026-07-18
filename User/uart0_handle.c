#include "uart0_handle.h"
#include "user_config.h"

#include "uart0.h"

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
	uart_receiver.timeout_enable = 0;
	uart_receiver.timeout_cnt = 0;
	uart_receiver.status = UART_RECV_STATUS_IDLE;
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

	// printf("uart_receiver.status == %u\n", (u16)uart_receiver.status);
	// printf("byte == 0x%02x\n", (u16)byte);

	switch (uart_receiver.status)
	{
	case UART_RECV_STATUS_IDLE:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_0 == byte)
		{
			ret = 0;
			uart_receiver.status++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_0:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_1 == byte)
		{
			ret = 0;
			uart_receiver.status++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_1:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_2 == byte)
		{
			ret = 0;
			uart_receiver.status++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_2:
		if (UART_DATA_HANDLE_FORMAT_FIX_VAL_3 == byte)
		{
			ret = 0;
			uart_receiver.status++;
		}
		break;

	case UART_RECV_STATUS_FORMAT_HEAD_3:
		// 接收完所有格式头，现在接收控制命令

		switch (byte)
		{
		case UART_CMD_DEV_ON:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 4; // 接下来还需要接收的数据长度
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_DEV_OFF:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 3; // 接下来还需要接收的数据长度
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_PAIR:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 4; // 接下来还需要接收的数据长度
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_CANCEL_PAIRING:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 4; // 接下来还需要接收的数据长度
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_SET_COLOR:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 6; // 接下来还需要接收的数据长度
			ret = 0;
			uart_receiver.status++;
			break;
		case UART_CMD_SET_BRIGHTNESS_LEV:
			uart_receiver.recv_len = 0;
			uart_receiver.expect_recv_len = 5; // 接下来还需要接收的数据长度
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
			uart_receiver.status++;
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
		printf("recv sta err\n");
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

void uart_handle(void)
{
	u8 recv_byte;
	u8 ret;
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
		break;
	case UART_CMD_DEV_OFF:
// 关闭设备
#if USER_DEBUG_ENABLE
		printf("recv cmd: dev off\n");
#endif
		break;
	case UART_CMD_PAIR:
// 开始配对
#if USER_DEBUG_ENABLE
		printf("recv cmd: pair\n");
#endif
		break;
	case UART_CMD_CANCEL_PAIRING:
		// 取消配对
#if USER_DEBUG_ENABLE
		printf("recv cmd: cancel pairing\n");
#endif
		break;
	case UART_CMD_SET_COLOR:
		// 设置颜色（颜色、色温）
#if USER_DEBUG_ENABLE
		printf("recv cmd: set color\n");
#endif
		break;
	case UART_CMD_SET_BRIGHTNESS_LEV:
		// 设置亮度等级
#if USER_DEBUG_ENABLE
		printf("recv cmd: set brightness lev\n");
#endif
		break;

	default:
		break;
	}

	// 重置接收器，准备下一次接收
	uart_receiver_reset();
}