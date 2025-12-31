#include <stdio.h>
#include "lcd.h"
#include <string.h>
#include <unistd.h>
#include "bme280.h"

#define RETRY_TRY	5

int main(int argc, char **argv){
	const char path[] = PATH TEMP_VALUE;
	int ret;
	ret = lcd_init();
	if(ret < 0){
		printf("Couldn't initialize LCD\n");
		return ret;
	}

	sleep(2);

	ret = lcd_clear();
	if(ret < 0)
		return ret;
	
	sensor_attr temp_attr;
	char msg[SOME_BYTES];
	while(1){
		
		lcd_set_cursor(1,1);
		int ret = read_sysfs_long(path, &temp_attr.current_val);
		if(ret){
			printf("Couldn't read temperature value from sysfs , error %d", ret);
			return ret;
		}

		printf("current_val = %4.1f \n", temp_attr.current_val);
		ret = max_min_check(&temp_attr);
		if(ret){
			printf("Couldn't check max/min values \n");
			return ret;
		}
		printf("max/min = %4.1f/%4.1f \n", temp_attr.max_value, temp_attr.min_value);
		ret = status_update(&temp_attr);
		if(ret)
			return ret;

		snprintf(msg, sizeof(msg), "T:%4.1fC ST:%s  ", temp_attr.current_val,temp_attr.stat);

		printf("msg = %s\n", msg);
		ret = lcd_send_message(msg);
		if(ret < 0)
			return ret;

		lcd_set_cursor(2,1);
		
		snprintf(msg, sizeof(msg), "LO/HI: %4.1f/%4.1f", temp_attr.min_value, temp_attr.max_value);
		      
		printf("msg = %s\n", msg);
		ret = lcd_send_message(msg);
		if(ret < 0)
			return ret;

		sleep(1);
	}


}

