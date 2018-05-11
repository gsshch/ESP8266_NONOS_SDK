/*
 * user_sensor_apds9960.c
 *
 *  Created on: 2018年5月11日
 *      Author: guo
 */

#include "../include/driver/apds9960.h"

#include "../../app/include/driver/i2c_master.h"
#include "c_types.h"
#include "mem.h"
#include "osapi.h"

/*Configures I2C communications and initializes registers to defaults*/

enum ERROR_INFO{
	APDS9960_ID_ERROR,
};
bool ICACHE_FLASH_ATTR apds9960_initialize() {
	os_printf("apds9960 init\n");
	unsigned char id = 0;
	/* Initialize I2C */
	i2c.i2c_master_gpio_init();
	i2c.i2c_master_init();

	/* Read ID register and check against known values for APDS-9960 */
	id = wireReadDataByte(APDS9960_ID);

	if (!(id == APDS9960_ID_1 || id == APDS9960_ID_2)) {
		return 0;
	}
	/* Set ENABLE register to 0 (disable all features) */
	setMode(ALL, OFF);
	/* Set default values for ambient light and proximity registers */
	wireWriteDataByte(APDS9960_ATIME, DEFAULT_ATIME);
	wireWriteDataByte(APDS9960_WTIME, DEFAULT_WTIME);
	wireWriteDataByte(APDS9960_PPULSE, DEFAULT_PROX_PPULSE);
	wireWriteDataByte(APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR);
	wireWriteDataByte(APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL);
	wireWriteDataByte(APDS9960_CONFIG1, DEFAULT_CONFIG1);
	setLEDDrive(DEFAULT_LDRIVE);
	setProximityGain(DEFAULT_PGAIN);
	setAmbientLightGain(DEFAULT_AGAIN);
	if (!setProxIntLowThresh(DEFAULT_PILT)) {

		return false;
	}
	if (!setProxIntHighThresh(DEFAULT_PIHT)) {

		return false;
	}

	if (!setLightIntLowThreshold(DEFAULT_AILT)) {
		return false;
	}
	if (!setLightIntHighThreshold(DEFAULT_AIHT)) {

		return false;
	}
	if (!wireWriteDataByte(APDS9960_PERS, DEFAULT_PERS)) {

		return false;
	}
	if (!wireWriteDataByte(APDS9960_CONFIG2, DEFAULT_CONFIG2)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_CONFIG3, DEFAULT_CONFIG3)) {
		return false;
	}
	/* Set default values for gesture sense registers */
	if (!setGestureEnterThresh(DEFAULT_GPENTH)) {

		return false;
	}
	if (!setGestureExitThresh(DEFAULT_GEXTH)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GCONF1, DEFAULT_GCONF1)) {
		return false;
	}
	if (!setGestureGain(DEFAULT_GGAIN)) {
		return false;
	}
	if (!setGestureLEDDrive(DEFAULT_GLDRIVE)) {
		return false;
	}
	if (!setGestureWaitTime(DEFAULT_GWTIME)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GOFFSET_U, DEFAULT_GOFFSET)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GOFFSET_D, DEFAULT_GOFFSET)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GOFFSET_L, DEFAULT_GOFFSET)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GOFFSET_R, DEFAULT_GOFFSET)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_GCONF3, DEFAULT_GCONF3)) {
		return false;
	}
	if (!setGestureIntEnable(DEFAULT_GIEN)) {
		return false;
	}

	return true;
}

/*Enables or disables a feature in the APDS-9960*/
bool ICACHE_FLASH_ATTR setMode(uint8_t mode, uint8_t enable) {
	uint8_t reg_val;

	/* Read current ENABLE register */
	reg_val = getMode();
	if (reg_val == ERROR) {
		return false;
	}

	/* Change bit(s) in ENABLE register */
	enable = enable & 0x01;
	if (mode >= 0 && mode <= 6) {
		if (enable) {
			reg_val |= (1 << mode);
		} else {
			reg_val &= ~(1 << mode);
		}
	} else if (mode == ALL) {
		if (enable) {
			reg_val = 0x7F;
		} else {
			reg_val = 0x00;
		}
	}

	/* Write value back to ENABLE register */
	if (!wireWriteDataByte(APDS9960_ENABLE, reg_val)) {
		return false;
	}

	return true;
}

