/*
 * lis3dh.c
 *
 *  Created on: Nov 6, 2026
 *      Author: adria
 */

#include "lis3dh.h"
#include <stdio.h>


//serial monitor section for testing and debugging, comment out when moving into production
uint8_t LIS3DH_Serial(void){
    uint8_t id = LIS3DH_ReadReg(LIS3DH_WHO_AM_I);

    printf("WHO_AM_I = 0x%02X (expected 0x33)\r\n", id);

    if (id != 0x33) {
        printf("LIS3DH not detected!\r\n");
        return 0;
    }

    LIS3DH_WriteReg(LIS3DH_CTRL_REG1, 0x57);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x08);
    printf("LIS3DH initialized\r\n");
    return 1;
}

extern SPI_HandleTypeDef hspi1; //externalized variable, exists in project

//read single byte from register
uint8_t LIS3DH_ReadReg(uint8_t reg)

{

	uint8_t tx_data[2]; //transmit data, 2 bytes
	uint8_t rx_data[2]; //receive data, 2 bytes

	tx_data[0] = reg | LIS3DH_SPI_READ;  //or reg with SPI_READ, data will be sent out from this array, or with LIS3DH_SPI_READ to ensure bit 7 is high to enable read from accelerometer
	tx_data[1] = 0x00; //since we are in full duplex mode we must transmit and receive data simultaneously, transmit this dummy byte when we want to recieve data

	LIS3DH_CS_LOW(); //cs low to enable data transfer
	HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, HAL_MAX_DELAY); //use spi2, tx and rx consist of 2 bytes, do this until complete, max delay is HAL_MAX_DELAY, if delay exceed HAL_TIMEOUT error is returned
	LIS3DH_CS_HIGH(); //deselect chip

	return rx_data[1];

}


//writing to sensor to configure it
void LIS3DH_WriteReg(uint8_t reg, uint8_t value)
{

	uint8_t tx_data[2]; //transmit data, 2 bytes
	tx_data[0] = reg & ~LIS3DH_SPI_READ; //bitwise & and invert, functionally flips bit 7 to 0, enabling write while leaving data from reg intact
	tx_data[1] = value; //transmit data

	LIS3DH_CS_LOW(); //select chip
	HAL_SPI_Transmit(&hspi1,tx_data, 2, HAL_MAX_DELAY); //transmit data
	LIS3DH_CS_HIGH(); //deselect chip


}

void LIS3DH_Init(void)
{

	//read device ID, for debugging purposes, remove if necessary for production
	uint8_t id = LIS3DH_ReadReg(LIS3DH_WHO_AM_I);


	/*
	 * write to control register 1:   0101  0111
	 *
	 * 	0111 (MSB to LSB): (0: normal power mode, 1: Z axis enable, 1: Y axis enable, 1: X axis enable)
	 * 	0101: tells sensor to produce data at 100 HZ, note that 0000 would initiate power down mode
	 *
	 * 	MCU will read at 100ms, meaning 9 out of 10 readings by sensor are ignored, this can be changed
	 * 	by manually change HAL_Delay in main function
	 */
	LIS3DH_WriteReg(LIS3DH_CTRL_REG1, 0x57);

	/*
	 *
	 * write to control register 4: 0000 1100, 0000 1010 in two seperate functions
	 *
	 * 1100 high resolution mode, self test 1
	 * 1010 high resolution mode, self test 0
	 */
	LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x0C);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x0A);

}

void LIS3DH_ReadAccel(int16_t *x, int16_t *y, int16_t *z)
{

	uint8_t tx_data[7]; //1 address byte, 6 data bytes
	uint8_t rx_data[7]; //1 address byte, 6 data bytes

	//set starting address as LIS3DH_OUT_X_L (0x28), allow read, allow auto increment, send dummy bytes to allow data to be read from device
	tx_data[0] = LIS3DH_OUT_X_L | LIS3DH_SPI_READ | LIS3DH_SPI_MS;
	for (int i = 1; i < 7; i++){
		tx_data[i] = 0x00;
	}

	LIS3DH_CS_LOW(); //enable chip
	HAL_SPI_TransmitReceive(&hspi1,tx_data,rx_data,7, HAL_MAX_DELAY); //read accelerometer data
	LIS3DH_CS_HIGH(); // disable chip

	*x = (int16_t)(( rx_data[2] << 8) | (rx_data[1])); //combine low and high byte along x
	*y = (int16_t)(( rx_data[4] << 8) | (rx_data[3])); //combine low and high byte along y
	*z = (int16_t)(( rx_data[6] << 8) | (rx_data[5])); //combine low and high byte along z

}

