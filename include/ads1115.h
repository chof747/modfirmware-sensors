#ifndef ADS1115_SENSOR_CLASS
#define ADS1115_SENSOR_CLASS

#include "sensor_component.h"
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

namespace ModFirmWare
{
  class Ads1115Sensor : public SensorComponent
  {
  public:

    static const uint8_t MAX_CHANNELS = 4;
    static const int8_t CHANNEL_NOT_USED = 0x81; // -127
    static const int8_t CHANNEL_ACTIVE_NO_DRIVER = 0xFF; // -1

    /**
     * @brief Channels Array is the type to contain the channels definition
     * 
     * The channels definition can contain the following values:
     * 
     * - 
     *
     */
    typedef int8_t channels_t[MAX_CHANNELS];
    enum class status_t {
      UNINITIALIZED = 0,
      READY = 1,
      MEASURING = 2,
      FAILURE = 3
    };
 
    Ads1115Sensor(channels_t channels, float referenceVoltage, float rFixed);
    Ads1115Sensor(channels_t channels, float referenceVoltage, float rFixed, uint8_t addr);

    bool setup(Application *app);
    bool measure() override;

    const float getVoltage(int8_t channelIx) const;
    const float getResistance(int8_t channelIx) const;

  private:

    Adafruit_ADS1115 ads;

    uint8_t i2cAddr; 
    uint8_t activeChannels;
    channels_t channels;
    status_t status;
    float referenceVoltage;
    float rFixed;

    int16_t rawValues[MAX_CHANNELS];
    float voltages[MAX_CHANNELS];
  };
};

#endif //