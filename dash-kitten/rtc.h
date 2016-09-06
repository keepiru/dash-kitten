
typedef uint8_t U8;
typedef uint16_t U16;
void txRtcFrame( DateTime * );
void printDateTime( DateTime * );
void printRtcTemp();
DateTime dateTimeFromCan( U8 *, int );
void displayTime();

//#define USE_RTC
