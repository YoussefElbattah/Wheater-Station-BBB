#ifndef _WEATHER_H
#define _WEATHER_H

#include "bme280.h"

int weather_read(char *path, sensor_attr *attr);

#endif
