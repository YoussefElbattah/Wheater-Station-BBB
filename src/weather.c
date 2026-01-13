#include "weather.h"
#include <stdio.h>

int weather_read(char *path, sensor_attr *attr){

	int ret = read_sysfs_long(path, &attr->current_val);
	if(ret){
		printf("Couldn't read temperature value from sysfs , error %d\n", ret);
		return ret;
	}

	ret = max_min_check(attr);
	if(ret){
		printf("Couldn't check max/min values \n");
		return ret;
	}
	ret = status_update(attr);
	if(ret){
		printf("Couldn't update status value\n");
		return ret;
	}

	return 0;
}
