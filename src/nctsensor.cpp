#include "ntcsensor.h"
#include "ads1115.h"
#include <math.h>

using namespace ModFirmWare;
#define ACCURACY 1e-4f

const double NTCSensor::getResistance()
//******************************************************************************
{
  return ads->getResistance(channel);
}

const double NTCSensor::getTemperatureK()
//******************************************************************************
{
  return convertResistanceToKelvin();
}

const double NTCSensor::getTemperatureC()
//******************************************************************************
{
  return convertResistanceToKelvin() - 273.15;
}

const double NTCSensor::getTemperatureF()
//******************************************************************************
{
  double k = convertResistanceToKelvin();
  return (k - 273.15) * 1.8 + 32.0;
}

void NTCSensor::calibrate(double alpha, double beta, double gamma)
//******************************************************************************
{
  this->alpha = alpha;
  this->beta = beta;
  this->gamma = gamma;
}

bool ModFirmWare::NTCSensor::measure()
//******************************************************************************
{
  float r = getResistance();
  if (abs(lastR - r) > ACCURACY)
  {
    lastR = r;
    return true;
  }
  else 
  {
    return false;
  }
}

const double NTCSensor::convertResistanceToKelvin()
//******************************************************************************
{
  readingValid = false;
  if (nullptr == ads)
  {
    return MAXFLOAT;
  }
  double r = getResistance();

  if (MAXFLOAT == r)
  {
    return MAXFLOAT;
  }

  double lnR = log(r); // Natural logarithm of resistance
  readingValid = true;
  return 1.0 / (alpha + beta * lnR + gamma * lnR * lnR * lnR);
}