/*Reads and returns the contents of the ENABLE register*/
uint8_t ICACHE_FLASH_ATTR getMode() {
	uint8_t enable_value;

	/* Read current ENABLE register */
	enable_value = wireReadDataByte(APDS9960_ENABLE);

	return enable_value;
}

/*Sets the LED drive strength for proximity and ambient light sensor (ALS)*/
bool ICACHE_FLASH_ATTR setLEDDrive(uint8_t drive) {
	uint8_t val;

	/* Read value from CONTROL register */
	val = wireReadDataByte(APDS9960_CONTROL);

	/* Set bits in register to given value */
	drive &= 0b00000011;
	drive = drive << 6;
	val &= 0b00111111;
	val |= drive;

	/* Write register value back into CONTROL register */
	if (!wireWriteDataByte(APDS9960_CONTROL, val)) {
		return false;
	}

	return true;
}

/*Sets the receiver gain for proximity detection*/
bool ICACHE_FLASH_ATTR setProximityGain(uint8_t drive) {
	uint8_t val;

	/* Read value from CONTROL register */
	val = wireReadDataByte(APDS9960_CONTROL);
	/* Set bits in register to given value */
	drive &= 0b00000011;
	drive = drive << 2;
	val &= 0b11110011;
	val |= drive;

	/* Write register value back into CONTROL register */
	if (!wireWriteDataByte(APDS9960_CONTROL, val)) {
		return false;
	}

	return true;
}

/*Sets the receiver gain for the ambient light sensor (ALS)*/
bool ICACHE_FLASH_ATTR setAmbientLightGain(uint8_t drive) {
	uint8_t val;

	/* Read value from CONTROL register */
	val = wireReadDataByte(APDS9960_CONTROL);

	/* Set bits in register to given value */
	drive &= 0b00000011;
	val &= 0b11111100;
	val |= drive;

	/* Write register value back into CONTROL register */
	if (!wireWriteDataByte(APDS9960_CONTROL, val)) {
		return false;
	}

	return true;
}

/*Sets the lower threshold for proximity detection*/
bool ICACHE_FLASH_ATTR setProxIntLowThresh(uint8_t threshold) {
	if (!wireWriteDataByte(APDS9960_PILT, threshold)) {
		return false;
	}

	return true;
}

/*Sets the high threshold for proximity detection*/
bool ICACHE_FLASH_ATTR setProxIntHighThresh(uint8_t threshold) {
	if (!wireWriteDataByte(APDS9960_PIHT, threshold)) {
		return false;
	}

	return true;
}

/*Sets the low threshold for ambient light interrupts*/
bool ICACHE_FLASH_ATTR setLightIntLowThreshold(uint16_t threshold) {
	uint8_t val_low;
	uint8_t val_high;

	/* Break 16-bit threshold into 2 8-bit values */
	val_low = threshold & 0x00FF;
	val_high = (threshold & 0xFF00) >> 8;

	/* Write low byte */
	if (!wireWriteDataByte(APDS9960_AILTL, val_low)) {
		return false;
	}

	/* Write high byte */
	if (!wireWriteDataByte(APDS9960_AILTH, val_high)) {
		return false;
	}

	return true;
}

/*Sets the high threshold for ambient light interrupts*/
bool ICACHE_FLASH_ATTR setLightIntHighThreshold(uint16_t threshold) {
	uint8_t val_low;
	uint8_t val_high;

	/* Break 16-bit threshold into 2 8-bit values */
	val_low = threshold & 0x00FF;
	val_high = (threshold & 0xFF00) >> 8;

	/* Write low byte */
	if (!wireWriteDataByte(APDS9960_AIHTL, val_low)) {
		return false;
	}

	/* Write high byte */
	if (!wireWriteDataByte(APDS9960_AIHTH, val_high)) {
		return false;
	}

	return true;
}

/*Sets the entry proximity threshold for gesture sensing*/
bool ICACHE_FLASH_ATTR setGestureEnterThresh(uint8_t threshold) {
	if (!wireWriteDataByte(APDS9960_GPENTH, threshold)) {
		return false;
	}

	return true;
}

