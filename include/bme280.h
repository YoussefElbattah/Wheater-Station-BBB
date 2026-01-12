#ifndef __BME280_H
#define __BME280_H

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define TEMP_VALUE 			"in_temp_input"
#define TEMP_CALIBRATION 		"in_temp_oversampling_ratio"
#define HUMIDITY_VALUE              	"in_humidityrelative_input"
#define HUMIDITY_CALIBRATION        	"in_humidityrelative_oversampling_ratio"
#define PRESSURE_VALUE             	"in_pressure_input"
#define PRESSURE_CALIBRATION        	"in_pressure_oversampling_ratio"

#define PATH		"/sys/bus/iio/devices/iio:device0/"

#define SOME_BYTES 	32

#define LOW_CODE	0
#define MED_CODE	1
#define HOT_CODE	2

#define UNDEFINED_CODE -99
#define STATUS_TO_CODE(status) (                                   	\
		    (strcmp((status), "LOW") == 0) ? LOW_CODE :         \
		    (strcmp((status), "MED") == 0) ? MED_CODE :         \
		    (strcmp((status), "HOT") == 0) ? HOT_CODE : UNDEFINED_CODE       \
		    )

/**
 * Status definition:
 * - Low    : T < 15°C
 * - Medium : 15°C ≤ T ≤ 28°C
 * - High   : T > 28°C
 */

typedef struct{
	float current_val;
	float max_value, min_value;
	char stat[5];
	bool initialized;
}sensor_attr;

int read_sysfs_long(const char *path, float *val);
int bme280_init(int calib_temp, int calib_hum, int calib_press);
int write_sysfs_bme(const char *path, int calib);
int max_min_check(sensor_attr *attr);
int status_update(sensor_attr *attr);
#endif
