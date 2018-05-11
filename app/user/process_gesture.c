/*
 * process_gesture.c
 *
 *  Created on: 2018年5月11日
 *      Author: guo
 */

#include "process_gesture.h"

#include "osapi.h"
#include "c_types.h"
#include "gpio.h"
#include "../include/driver/apds9960.h"



void ICACHE_FLASH_ATTR process_gesture() {
	apds.apds9960_initialize();

}