/*Sets the exit proximity threshold for gesture sensing*/
bool ICACHE_FLASH_ATTR setGestureExitThresh(uint8_t threshold) {
	if (!wireWriteDataByte(APDS9960_GEXTH, threshold)) {
		return false;
	}

	return true;
}

/*Sets the gain of the photodiode during gesture mode*/
bool ICACHE_FLASH_ATTR setGestureGain(uint8_t gain) {
	uint8_t val;
	/* Read value from GCONF2 register */
	val = wireReadDataByte(APDS9960_GCONF2);

	/* Set bits in register to given value */
	gain &= 0b00000011;
	gain = gain << 5;
	val &= 0b10011111;
	val |= gain;

	/* Write register value back into GCONF2 register */
	if (!wireWriteDataByte(APDS9960_GCONF2, val)) {
		return false;
	}

	return true;
}

/*Sets the LED drive current during gesture mode*/
bool ICACHE_FLASH_ATTR setGestureLEDDrive(uint8_t drive) {
	uint8_t val;

	/* Read value from GCONF2 register */
	val = wireReadDataByte(APDS9960_GCONF2);

	/* Set bits in register to given value */
	drive &= 0b00000011;
	drive = drive << 3;
	val &= 0b11100111;
	val |= drive;

	/* Write register value back into GCONF2 register */
	if (!wireWriteDataByte(APDS9960_GCONF2, val)) {
		return false;
	}

	return true;
}

/*Sets the time in low power mode between gesture detections*/
bool ICACHE_FLASH_ATTR setGestureWaitTime(uint8_t time) {
	uint8_t val;

	/* Read value from GCONF2 register */
	val = wireReadDataByte(APDS9960_GCONF2);

	/* Set bits in register to given value */
	time &= 0b00000111;
	val &= 0b11111000;
	val |= time;

	/* Write register value back into GCONF2 register */
	if (!wireWriteDataByte(APDS9960_GCONF2, val)) {
		return false;
	}

	return true;
}

/* Turns gesture-related interrupts on or off*/
bool ICACHE_FLASH_ATTR setGestureIntEnable(uint8_t enable) {
	uint8_t val;

	/* Read value from GCONF4 register */
	val = wireReadDataByte(APDS9960_GCONF4);

	/* Set bits in register to given value */
	enable &= 0b00000001;
	enable = enable << 1;
	val &= 0b11111101;
	val |= enable;

	/* Write register value back into GCONF4 register */
	if (!wireWriteDataByte(APDS9960_GCONF4, val)) {
		return false;
	}

	return true;
}

/*Starts the gesture recognition engine on the APDS-9960*/
bool ICACHE_FLASH_ATTR enableGestureSensor(bool interrupts) {

	/* Enable gesture mode
	 Set ENABLE to 0 (power off)
	 Set WTIME to 0xFF
	 Set AUX to LED_BOOST_300
	 Enable PON, WEN, PEN, GEN in ENABLE
	 */
	resetGestureParameters();
	if (!wireWriteDataByte(APDS9960_WTIME, 0xFF)) {
		return false;
	}
	if (!wireWriteDataByte(APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE)) {
		return false;
	}
	if (!setLEDBoost(LED_BOOST_300)) {
		return false;
	}
	if (interrupts) {
		if (!setGestureIntEnable(1)) {
			return false;
		}
	} else {
		if (!setGestureIntEnable(0)) {
			return false;
		}
	}
	if (!setGestureMode(1)) {
		return false;
	}
	if (!enablePower()) {
		return false;
	}
	if (!setMode(WAIT, 1)) {
		return false;
	}
	if (!setMode(PROXIMITY, 1)) {
		return false;
	}
	if (!setMode(GESTURE, 1)) {
		return false;
	}

	return true;
}

/*Resets all the parameters in the gesture data member*/
void ICACHE_FLASH_ATTR resetGestureParameters() {

	gesture_data_.index = 0;
	gesture_data_.total_gestures = 0;

	gesture_ud_delta_ = 0;
	gesture_lr_delta_ = 0;

	gesture_ud_count_ = 0;
	gesture_lr_count_ = 0;

	gesture_near_count_ = 0;
	gesture_far_count_ = 0;

	gesture_state_ = 0;
	gesture_motion_ = DIR_NONE;
}

