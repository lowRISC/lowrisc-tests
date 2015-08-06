
#include "uart.h"

volatile uint32_t * get_uart_base() {
  return (uint32_t *)(UART_BASE);
}

void uart_init() {
  
  // get a base pointer
  volatile uint32_t *uart_base_ptr = get_uart_base();

  // set 0x0080 to UART.LCR to enable DLL and DLM write
  // configure baud rate
  *(uart_base_ptr + UART_LCR) = 0x0080;

  // System clock 50 MHz, 115200 baud rate
  // divisor = clk_freq / (16 * Baud)
  *(uart_base_ptr + UART_DLL) = 50*1000*1000u / (16u * 115200u) % 0x100u;
  *(uart_base_ptr + UART_DLM) = 50*1000*1000u / (16u * 115200u) >> 8;

  // 8-bit data, 1-bit odd parity
  *(uart_base_ptr + UART_LCR) = 0x000Bu;

  // Disable all interrupt, use polling
  *(uart_base_ptr + UART_IER) = 0x0000u;

}

void uart_send(uint8_t data) {
  // get a base pointer
  volatile uint32_t *uart_base_ptr = get_uart_base();

  // wait until THR empty
  uint32_t status;
  while(! (*(uart_base_ptr + UART_LSR) & 0x40u)) { }

  *(uart_base_ptr + UART_THR) = data;
}

uint8_t uart_recv() {
  // get a base pointer
  volatile uint32_t *uart_base_ptr = get_uart_base();

  // wait until RBR has data
  uint32_t status;
  while(! (*(uart_base_ptr + UART_LSR) & 0x01u)) { }

  return *(uart_base_ptr + UART_RBR);

}
