
#include <stdio.h>
#include "lcd.h"
#include "bme280.h"
#include "display.h"

int display_init(void){
	
	int ret = lcd_init();
	if(ret < 0)
		return ret;

	sleep(2);

	ret = lcd_clear();
	if(ret < 0)
		return ret;

	return 0;
}
int display_result(sensor_attr attr){
	
	char msg[64];

	lcd_set_cursor(1,1);

	snprintf(msg, sizeof(msg), "T:%4.1fC ST:%s  ", attr.current_val, attr.stat);

	printf("msg = %s\n", msg);
	
	int ret = lcd_send_message(msg);
	if(ret < 0)
		return ret;

	lcd_set_cursor(2,1);
		
	snprintf(msg, sizeof(msg), "LO/HI: %4.1f/%4.1f", attr.min_value, attr.max_value);
		      
	printf("msg = %s\n", msg);

	ret = lcd_send_message(msg);
	if(ret < 0)
		return ret;

	return 0;

}
