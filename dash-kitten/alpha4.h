#include <Adafruit_MAX31855.h>
#include <Adafruit_LEDBackpack.h>


class Alpha4
{
  private:
    const uint8_t __pin_thermocouple_egt_cs = 6;
    Adafruit_AlphaNum4 alpha4;
    Adafruit_MAX31855 Thermocouple_EGT;
  public:
    Alpha4() :
      Thermocouple_EGT(__pin_thermocouple_egt_cs)
      {}
    void init(void);
    void housekeeping(void);
};

