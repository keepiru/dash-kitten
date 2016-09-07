#include <RTC_DS3231.h>

typedef uint8_t U8;
typedef uint16_t U16;
void txRtcFrame( DateTime * );
void printDateTime( DateTime * );
void printRtcTemp();
DateTime dateTimeFromCan( U8 *, int );
void displayTime();

extern RTC_DS3231 RTC;
#define MS3_RTC_REQ_ADDR 28869304
#define MS3_RTC_WRITE_ADDR 644

class TKRTC
{
  public:
    static void init(void);
};
