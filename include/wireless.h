#ifndef _WIRELESS_H
#define _WIRELESS_H

#include <mosquitto.h>
#include "bme280.h"
#include "mqtt.h"

int wireless_init(struct mosquitto **mosq, struct mqtt_config_t *cfg_ctx);
void wireless_send(struct mosquitto *mosq, char *topic, sensor_attr attr);

#endif 
