#pragma once
#include "arch/x86/asm.h"
#include <types.h>

#define CMOS_ADDRESS_PORT  0x70
#define CMOS_DATA_PORT     0x71
#define NMI_DISABLE_FLAG   0x80
#define RTC_REG_SECONDS    0x00
#define RTC_REG_MIMUTES    0x02
#define RTC_REG_HOURS      0x04
#define RTC_REG_WEEKDAY    0x06
#define RTC_REG_DAYOFMONTH 0x07
#define RTC_REG_MONTH      0x08
#define RTC_REG_YEAR       0x09

struct DATE_TIME {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day_of_month;
	uint8_t day_of_week;
	uint8_t month;
	uint8_t year;
};

class RTC
{
  public:
	static void get_time(DATE_TIME* Time);

  private:
	static ::uint8_t BCDtoBYTE(uint8_t BCD);
};