/*Sets the LED current boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 */
bool ICACHE_FLASH_ATTR setLEDBoost(uint8_t boost) {
	uint8_t val;

	/* Read value from CONFIG2 register */
	val = wireReadDataByte(APDS9960_CONFIG2);

	/* Set bits in register to given value */
	boost &= 0b00000011;
	boost = boost << 4;
	val &= 0b11001111;
	val |= boost;

	/* Write register value back into CONFIG2 register */
	if (!wireWriteDataByte(APDS9960_CONFIG2, val)) {
		return false;
	}

	return true;
}

/*Tells the state machine to either enter or exit gesture state machine*/
bool ICACHE_FLASH_ATTR setGestureMode(uint8_t mode) {
	uint8_t val;

	/* Read value from GCONF4 register */
	val = wireReadDataByte(APDS9960_GCONF4);

	/* Set bits in register to given value */
	mode &= 0b00000001;
	val &= 0b11111110;
	val |= mode;

	/* Write register value back into GCONF4 register */
	if (!wireWriteDataByte(APDS9960_GCONF4, val)) {
		return false;
	}

	return true;
}

/*Turn the APDS-9960 on*/
bool ICACHE_FLASH_ATTR enablePower() {
	if (!setMode(POWER, 1)) {
		return false;
	}

	return true;
}

/*Determines if there is a gesture available for reading*/
bool ICACHE_FLASH_ATTR isGestureAvailable() {
	uint8_t val;

	/* Read value from GSTATUS register */
	val = wireReadDataByte(APDS9960_GSTATUS);

	/* Shift and mask out GVALID bit */
	val &= APDS9960_GVALID;

	/* Return true/false based on GVALID bit */
	if (val == 1) {
		return true;
	} else {
		return false;
	}
}

/*Processes a gesture event and returns best guessed gesture*/
int ICACHE_FLASH_ATTR readGesture() {
	uint8_t fifo_level = 0;
	int bytes_read = 0;
	uint8_t fifo_data[64];
	uint8_t fifo_data2[64];
	uint8_t gstatus;
	int motion;
	int i;

	/* Make sure that power and gesture is on and data is valid */
	if (!isGestureAvailable() || !(getMode() & 0b01000001)) {
		return DIR_NONE;
	}

	/* Keep looping as long as gesture data is valid */
	while (1) {

		/* Wait some time to collect next batch of FIFO data */
//		__delay_ms(FIFO_PAUSE_TIME);
		os_delay_us(30000);
		/* Get the contents of the STATUS register. Is data still valid? */
		gstatus = wireReadDataByte(APDS9960_GSTATUS);

		/* If we have valid data, read in FIFO */
		if ((gstatus & APDS9960_GVALID) == APDS9960_GVALID) {
			/* Read the current FIFO level */
			fifo_level = wireReadDataByte(APDS9960_GFLVL);

			/* If there's stuff in the FIFO, read it into our data block */
			if (fifo_level > 0) {

				if (fifo_level < 17) {
					bytes_read = wireReadDataBlock( APDS9960_GFIFO_U,
							(uint8_t*) fifo_data, (fifo_level * 4));
				} else {
					bytes_read = wireReadDataBlock( APDS9960_GFIFO_U,
							(uint8_t*) fifo_data2, (fifo_level * 4));
				}

				if (bytes_read == -1) {
					return ERROR;
				}

				/* If at least 1 set of data, sort the data into U/D/L/R */
				if (bytes_read >= 4) {

					for (i = 0; i < bytes_read; i += 4) {
						gesture_data_.u_data[gesture_data_.index] = fifo_data[i
								+ 0];
						gesture_data_.d_data[gesture_data_.index] = fifo_data[i
								+ 1];
						gesture_data_.l_data[gesture_data_.index] = fifo_data[i
								+ 2];
						gesture_data_.r_data[gesture_data_.index] = fifo_data[i
								+ 3];
						gesture_data_.index++;
						gesture_data_.total_gestures++;
					}

					/* Filter and process gesture data. Decode near/far state */
					if (processGestureData()) {

						if (decodeGesture()) {
							//***TODO: U-Turn Gestures
						}
					}

					/* Reset data */
					gesture_data_.index = 0;
					gesture_data_.total_gestures = 0;
				}
			}
		} else {

			/* Determine best guessed gesture and clean up */
//			__delay_ms(FIFO_PAUSE_TIME);
			os_delay_us(30000);
			decodeGesture();
			motion = gesture_motion_;

			resetGestureParameters();

			return motion;
		}
	}
}

