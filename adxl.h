#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************************************************************************************************************/
/*																				 Registers and Variables 																	  */
/******************************************************************************************************************************************************************************/

#define DATA_FORMAT 				0x31
#define MEASUREMENTS_DATA 			0x32
#define PWR_CNTRL 					0x2D
#define THRESHOLD_ACTIVITY 			0x24
#define INTERRUPT_ENABLE 			0x2E
#define INTERRUPT_MAP 				0x2F
#define THRESHOLD_INACTIVITY 		0x25
#define ACT_INACT_CNT 				0x27
#define X_AXIS_OFFSET 				0x1E
#define Y_AXIS_OFFSET 				0x1F
#define Z_AXIS_OFFSET 				0x20
#define TIME_INACTIVITY 			0x26
#define DEVID_0 					0x00
#define DEVID_1 					0x01
#define PARTID 						0x02
#define REVID 						0x03
#define XID 						0x04
#define SOFT_RESET 					0x18
#define INT_SOURCE 					0x30
#define BW_RATE 					0x2c

#define BIT0 						0x00 	// 0000 0000
#define BIT1 						0x01	// 0000 0001
#define BIT2 						0x02	// 0000 0010
#define BIT3 						0x03	// 0000 0011
#define BIT4 						0x04	// 0000 0100
#define BIT5 						0x05	// 0000 0101
#define BIT6 						0x06	// 0000 0110
#define BIT7 						0x07	// 0000 0111

#define OVERRUN_BIT 				0x00
#define WATERMARK_BIT 				0x01
#define INACTIVITY_BIT 				0x03
#define ACTIVITY_BIT 				0x04
#define DATA_READY_BIT 				0x07

typedef enum STATUS_ADXL
{
	STATUS_OK_ADXL = 0x00,
	ERR_SPI,
	ERR_TRANSMIT,
	ERR_RECEIVE,
	ERR_READING,
	ERR_WRITE,
	ERR_ID
} STATUS_ADXL;

typedef enum HZ_SLEEP_MODE
{
	FREC_8HZ = 0,
	FREC_4HZ,
	FREC_2HZ,
	FREC_1HZ
} HZ_SLEEP_MODE;

typedef enum RANGE_SETTINGS
{
	RANGE_0_5_G = 0,
	RANGE_1_G,
	RANGE_2_G,
	RANGE_4_G
} RANGE_SETTINGS;

typedef enum AC_DC_SETTINGS
{
	DC_SET = 0,
	AC_SET
} AC_DC_SETTINGS;

typedef enum BANDWIDTH
{
	BW_3_125_Hz = 0x6,
	BW_6_25_Hz,
	BW_12_5_Hz,
	BW_25_Hz,
	BW_50_Hz,
	BW_100_Hz,
	BW_200_Hz,
	BW_400_Hz,
	BW_800_Hz,
	BW_1600_Hz
} BANDWIDTH;

typedef struct t_IntSource
{
	bool data_ready;
	bool activity;
	bool inactivity;
	bool watermark;
	bool overrun;
} t_IntSource;

/******************************************************************************************************************************************************************************/
/*																				 SPI INTERFACE 																				  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that writes to 8-bit register
 *
 * @param spi SPI interface
 * @param address register address
 * @param value value to write
 * @return STATUS_ADXL
 */
STATUS_ADXL Register_Write(SPI_HandleTypeDef *spi, uint8_t address, uint8_t value);

/**
 * @brief Function that reads 8-bit registers
 *
 * @param spi SPI interface
 * @param address Register address
 * @param pdata Pointer to value after lecture
 * @return STATUS_ADXL
 */
STATUS_ADXL Read_Byte(SPI_HandleTypeDef *spi, uint8_t address, uint8_t *pdata);

