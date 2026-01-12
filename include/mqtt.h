#ifndef __MQTT_H
#define __MQTT_H

#include <mosquitto.h>
#include "bme280.h"

#define HOST		"youssef-bbb.duckdns.org"
#define PORT		40000
#define KEEP_ALIVE	60
#define USER_NAME	"youssef_user"
#define PASSWORD	"YoussefEl123"


int mqtt_connect(struct mosquitto *mosq);
int mqtt_init(struct mosquitto **mosq);
int mqtt_publish(struct mosquitto *mosq, sensor_attr attr);

#endif 