/*Processes the raw gesture data to determine swipe direction*/
bool ICACHE_FLASH_ATTR processGestureData() {
	uint8_t u_first = 0;
	uint8_t d_first = 0;
	uint8_t l_first = 0;
	uint8_t r_first = 0;
	uint8_t u_last = 0;
	uint8_t d_last = 0;
	uint8_t l_last = 0;
	uint8_t r_last = 0;
	int ud_ratio_first;
	int lr_ratio_first;
	int ud_ratio_last;
	int lr_ratio_last;
	int ud_delta;
	int lr_delta;
	int i;

	/* If we have less than 4 total gestures, that's not enough */
	if (gesture_data_.total_gestures <= 4) {
		return false;
	}

	/* Check to make sure our data isn't out of bounds */
	if ((gesture_data_.total_gestures <= 32)
			&& (gesture_data_.total_gestures > 0)) {

		/* Find the first value in U/D/L/R above the threshold */
		for (i = 0; i < gesture_data_.total_gestures; i++) {
			if ((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT)) {

				u_first = gesture_data_.u_data[i];
				d_first = gesture_data_.d_data[i];
				l_first = gesture_data_.l_data[i];
				r_first = gesture_data_.r_data[i];
				break;
			}
		}

		/* If one of the _first values is 0, then there is no good data */
		if ((u_first == 0) || (d_first == 0) || (l_first == 0)
				|| (r_first == 0)) {

			return false;
		}
		/* Find the last value in U/D/L/R above the threshold */
		for (i = gesture_data_.total_gestures - 1; i >= 0; i--) {

			if ((gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT)
					&& (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT)) {

				u_last = gesture_data_.u_data[i];
				d_last = gesture_data_.d_data[i];
				l_last = gesture_data_.l_data[i];
				r_last = gesture_data_.r_data[i];
				break;
			}
		}
	}

	/* Calculate the first vs. last ratio of up/down and left/right */
	ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
	lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
	ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
	lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);

	/* Determine the difference between the first and last ratios */
	ud_delta = ud_ratio_last - ud_ratio_first;
	lr_delta = lr_ratio_last - lr_ratio_first;

	/* Accumulate the UD and LR delta values */
	gesture_ud_delta_ += ud_delta;
	gesture_lr_delta_ += lr_delta;

	/* Determine U/D gesture */
	if (gesture_ud_delta_ >= GESTURE_SENSITIVITY_1) {
		gesture_ud_count_ = 1;
	} else if (gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1) {
		gesture_ud_count_ = -1;
	} else {
		gesture_ud_count_ = 0;
	}

	/* Determine L/R gesture */
	if (gesture_lr_delta_ >= GESTURE_SENSITIVITY_1) {
		gesture_lr_count_ = 1;
	} else if (gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1) {
		gesture_lr_count_ = -1;
	} else {
		gesture_lr_count_ = 0;
	}

	/* Determine Near/Far gesture */
	if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 0)) {
		if ((abs(ud_delta) < GESTURE_SENSITIVITY_2)
				&& (abs(lr_delta) < GESTURE_SENSITIVITY_2)) {

			if ((ud_delta == 0) && (lr_delta == 0)) {
				gesture_near_count_++;
			} else if ((ud_delta != 0) || (lr_delta != 0)) {
				gesture_far_count_++;
			}

			if ((gesture_near_count_ >= 10) && (gesture_far_count_ >= 2)) {
				if ((ud_delta == 0) && (lr_delta == 0)) {
					gesture_state_ = NEAR_STATE;
				} else if ((ud_delta != 0) && (lr_delta != 0)) {
					gesture_state_ = FAR_STATE;
				}
				return true;
			}
		}
	} else {
		if ((abs(ud_delta) < GESTURE_SENSITIVITY_2)
				&& (abs(lr_delta) < GESTURE_SENSITIVITY_2)) {

			if ((ud_delta == 0) && (lr_delta == 0)) {
				gesture_near_count_++;
			}

			if (gesture_near_count_ >= 10) {
				gesture_ud_count_ = 0;
				gesture_lr_count_ = 0;
				gesture_ud_delta_ = 0;
				gesture_lr_delta_ = 0;
			}
		}
	}

	return false;
}

