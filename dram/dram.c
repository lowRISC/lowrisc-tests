// A dram test program

#include <stdio.h>
#include "uart.h"
#include "memory.h"

unsigned long long lfsr64(unsigned long long d) {
  // x^64 + x^63 + x^61 + x^60 + 1
  unsigned long long bit = 
    (d >> (64-64)) ^
    (d >> (64-63)) ^
    (d >> (64-61)) ^
    (d >> (64-60)) ^
    1;
  return (d >> 1) | (bit << 63);
}

int main() {
  unsigned long waddr = 0;
  unsigned long raddr = 0;
  unsigned long long wkey = 0;
  unsigned long long rkey = 0;
  unsigned int i = 0;

  uart_init();
  printf("DRAM test program.\n");

  while(1) {
    printf("Write block @%lx using key %llx\n", waddr, wkey);
    for(i=0; i<1024*16; i++) {
      *(get_ddr_base() + waddr) = wkey;
      waddr = (waddr + 1) & 0x3ffffff;
      wkey = lfsr64(wkey);
    }

    if(waddr == (raddr + 16*1024*16) & 0x3ffffff) { /* read after write 1M, force write back in L2 */
      printf("Check block @%lx using key %llx\n", raddr, rkey);
      for(i=0; i<1024*16; i++) {
        unsigned long long rd = *(get_ddr_base() + raddr);
        raddr = (raddr + 1) & 0x3ffffff;
        if(rkey != rd) {
          printf("Error! key %llx stored @%lx does not match with %llx\n", rd, raddr, rkey);
          return 1;
        }
        rkey = lfsr64(rkey);
      }
    }
  }
}

