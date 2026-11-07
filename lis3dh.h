/*header file defining registers for LIS3DH and communication protocol stuff
 *
 * X axis data is 16 bits, signed
 *
 * */

//ifndef used to make sure this is the only header of this name
#ifndef lis3dh_h
#define lis3dh_h

#include "main.h"
#include <stdint.h>

//LIS3DH register definitions

#define LIS3DH_WHO_AM_I		0x0F	//r,dummy register in LIS3DH, identifies device
#define LIS3DH_CTRL_REG1	0x20 	//rw,control register, controls power, data collection, data rate selection
#define LIS3DH_CTRL_REG4	0x23	//rw,control register, handles SPI communication, measurement range
#define LIS3DH_OUT_X_L		0x28	//r, output along x axis, low byte, signed
#define LIS3DH_OUT_X_H		0x29	//r, output along x axis, high byte, signed
#define LIS3DH_OUT_Y_L		0x2A	//r, output along y axis, low byte, signed
#define LIS3DH_OUT_Y_H		0x2B	//r, output along y axis, high byte, signed
#define LIS3DH_OUT_Z_L		0x2C	//r, output along z axis, low byte, signed
#define LIS3DH_OUT_Z_H		0x2D	//r, output along z axis, high byte, signed

//SPI control information
#define LIS3DH_SPI_READ		0x80	//sets the first bit of the 2 8 bit cycles high to indicate READ mode, bit 7 is 1
#define LIS3DH_SPI_MS		0x40	//allows us to read from all 6 registers for x,y,z in 1 SPI communication, bit 6 is 1

//CS configuration
#define LIS3DH_CS_LOW()		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)	//sets CS pin low for communication
#define LIS3DH_CS_HIGH()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)		//sets CS pin high as default state

//function prototypes
uint8_t LIS3DH_ReadReg(uint8_t reg);	//reg is an 8 bit register address that is read from
void LIS3DH_WriteReg(uint8_t reg, uint8_t value);		//reg is where to write, value is what to write into reg
void LIS3DH_Init(void);		//initializes control registers for operation of the LIS3DH
void LIS3DH_ReadAccel(int16_t *x, int16_t *y, int16_t *z); //reads raw acceleration data as pointers since functions can't return multiple values apparently
uint8_t LIS3DH_Serial(void); //sets up serial monitor, comment out when moving into actual production


#endif //end of conditional header file