/*Determines swipe direction or near/far state*/
bool ICACHE_FLASH_ATTR decodeGesture() {
	/* Return if near or far event is detected */
	if (gesture_state_ == NEAR_STATE) {
		gesture_motion_ = DIR_NEAR;
		return true;
	} else if (gesture_state_ == FAR_STATE) {
		gesture_motion_ = DIR_FAR;
		return true;
	}

	/* Determine swipe direction */
	if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 0)) {
		gesture_motion_ = DIR_UP;
	} else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 0)) {
		gesture_motion_ = DIR_DOWN;
	} else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == 1)) {
		gesture_motion_ = DIR_RIGHT;
	} else if ((gesture_ud_count_ == 0) && (gesture_lr_count_ == -1)) {
		gesture_motion_ = DIR_LEFT;
	} else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == 1)) {
		if ( abs(gesture_ud_delta_) > abs(gesture_lr_delta_)) {
			gesture_motion_ = DIR_UP;
		} else {
			gesture_motion_ = DIR_RIGHT;
		}
	} else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == -1)) {
		if ( abs(gesture_ud_delta_) > abs(gesture_lr_delta_)) {
			gesture_motion_ = DIR_DOWN;
		} else {
			gesture_motion_ = DIR_LEFT;
		}
	} else if ((gesture_ud_count_ == -1) && (gesture_lr_count_ == -1)) {
		if ( abs(gesture_ud_delta_) > abs(gesture_lr_delta_)) {
			gesture_motion_ = DIR_UP;
		} else {
			gesture_motion_ = DIR_LEFT;
		}
	} else if ((gesture_ud_count_ == 1) && (gesture_lr_count_ == 1)) {
		if ( abs(gesture_ud_delta_) > abs(gesture_lr_delta_)) {
			gesture_motion_ = DIR_DOWN;
		} else {
			gesture_motion_ = DIR_RIGHT;
		}
	} else {
		return false;
	}

	return true;
}

/*Reads a block (array) of bytes from the I2C device and register*/
int ICACHE_FLASH_ATTR wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len) {
	unsigned char j = 0;

	i2c.i2c_master_start();
	i2c.i2c_master_writeByte((APDS9960_I2C_ADDR << 1) | 0x00); // Slave address + Write command

	i2c.i2c_master_writeByte(reg); // write the location

	for (j = 0; j < len; j++) {

//		I2C_ReStart();
		i2c.i2c_master_writeByte((APDS9960_I2C_ADDR << 1) | 0x01); //Slave address+ read command
		val[j] = i2c.i2c_master_readByte(); // Store the Receive value in a variable
		i2c.i2c_master_send_nack();
	}

	i2c.i2c_master_stop();

	return (int) j;
}

/*Writes a single byte to the I2C device and specified register*/
int ICACHE_FLASH_ATTR wireWriteDataByte(unsigned char reg, unsigned char val) {

	i2c.i2c_master_start();
	i2c.i2c_master_writeByte((APDS9960_I2C_ADDR << 1) | 0x00); // Slave address + Write command
	i2c.i2c_master_writeByte(reg); // write the location
	i2c.i2c_master_writeByte(val); // Write the Data
	i2c.i2c_master_stop();

	return 1;
}

/*Reads a single byte from the I2C device and specified register*/
uint8_t ICACHE_FLASH_ATTR wireReadDataByte(unsigned char reg) {
//	os_printf("wire read data byte=%d\n",reg);
	/* Indicate which register we want to read from */
	unsigned char val;

	i2c.i2c_master_start();
	i2c.i2c_master_writeByte((APDS9960_I2C_ADDR << 1) | 0x00); // Slave address + Write command
	i2c.i2c_master_writeByte(reg); // write location
//	I2C_ReStart();
	i2c.i2c_master_writeByte((APDS9960_I2C_ADDR << 1) | 0x01); //Slave address+ read command
	val = i2c.i2c_master_readByte();   // Store the Receive value in a variable
	i2c.i2c_master_send_nack();
	i2c.i2c_master_stop();

	return (val);

}

api_apds9960 apds = {
		apds9960_initialize,
		processGestureData,
		readGesture,
		isGestureAvailable
};

