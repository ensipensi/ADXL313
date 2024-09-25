#include "adxl.h"

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
STATUS_ADXL Register_Write(SPI_HandleTypeDef *spi, uint8_t address, uint8_t value)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data[2];
	data[0] = address |= 0x40; // see datasheet ADXL313 SPI
	data[1] = value;
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_RESET); // In this case, I use F411RE
	if (HAL_SPI_Transmit(spi, data, 2, 100))
	{
		ret_val = ERR_SPI;
	}
	else
	{
		ret_val = STATUS_OK_ADXL;
	}
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_SET);
	return ret_val;
}

/**
 * @brief Function that reads 8-bit registers
 *
 * @param spi SPI interface
 * @param address Register address
 * @param pdata Pointer to value after lecture
 * @return STATUS_ADXL
 */
STATUS_ADXL Read_Byte(SPI_HandleTypeDef *spi, uint8_t address, uint8_t *pdata)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	address |= 0x80; // See datasheet. It sets the bit 7 to 1
	address |= 0x40; // See datasheet. It sets the bit 6 to 1
	*pdata = 0;
	uint8_t tmp = 0;
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(spi, &address, 1, 100))
	{
		ret_val = ERR_TRANSMIT;
	}
	else if (HAL_SPI_Receive(spi, &tmp, 1, 100))
	{
		ret_val = ERR_RECEIVE;
	}
	else
	{
		*pdata = tmp;
	}
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_SET);
	return ret_val;
}

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
STATUS_ADXL Read_6Bytes(SPI_HandleTypeDef *spi, uint8_t address, int16_t *x_axis, int16_t *y_axis, int16_t *z_axis)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	address |= 0x80; // See datasheet. It sets the bit 7 to 1
	address |= 0x40; // See datasheet. It sets the bit 6 to 1

	uint8_t tmp[6];
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(spi, &address, 1, 100))
	{
		ret_val = ERR_TRANSMIT;
	}
	else if (HAL_SPI_Receive(spi, tmp, 6, 100))
	{
		ret_val = ERR_RECEIVE;
	}
	else
	{

		*x_axis = (int16_t)(tmp[1] << 8 | tmp[0]);
		*y_axis = (int16_t)(tmp[3] << 8 | tmp[2]);
		*z_axis = (int16_t)(tmp[5] << 8 | tmp[4]);
	}
	HAL_GPIO_WritePin(GPIOB, CS_Pin, GPIO_PIN_SET);
	return ret_val;
}

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
STATUS_ADXL Get_Device_ID_0(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	*data = 0;
	if (Read_Byte(spi, DEVID_0, &tmp))
	{
		ret_val = ERR_READING;
		if (ret_val == ERR_READING)
		{
			ret_val = ERR_ID;
		}
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that receives device ID. The value of this register is 0x1D
 *
 * @param spi SPI interface
 * @param data pointer to id value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Device_ID_1(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	*data = 0;
	if (Read_Byte(spi, DEVID_1, &tmp))
	{
		ret_val = ERR_READING;
		if (ret_val == ERR_READING)
		{
			ret_val = ERR_ID;
		}
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that receives device identification. The value is 0xCB
 *
 * @param spi SPI interface
 * @param data Pointer to PARTID value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Part_ID(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	*data = 0;
	if (Read_Byte(spi, PARTID, &tmp))
	{
		ret_val = ERR_READING;
		if (ret_val == ERR_READING)
		{
			ret_val = ERR_ID;
		}
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that receives x id
 *
 * @param spi SPI interface
 * @param data Pointer to x id value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_X_ID(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	*data = 0;
	if (Read_Byte(spi, XID, &tmp))
	{
		ret_val = ERR_READING;
		if (ret_val == ERR_READING)
		{
			ret_val = ERR_ID;
		}
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Get_Power_Control(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	*data = 0;
	if (Read_Byte(spi, PWR_CNTRL, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Set_Power_Control(SPI_HandleTypeDef *spi, bool i2c, bool link, bool auto_sleep, bool measure, bool sleep, uint8_t wake_up)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = i2c << 6 | link << 5 | auto_sleep << 4 | measure << 3 | sleep << 2 | wake_up;
	if (Register_Write(spi, PWR_CNTRL, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function that receives the data format register
 *
 * @param spi SPI interface
 * @param data Pointer to the data format value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Data_format(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	if (Read_Byte(spi, DATA_FORMAT, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Set_Data_Format(SPI_HandleTypeDef *spi, bool self_test, bool spi_state, bool int_invert, bool full_res, bool justify, uint8_t range)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = self_test << 7 | spi_state << 6 | int_invert << 5 | full_res << 3 | justify << 2 | range;
	if (Register_Write(spi, DATA_FORMAT, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function to set the bandwidth in Normal Mode or Low Power Mode
 *
 * @param spi SPI interface
 * @param low_power A setting of 0 in the LOW_POWER bit selects normal operation, and a setting of 1 selects reduced power operation, which has somewhat higher noise
 * @param rate These bits select the device bandwidth and output data rate
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Bandwidth_Rate(SPI_HandleTypeDef *spi, bool low_power, uint8_t rate)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = low_power << 4 | rate;
	if (Register_Write(spi, BW_RATE, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

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
STATUS_ADXL Get_Offset(SPI_HandleTypeDef *spi, uint8_t *x_axis_offset, uint8_t *y_axis_offset, uint8_t *z_axis_offset)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t z = 0;
	if (Read_Byte(spi, X_AXIS_OFFSET, &x))
	{
		ret_val = ERR_READING;
	}
	else if (Read_Byte(spi, Y_AXIS_OFFSET, &y))
	{
		ret_val = ERR_READING;
	}
	else if (Read_Byte(spi, Z_AXIS_OFFSET, &z))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*x_axis_offset = x;
		*y_axis_offset = y;
		*z_axis_offset = z;
	}
	return ret_val;
}

/**
 * @brief Function that sets offset with a scale factor of 3.9 mg/LSB (that is, 0x7F = 0.5g)
 *
 * @param spi
 * @param x_axis
 * @param y_axis
 * @param z_axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Offset(SPI_HandleTypeDef *spi, uint8_t x_axis, uint8_t y_axis, uint8_t z_axis)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	if (Register_Write(spi, X_AXIS_OFFSET, x_axis))
	{
		ret_val = ERR_WRITE;
	}
	else if (Register_Write(spi, Y_AXIS_OFFSET, y_axis))
	{
		ret_val = ERR_WRITE;
	}
	else if (Register_Write(spi, Z_AXIS_OFFSET, z_axis))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function that receives acceleration value in g's
 *
 * @param spi SPI interface
 * @param x_axis Pointer to x axis
 * @param y_axis Pointer to y axis
 * @param z_axis Pointer to z axis
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Acceleration(SPI_HandleTypeDef *spi, float *x_axis, float *y_axis, float *z_axis)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	if (Read_6Bytes(spi, MEASUREMENTS_DATA, &x, &y, &z))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*x_axis = (int16_t)x;
		*y_axis = (int16_t)y;
		*z_axis = (int16_t)z;
	}
	return ret_val;
}

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
STATUS_ADXL Get_Activity_Inactivity_Control(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	if (Read_Byte(spi, ACT_INACT_CNT, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Set_Activity_Inactivity_Control(SPI_HandleTypeDef *spi, uint8_t activity_mode, uint8_t inactivity_mode, bool x_axis, bool y_axis, bool z_axis)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = activity_mode << 7 | x_axis << 6 | y_axis << 5 | z_axis << 4 | inactivity_mode << 3 | x_axis << 2 | y_axis << 1 | z_axis << 0;
	if (Register_Write(spi, ACT_INACT_CNT, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function that receives threshold activity
 *
 * @param spi SPI interface
 * @param data pointer to the threshold we have chosen
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Threshold_Activity(SPI_HandleTypeDef *spi, uint8_t *data)
{
	uint8_t tmp = 0;
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	if (Read_Byte(spi, THRESHOLD_ACTIVITY, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that sets if we want threshold activity
 *
 * @param spi SPI interface
 * @param value Factor value since 1-255 (the scale factor is 15.625 mg/LSB)
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Threshold_Activity(SPI_HandleTypeDef *spi, uint8_t value)
{
	STATUS_ADXL ret_value = STATUS_OK_ADXL;
	if (Register_Write(spi, THRESHOLD_ACTIVITY, value))
	{
		ret_value = ERR_WRITE;
	}
	return ret_value;
}

/**
 * @brief Function that receives threshold inactivity
 *
 * @param spi SPI interface
 * @param data Pointer to the threshold we have chosen
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Threshold_Inactivity(SPI_HandleTypeDef *spi, uint8_t *data)
{
	uint8_t tmp = 0;
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	if (Read_Byte(spi, THRESHOLD_INACTIVITY, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that sets threshold inactivity
 *
 * @param spi SPI interface
 * @param value Factor value since 1-255 (the scale factor is 15.625 mg/LSB)
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Threshold_Inactivity(SPI_HandleTypeDef *spi, uint8_t value)
{
	STATUS_ADXL ret_value = STATUS_OK_ADXL;
	if (Register_Write(spi, THRESHOLD_INACTIVITY, value))
	{
		ret_value = ERR_WRITE;
	}
	return ret_value;
}

/**
 * @brief Fuction that receives the inactivity time
 *
 * @param spi SPI interface
 * @param data pointer to the inactivity time value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Time_Inactivity(SPI_HandleTypeDef *spi, uint8_t *data)
{
	uint8_t tmp = 0;
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	if (Read_Byte(spi, TIME_INACTIVITY, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

/**
 * @brief Function that sets when we want time inactivity (The scale factor is 1 sec/LSB. Unlike the other interrupt functions, which use unfiltered data(see the Threshold section),
 * the inactivity function uses filtered output data)
 *
 * @param spi SPI interface
 * @param value it contains an unsignedtime value
 * @return STATUS_ADXL
 */
STATUS_ADXL Set_Time_Inactivity(SPI_HandleTypeDef *spi, uint8_t value)
{
	STATUS_ADXL ret_value = STATUS_OK_ADXL;
	if (Register_Write(spi, TIME_INACTIVITY, value))
	{
		ret_value = ERR_WRITE;
	}
	return ret_value;
}

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
STATUS_ADXL Get_Interrupt_Enable(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	if (Read_Byte(spi, INTERRUPT_ENABLE, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Set_Interrupt_Enable(SPI_HandleTypeDef *spi, bool data_ready, bool activity, bool inactivity, bool watermark, bool overrun)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = data_ready << 7 | activity << 4 | inactivity << 3 | watermark << 1 | overrun;
	if (Register_Write(spi, INTERRUPT_ENABLE, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function that receives the register map (We set 1 to the bits we want)
 *
 * @param spi SPI interface
 * @param data Pointer to enable interrupt value
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Interrupt_Pins(SPI_HandleTypeDef *spi, uint8_t *data)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t tmp = 0;
	if (Read_Byte(spi, INTERRUPT_MAP, &tmp))
	{
		ret_val = ERR_READING;
	}
	else
	{
		*data = tmp;
	}
	return ret_val;
}

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
STATUS_ADXL Set_Interrupt_Pins(SPI_HandleTypeDef *spi, bool data_ready, bool activity, bool inactivity, bool watermark, bool overrun)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t data = 0;
	data = data_ready << 7 | activity << 4 | inactivity << 3 | watermark << 1 | overrun;
	if (Register_Write(spi, INTERRUPT_MAP, data))
	{
		ret_val = ERR_WRITE;
	}
	return ret_val;
}

/**
 * @brief Function to get interrupt source
 *
 * @param spi SPI interface
 * @param p_int_source Pointer to struct
 * @return STATUS_ADXL
 */
STATUS_ADXL Get_Interrupt_Source(SPI_HandleTypeDef *spi, t_IntSource *p_int_source)
{
	STATUS_ADXL ret_val = STATUS_OK_ADXL;
	uint8_t byte = 0;
	ret_val = Read_Byte(spi, INT_SOURCE, &byte);
	p_int_source->data_ready = ((byte >> DATA_READY_BIT) & 1);
	p_int_source->activity = ((byte >> ACTIVITY_BIT) & 1);
	p_int_source->inactivity = ((byte >> INACTIVITY_BIT) & 1);
	p_int_source->watermark = ((byte >> WATERMARK_BIT) & 1);
	p_int_source->overrun = ((byte >> OVERRUN_BIT) & 1);
	return ret_val;
}
