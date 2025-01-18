#include "ads1115.h"
#include <Arduino.h>


using namespace ModFirmWare;

#define LOGTAG "ads1115"

#define V_THRESHOLD 0.02
#define GPIO_SATURATION_DELAY_US 200

adsGain_t getOptimalGain(float referenceVoltage) {
  if (referenceVoltage <= 0.256) {
    return GAIN_SIXTEEN;  // ±0.256V
  } else if (referenceVoltage <= 0.512) {
    return GAIN_EIGHT;    // ±0.512V
  } else if (referenceVoltage <= 1.024) {
    return GAIN_FOUR;     // ±1.024V
  } else if (referenceVoltage <= 2.048) {
    return GAIN_TWO;      // ±2.048V
  } else if (referenceVoltage <= 4.096) {
    return GAIN_ONE;      // ±4.096V
  } else {
    return GAIN_TWOTHIRDS; // ±6.144V (default, largest range)
  }
}

Ads1115Sensor::Ads1115Sensor(Ads1115Sensor::channels_t channels, float referenceVoltage, float rFixed) 
: SensorComponent(), ads(), i2cAddr(ADS1X15_ADDRESS), status(status_t::UNINITIALIZED),
  referenceVoltage(referenceVoltage), rFixed(rFixed)
//******************************************************************************
{
  memcpy(this->channels, channels, sizeof(this->channels));
}

Ads1115Sensor::Ads1115Sensor(Ads1115Sensor::channels_t channels, float referenceVoltage, float rFixed, uint8_t addr) 
: SensorComponent(), ads(), i2cAddr(addr), status(status_t::UNINITIALIZED),
  referenceVoltage(referenceVoltage), rFixed(rFixed)
//******************************************************************************
{
  memcpy(this->channels, channels, sizeof(this->channels));
}
bool Ads1115Sensor::setup(Application *app)
//******************************************************************************
{
  if (!ads.begin())
  {
    logger->error(LOGTAG, "Failed to initialize ADC.");
    status = status_t::FAILURE;
    return false;
  }

  ads.setGain(getOptimalGain(referenceVoltage));

  status = status_t::READY;

  for (int i = 0; i < MAX_CHANNELS; ++i)
  {
    int8_t channel = channels[i];
    if ((CHANNEL_NOT_USED != channel) && (CHANNEL_ACTIVE_NO_DRIVER != channel))
    {
      pinMode(channel, OUTPUT);
      digitalWrite(channel, LOW);
    }
  }

  return true;
}

bool Ads1115Sensor::measure()
//******************************************************************************
{
  if (status_t::FAILURE == status)
  {
    logger->warn(LOGTAG, "The ADC (ADS1115) is in failure, therefore no measurement will be done!");
    return false;
  }

  for (int i = 0; i < MAX_CHANNELS; ++i)
  {
    if (CHANNEL_NOT_USED != channels[i])
    {
      // channel used
      int8_t channel = channels[i];
      bool driven = (CHANNEL_ACTIVE_NO_DRIVER != channel);

      if (driven)
      {
        digitalWrite(channel, HIGH);
        delayMicroseconds(GPIO_SATURATION_DELAY_US); //alternativels do a read to be discarded here
      }

      status = status_t::MEASURING;
      int16_t raw = ads.readADC_SingleEnded(i);
      status = status_t::READY;

      if (driven)
      {
        digitalWrite(channel, LOW);
      }

      voltages[i] = ads.computeVolts(raw);
      rawValues[i] = raw;

      logger->info(LOGTAG, "Reading channel %d from ADC: raw = %d, v = %.2f V", i, raw, voltages[i]);
    }
  }

  return true;
}

const float Ads1115Sensor::getVoltage(int8_t channelIx) const
//******************************************************************************
{
  if (!((channelIx < MAX_CHANNELS) && (channelIx >= 0)))
  {
    return 0;
  }

  return voltages[channelIx];
}

const float Ads1115Sensor::getResistance(int8_t channelIx) const
//******************************************************************************
{
  if (!((channelIx < MAX_CHANNELS) && (channelIx >= 0)))
  {
    return 0;
  }

  float v = voltages[channelIx];
  if ((referenceVoltage - v) < V_THRESHOLD)
  {
    return MAXFLOAT;
  }
  else
  {
    return rFixed * (v/(referenceVoltage - v));
  }
}