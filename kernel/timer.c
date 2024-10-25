#include "include/timer.h"
#include "include/util.h"

// get the divisor - the amount the time needs to count down from - from a number in hz
static inline unsigned short get_divisor(unsigned int freq) {
  return (unsigned short) (PIT_FREQ_HZ / freq);
}

void init_timer(unsigned short divisor) {
  // com[7:6] - channel
  // com[5:4] - r/w mode
    // 01 - LSB
    // 10 - MSB
    // 11 - both
  // com[3:1] - mode
  // com[0] - binary/bcd
  pbyte_out(0x43, 0b00110110);

  idle();
  pbyte_out(0x40, divisor & 0xff);
  idle();
  pbyte_out(0x40, (divisor >> 8) & 0xff);
}