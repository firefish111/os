#include "util.h"
#include "port.h"
#include "memring.h"

#ifndef CMOS_H
#define CMOS_H

#define RTC_SEC  0x00
#define RTC_MIN  0x02
#define RTC_HR   0x04
#define RTC_WKDY 0x06
#define RTC_DATE 0x07
#define RTC_MON  0x08
#define RTC_YR   0x09
#define RTC_CENT 0x32

#define RTC_STAT_A 0x0a
#define RTC_STAT_B 0x0b

#define P_CMOS_ADDRESS 0x70
#define P_CMOS_DATA    0x71

#define MK_BCD(num) num = ((num) & 0x0f) + (((num) >> 4) * 10)

static inline unsigned char get_cmos_reg(unsigned char reg) {
  pbyte_out(P_CMOS_ADDRESS, reg | 0x80);
  return pbyte_in(P_CMOS_DATA);
}

enum weekdays {
  Sunday = 1,
  Monday = 2,
  Tuesday = 3,
  Wednesday = 4,
  Thursday = 5,
  Friday = 6,
  Saturday = 7
};

char *weekmap[7] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat"
};

struct timestamp {
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  enum weekdays weekday;
  unsigned char date;
  unsigned char month;
  unsigned int year;
} __attribute__((packed));

void read_rtc(struct timestamp *ts) {
  // update in progress flag
//  while (get_cmos_reg(RTC_STAT_A) | 0x80);

  ts -> second  = get_cmos_reg(RTC_SEC );
  ts -> minute  = get_cmos_reg(RTC_MIN );
  ts -> hour    = get_cmos_reg(RTC_HR );
  unsigned char pm_flag = bitclear(&(ts -> hour), 7); // only relevant if 12hour: takes the most signifcant bit
  ts -> weekday = (enum weekdays) get_cmos_reg(RTC_WKDY);
  ts -> date    = get_cmos_reg(RTC_DATE);
  ts -> month   = get_cmos_reg(RTC_MON );
  ts -> year    = (unsigned int) get_cmos_reg(RTC_YR);
  unsigned char century = get_cmos_reg(RTC_CENT);
  
  // bcd mode - if bit 2 of B is clear we are in bcd mode.
  if (!(get_cmos_reg(RTC_STAT_B) & 0x04)) {
    MK_BCD(ts -> second);
    MK_BCD(ts -> minute);
    MK_BCD(ts -> hour);
    MK_BCD(ts -> date);
    MK_BCD(ts -> month);
    MK_BCD(ts -> year);
    MK_BCD(century);
  }

  // 12/24h - if bit 1 set we are in 24h mode.
  if (!(get_cmos_reg(RTC_STAT_B) & 0x02) && pm_flag) {
    ts -> hour = (ts -> hour + 12) % 24;
  }

  ts -> year += century * 100;
}

void time(void *tbuf) {
  // get two readings - check if they are equal. a very naive method.
  struct timestamp *temp = malloc(sizeof(struct timestamp));
  struct timestamp *next = malloc(sizeof(struct timestamp));

  read_rtc(temp);
  read_rtc(next);

  while (!memcmp(temp, next, sizeof(struct timestamp))) {
    free(temp, sizeof(struct timestamp));
    temp = next;
    next = malloc(sizeof(struct timestamp));
    read_rtc(next);
  }

  free(temp, sizeof(struct timestamp));
  memcpy(next, tbuf, sizeof(struct timestamp));
  free(next, sizeof(struct timestamp));
}

#endif