/**
 * @brief Function that reads 48-bit registers
 *
 * @param spi SPI interface
 * @param address register address
 * @param x_axis pointer to the value of the X-axis
 * @param y_axis pointer to the value of the Y-axis
 * @param z_axis pinter to the value of the Z-axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Read_6Bytes(SPI_HandleTypeDef *spi, uint8_t address, int16_t *x_axis, int16_t *y_axis, int16_t *z_axis);

/******************************************************************************************************************************************************************************/
/*																		Identification Functions																			  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that receives device ID. The value of this register is 0xAD
 *
 * @param spi SPI interface
 * @param data pointer to id value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Device_ID_0(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that receives device ID. The value of this register is 0x1D
 *
 * @param spi SPI interface
 * @param data pointer to id value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Device_ID_1(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that receives device identification. The value is 0xCB
 *
 * @param spi SPI interface
 * @param data Pointer to PARTID value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Part_ID(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that receives x id
 *
 * @param spi SPI interface
 * @param data Pointer to x id value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_X_ID(SPI_HandleTypeDef *spi, uint8_t *data);

/******************************************************************************************************************************************************************************/
/*																				Configuration Functions																		  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that receives power control register
 *
 * @param spi SPI interface
 * @param data Pointer to the power control value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Power_Control(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that sets ADXL313 funcionalities
 *
 * @param spi SPI interface
 * @param i2c true: i2c comunication / False: spi comunication
 * @param link A setting of 1 in the link bit with both the activity and inactivity
*  functions enabled delays the start of the activity function until inactivity is detected.
 * @param auto_sleep If the link bit is set, a setting of 1 in the AUTO_SLEEP bit sets
the ADXL313 to switch to sleep mode when inactivity is detected.
 * @param measure A setting of 0 in the measure bit places the part into standby mode,
and a setting of 1 places the part into measurement mode.
 * @param sleep A setting of 0 in the sleep bit puts the part into the normal mode
of operation, and a setting of 1 places the part into sleep mode.
Sleep mode suppresses DATA_READY
 * @param wake_up These bits control the frequency of readings in sleep mode
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Power_Control(SPI_HandleTypeDef *spi, bool i2c, bool link, bool auto_sleep, bool measure, bool sleep, uint8_t wake_up);

/**
 * @brief Function that receives the data format register
 *
 * @param spi SPI interface
 * @param data Pointer to the data format value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Data_format(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that sets the data format
 *
 * @param spi spi interface
 * @param self_test A setting of 1 in the SELF_TEST bit applies a self test force to the sensor, causing a shift in the output data
 * @param spi_state A value of 1 in the SPI bit sets the device to 3-wire SPI mode, and a value of 0 sets the device to 4-wire SPI mode.
 * @param int_invert A value of 0 in the INT_INVERT bit sets the interrupts to active high, and a value of 1 sets the interrupts to active low.
 * @param full_res When this bit is set to a value of 1, the device is in full resolution mode, where the output resolution increases with the g
 * @param justify A setting of 1 in the justify bit selects left (MSB) justified mode, and a setting of 0 selects right justified (LSB) mode with sign extension.
 * @param range g range
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Data_Format(SPI_HandleTypeDef *spi, bool self_test, bool spi_state, bool int_invert, bool full_res, bool justify, uint8_t range);

/**
 * @brief Function to set the bandwidth in Normal Mode or Low Power Mode
 *
 * @param spi SPI interface
 * @param low_power A setting of 0 in the LOW_POWER bit selects normal operation, and a setting of 1 selects reduced power operation, which has somewhat higher noise
 * @param rate These bits select the device bandwidth and output data rate
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Bandwidth_Rate(SPI_HandleTypeDef *spi, bool low_power, uint8_t rate);

/******************************************************************************************************************************************************************************/
/*																				Measures and calibrations																	  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that receives offset
 *
 * @param spi SPI interface
 * @param x_axis_offset Pointer to offset value
 * @param y_axis_offset Pointer to offset value
 * @param z_axis_offset Pointer to offset value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Offset(SPI_HandleTypeDef *spi, uint8_t *x_axis_offset, uint8_t *y_axis_offset, uint8_t *z_axis_offset);

/**
 * @brief Function that sets offset with a scale factor of 3.9 mg/LSB (that is, 0x7F = 0.5g)
 *
 * @param spi
 * @param x_axis
 * @param y_axis
 * @param z_axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Offset(SPI_HandleTypeDef *spi, uint8_t x_axis, uint8_t y_axis, uint8_t z_axis);

/**
 * @brief Function that receives acceleration value in g's
 *
 * @param spi SPI interface
 * @param x_axis Pointer to x axis
 * @param y_axis Pointer to y axis
 * @param z_axis Pointer to z axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Acceleration(SPI_HandleTypeDef *spi, float *x_axis, float *y_axis, float *z_axis);

/******************************************************************************************************************************************************************************/
/*																				Activity and Inactivity 																	  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that receives de activity or inactivity control register
 *
 * @param spi SPI interface
 * @param data Pointer to the value that we receive
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Activity_Inactivity_Control(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function Function that sets the activity and inactivity control
 *
 * @param spi SPI interface
 * @param activity_mode A setting of 0 selects dc-coupled operation, and a setting of 1 enables ac-coupled operation. In dc-coupled operation,
 * the current acceleration magnitude is compared directly with THRESH_ACT and THRESH_INACT to determine whether activity or inactivity is detected
 * In ac-coupled operation for activity detection, the acceleration value at the start of activity detection is taken as a reference value.
 * @param inactivity_mode
 * @param x_axis A setting of 1 enables x-, y-, or z-axis participation in detecting activity or inactivity
 * @param y_axis
 * @param z_axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Activity_Inactivity_Control(SPI_HandleTypeDef *spi, uint8_t activity_mode, uint8_t inactivity_mode, bool x_axis, bool y_axis, bool z_axis);

/**
 * @brief Function that receives threshold activity
 *
 * @param spi SPI interface
 * @param data pointer to the threshold we have chosen
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Threshold_Activity(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that sets if we want threshold activity
 *
 * @param spi SPI interface
 * @param value Factor value since 1-255 (the scale factor is 15.625 mg/LSB)
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Threshold_Activity(SPI_HandleTypeDef *spi, uint8_t value);

/**
 * @brief Function that receives threshold inactivity
 *
 * @param spi SPI interface
 * @param data Pointer to the threshold we have chosen
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Threshold_Inactivity(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that sets threshold inactivity
 *
 * @param spi SPI interface
 * @param value Factor value since 1-255 (the scale factor is 15.625 mg/LSB)
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Threshold_Inactivity(SPI_HandleTypeDef *spi, uint8_t value);

/**
 * @brief Fuction that receives the inactivity time
 *
 * @param spi SPI interface
 * @param data pointer to the inactivity time value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Time_Inactivity(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that sets when we want time inactivity (The scale factor is 1 sec/LSB. Unlike the other interrupt functions, which use unfiltered data(see the Threshold section),
 * the inactivity function uses filtered output data)
 *
 * @param spi SPI interface
 * @param value it contains an unsignedtime value
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Time_Inactivity(SPI_HandleTypeDef *spi, uint8_t value);

/******************************************************************************************************************************************************************************/
/*																				Interrupt Functions 																		  */
/******************************************************************************************************************************************************************************/

