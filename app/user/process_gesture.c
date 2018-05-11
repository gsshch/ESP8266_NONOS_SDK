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
#include "os_type.h"
static os_timer_t check_timer;


void ICACHE_FLASH_ATTR process_gesture() {
	int ges = apds.readGesture();
	os_printf("ges = %d\n", ges);
}

void ICACHE_FLASH_ATTR apds_task_init() {
	apds.apds9960_initialize();

	os_timer_disarm(&check_timer);
	os_timer_setfn(&check_timer, (os_timer_func_t *)process_gesture, NULL); //a demo to process the data in uart rx buffer
	os_timer_arm(&check_timer, 100, 1);
}
