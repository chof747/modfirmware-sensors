#ifndef MODFIRMWARE_NCT_SENSOR_H
#define MODFIRMWARE_NCT_SENSOR_H

#include "sensor_component.h"

namespace ModFirmWare
{
  class Ads1115Sensor;

  class NTCSensor : public SensorComponent
  {
  public:
    NTCSensor(Ads1115Sensor* ads, uint8_t channel, double alpha, double beta, double gamma)
        : SensorComponent(), ads(ads), channel(channel), alpha(alpha), beta(beta), gamma(gamma), readingValid(false), lastR(MAXFLOAT) {}

    const double getTemperatureC();
    const double getTemperatureK();
    const double getTemperatureF();
    const double getResistance();

    void calibrate(double alpha, double beta, double gamma);
    const bool isReadingValid() const { return readingValid; }

  protected:

    virtual bool measure();

  private:
    double alpha;
    double beta;
    double gamma;

    bool readingValid;

    Ads1115Sensor* ads;
    uint8_t channel;

    double lastR;
 
    const double convertResistanceToKelvin();
  };
};

#endif // MODFIRMWARE_NCT_SENSOR_H