/**
 * @brief Function that receives the register value
 *
 * @param spi SPI interface
 * @param data Pointer to enable interrupt value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Interrupt_Enable(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that activates the interrupts
 *
 * @param spi SPI interface
 * @param data_ready The DATA_READY bit is set when new data is available and is cleared when no new data is available.
 * @param activity The activity bit is set when acceleration greater than the value stored in the THRESH_ACT register is sensed.
 * @param inactivity The inactivity bit is set when acceleration of less than the value stored in the THRESH_INACT register (Address 0x25) is sensed
 * for more time than is specified in the TIME_INACT register (Address 0x26)
 * @param watermark The watermark bit is set when the number of samples in the FIFO equals the value stored in the samples bits in the FIFO_CTL register
 * @param overrun The overrun bit is set when new data replaces unread data.
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Interrupt_Enable(SPI_HandleTypeDef *spi, bool data_ready, bool activity, bool inactivity, bool watermark, bool overrun);

/**
 * @brief Function that receives the register map (We set 1 to the bits we want)
 *
 * @param spi SPI interface
 * @param data Pointer to enable interrupt value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Interrupt_Pins(SPI_HandleTypeDef *spi, uint8_t *data);

/**
 * @brief Function that activates the interrupts (0 is pin1 and 1 is pin2)
 *
 * @param spi SPI interface
 * @param data_ready The DATA_READY bit is set when new data is available and is cleared when no new data is available.
 * @param activity The activity bit is set when acceleration greater than the value stored in the THRESH_ACT register is sensed.
 * @param inactivity The inactivity bit is set when acceleration of less than the value stored in the THRESH_INACT register (Address 0x25) is sensed
 * for more time than is specified in the TIME_INACT register (Address 0x26)
 * @param watermark The watermark bit is set when the number of samples in the FIFO equals the value stored in the samples bits in the FIFO_CTL register
 * @param overrun The overrun bit is set when new data replaces unread data.
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Interrupt_Pins(SPI_HandleTypeDef *spi, bool data_ready, bool activity, bool inactivity, bool watermark, bool overrun);

/**
 * @brief Function to get interrupt source
 *
 * @param spi SPI interface
 * @param p_int_source Pointer to struct
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Interrupt_Source(SPI_HandleTypeDef *spi, t_IntSource *p_int_source);
