#ifndef __UART0_H__
#define __UART0_H__

#include "typedef.h"

#define UART0_BAUD 115200
#define USER_UART0_BAUD ((SYSCLK - UART0_BAUD) / (UART0_BAUD))

#define UART_RX_BUF_SIZE 128

// 增加环形缓冲区结构体
typedef struct
{
	u8 buffer[UART_RX_BUF_SIZE];
	u16 head;
	u16 tail;
	u16 count;
} uart_rx_buffer_t;

void uart0_config(void);

u16 uart0_rxbuffer_get_count(void);
u8 uart0_rxbuffer_get_byte(void);


#endif
