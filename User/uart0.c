#include "uart0.h"
#include "include.h"

#include "user_config.h"

volatile uart_rx_buffer_t uart0_rx_buffer = {0};

#if USER_DEBUG_ENABLE

// 重写puchar()函数
char putchar(char c)
{
    while (!(UART0_STA & UART_TX_DONE(0x01)))
        ;
    UART0_DATA = c;
    return c;
}
#endif

void uart0_config(void)
{
#if USER_DEBUG_ENABLE
    // 发送脚 ， 只在调试时使用
    P0_MD0 &= ~(GPIO_P00_MODE_SEL(0x3));
    P0_MD0 |= GPIO_P00_MODE_SEL(0x1); // 配置为输出模式
    FOUT_S00 |= GPIO_FOUT_UART0_TX;   // 配置为 UART0_TX
#endif

#if (!USER_DEBUG_PIN_ENABLE)
    // 接收脚
    // 可能是模块的信号拉低能力优先，这里不能开上拉，否则容易解错码
    // P0_PU |= GPIO_P03_PULL_UP(0x01);      // 上拉
    P0_MD0 &= ~(GPIO_P03_MODE_SEL(0x03)); // 输入模式
    FIN_S7 = GPIO_FIN_SEL_P03;            // 选择 uart0 rx 对应的引脚
#else
    // 接收脚
    // 可能是模块的信号拉低能力优先，这里不能开上拉，否则容易解错码
    // P2_PU |= GPIO_P25_PULL_UP(0x01);      // 上拉
    P2_MD1 &= ~(GPIO_P25_MODE_SEL(0x03)); // 输入模式
    FIN_S7 = GPIO_FIN_SEL_P25;            // 选择 uart0 rx 对应的引脚
#endif

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位

    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_RX_IRQ_EN(0x01) |
                 UART_EN(0x1); // 8bit数据，1bit停止位

    // __SetIRQnIP(UART0_IRQn, 1); // 设置中断优先级，数值越大，优先级越高
    __EnableIRQ(UART0_IRQn);    // 打开模块中断
    IE_EA = 1;                  // 打开总中断
}

// 获取接收缓冲区中有效的数据个数，单位：字节Byte
u16 uart0_rxbuffer_get_count(void)
{
    return uart0_rx_buffer.count;
}

// 从接收缓冲区中取出一个字节数据
u8 uart0_rxbuffer_get_byte(void)
{
    u8 rxbyte;

    if (0 == uart0_rx_buffer.count)
    {
        // 缓冲区空
        return 0;
    }

    // 先偏移索引，再取出数据
    // uart0_rx_buffer.tail = (uart0_rx_buffer.tail + 1) % UART_RX_BUF_SIZE;
    // rxbyte = uart0_rx_buffer.buffer[uart0_rx_buffer.tail];

    uart0_rx_buffer.tail++;
    if (uart0_rx_buffer.tail >= UART_RX_BUF_SIZE)
    {
        uart0_rx_buffer.tail = 0;
    }
    rxbyte = uart0_rx_buffer.buffer[uart0_rx_buffer.tail];

    uart0_rx_buffer.count--;

    return rxbyte;
}

/**
 * @brief 向UART0接收缓冲区中存入一个字节数据
 * @note 当缓冲区满时会覆盖旧的数据
 * @param byte 要存入的字节数据
 */
static void uart0_rxbuffer_put_byte(u8 byte)
{
    // 目前的逻辑：缓冲区满，覆盖旧的数据

    // 先偏移索引，再存放数据
    // uart0_rx_buffer.head = (uart0_rx_buffer.head + 1) % UART_RX_BUF_SIZE;
    // uart0_rx_buffer.buffer[uart0_rx_buffer.head] = byte;

    uart0_rx_buffer.head++;
    if (uart0_rx_buffer.head >= UART_RX_BUF_SIZE)
    {
        uart0_rx_buffer.head = 0;
    }
    uart0_rx_buffer.buffer[uart0_rx_buffer.head] = byte;

    uart0_rx_buffer.count++;
    if (uart0_rx_buffer.count > UART_RX_BUF_SIZE)
    {
        uart0_rx_buffer.count = UART_RX_BUF_SIZE;
    }
}

// UART0中断服务函数（接收中断）
void UART0_IRQHandler(void) interrupt UART0_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(UART0_IRQn);
    // ---------------- 用户函数处理 -------------------
    // RX接收完成中断
    if (UART0_STA & UART_RX_DONE(0x1))
    {
        u8 recv_byte = UART0_DATA;
        uart0_rxbuffer_put_byte(recv_byte);

        // printf("%02x\n", (u16)recv_byte);
    }
    // 退出中断设置IP，不可删除
    __IRQnIPnPop(UART0_IRQn);
}
