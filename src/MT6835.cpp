/** @file MT6835.cpp
 *
 * @brief A library for Arduino boards that reads angles from MT6835 sensors.
 * 		  Also support configuration of the sensor parameters.
 *
 * @par
 * COPYRIGHT NOTICE: MIT License
 *
 * 	
 *
 * 	Permission is hereby granted, free of charge, to any person obtaining a copy
 * 	of this software and associated documentation files (the "Software"), to deal
 * 	in the Software without restriction, including without limitation the rights
 * 	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * 	copies of the Software, and to permit persons to whom the Software is
 * 	furnished to do so, subject to the following conditions:
 *
 * 	The above copyright notice and this permission notice shall be included in all
 * 	copies or substantial portions of the Software.
 *
 * 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * 	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * 	SOFTWARE.
 *
*/

#include "MT6835.h"

SPIClass hspi;

extern uint8_t get_crc8(const void *Buf, uint32_t Len);
/**
 ****************************************************************************************
 * @brief   MT6835 SPI初始化
 *
 * @param _clock                SPI时钟
 *
 ****************************************************************************************
 */
MT6835::MT6835(uint32_t _clock){
	hspi.begin();
	hspi.setHwCs(true);
	hspi.beginTransaction(SPISettings(_clock, MSBFIRST, SPI_MODE3));
	
}
/**
 ****************************************************************************************
 * @brief   MT6835 读取寄存器数据
 *
 * @param registerAddress        寄存器地址
 * 
 * @retval  该寄存器数值    注意：使用硬件CS ，读取数据需发送一次数据，使用 transferBytes(const uint8_t * data, uint8_t * out, uint32_t size)，其中 size 即为 发送数据和接收数据的总长度
 *
 ****************************************************************************************
 */
uint8_t MT6835::readRegister(uint16_t registerAddress) {
	CommandFrame command;
	command.values.cmd = READ;
	command.values.address = registerAddress;

	uint8_t senddata[3] = {0};
	uint8_t re_data[3] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	hspi.transferBytes(senddata,re_data,3);
	return re_data[2]; // 前两个字节舍弃，第三个字节即为实际读取的数值
}
/**
 ****************************************************************************************
 * @brief   MT6835 连续读取寄存器数据
 *
 * @param registerAddress        寄存器地址
 * 
 * @retval 该连续寄存器数值
 *
 ****************************************************************************************
 */
void MT6835::readRegisterContinus(uint16_t registerAddress,uint8_t* re_data,uint32_t len) {
	CommandFrame command;
	command.values.cmd = CONTINOUS_READ;
	command.values.address= registerAddress;
	uint8_t senddata[len] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	hspi.transferBytes(senddata,re_data,len);
}

/**
 ****************************************************************************************
 * @brief   MT6835 写数据到寄存器
 *
 * @param registerAddress        寄存器地址
 * @param registerValue          写到该寄存器的值
 * 
 * @retval  None
 *
 ****************************************************************************************
 */
void MT6835::writeRegister(uint16_t registerAddress, uint8_t registerValue) {
	CommandFrame command;
	command.values.cmd = WRITE;
	command.values.address = registerAddress;

	uint8_t senddata[3] = {0};
	uint8_t re_data[3] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	senddata[2] = registerValue;
	hspi.transferBytes(senddata,re_data,3);

}
/**
 ****************************************************************************************
 * @brief   MT6835 写ABZ分辨率到相关寄存器
 *
 * @param abzResValue        ABZ分辨率  取值范围 0x0000 ~ 0x3FFF
 * 
 * @retval  None
 *
 ****************************************************************************************
 */
void MT6835::writeAbzRes(uint16_t abzResValue) {
	CommandFrame command;
	command.values.cmd = WRITE;
	command.values.address = ABZ_RESOLUTION_H;



	ABZ_RES abz_res;
	abz_res.values.res = abzResValue;
	abz_res.values.ab_swap = 0;
	abz_res.values.abz_off = 0;


	uint8_t senddata[4] = {0};
	uint8_t re_data[4] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	senddata[2] = abz_res.raw >> 8;
	senddata[3] = abz_res.raw ;
	hspi.transferBytes(senddata,re_data,4);
}
/**
 ****************************************************************************************
 * @brief   MT6835 写数据到EEPROM
 *
 * @param 	None
 * 
 * @retval  确认码   0x55则表示指令接收正确
 *
 ****************************************************************************************
 */
uint8_t MT6835::writeEEprom(void) {
	CommandFrame command;
	command.values.cmd = WRITE_EEPROM;
	command.values.address = 0;

	uint8_t senddata[3] = {0};
	uint8_t re_data[3] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	hspi.transferBytes(senddata,re_data,3);
	return re_data[2];


}
/**
 ****************************************************************************************
 * @brief   MT6835 自动设置零点
 *
 * @param None
 * 
 * @retval  确认码   0x55则表示指令接收正确. 此操作只是将当前角度写入到寄存器，并没有烧录到EEPROM中。
 *
 ****************************************************************************************
 */
uint8_t MT6835::autoSetZeroPoint(void) {
	CommandFrame command;
	command.values.cmd = SET_ZERO_POINT;
	command.values.address = 0;

	uint8_t senddata[3] = {0};
	uint8_t re_data[3] = {0};
	senddata[0] = command.raw >> 8;
	senddata[1] = command.raw;
	hspi.transferBytes(senddata,re_data,3);
	return re_data[2];
}
/**
 ****************************************************************************************
 * @brief   MT6835 读取当前绝对角度值  一次读取一个字节
 *
 * @param 	angle   存储角度指针
 * 
 * @retval 错误码，详见mt_status_t
 *
 ****************************************************************************************
 */
mt_status_t MT6835::readAngleByte(float* angle) {
	uint8_t readData[4];
	readData[0]  = readRegister(ANGLE_H);
	readData[1]  = readRegister(ANGLE_M);
	readData[2]  = readRegister(ANGLE_L);
	readData[3]  = readRegister(ANGLE_CRC);

	uint8_t crc_result = get_crc8(readData, 3);
	if(crc_result == readData[3])
	{
		if ((readData[2] & 0x07) == 0)
		{
			*angle = ( (readData[0] << 13) + (readData[1] << 5) + (readData[2] >> 3))/2097152.*360.;
			return MT_SUCSESS;
		}
		else
		{
			if((readData[2] & 0x01) == 1)
			return MT_SEEP_TOO_FAST;
			else if((readData[2] & 0x02) == 1)
			return MT_MAGNETIC_WEAK;
			else if((readData[2] & 0x04) == 1)
			return MT_LOW_POWER;
		}
	}
	else
	{
		Serial.println("crc err");
		return MT_CRC_ERR;
	}
	return MT_SUCSESS;

}
/**
 ****************************************************************************************
 * @brief   MT6835 使用连续读取指令读取当前绝对角度值
 *
 * @param 	angle   存储角度指针
 * 
 * @retval 错误码，详见mt_status_t
 *
 ****************************************************************************************
 */
mt_status_t MT6835::readAngle(float* angle) {
	uint8_t readData[6] = {0};
	readRegisterContinus(ANGLE_H,readData,6);

	uint8_t crc_result = get_crc8(&readData[2], 3);
	if(crc_result == readData[5])
	{
		if ((readData[4] & 0x07) == 0)
		{
			*angle = ( (readData[2] << 13) + (readData[3] << 5) + (readData[4] >> 3))/2097152.*360.;
			return MT_SUCSESS;
		}
		else
		{
			if((readData[4] & 0x01) == 1)
			return MT_SEEP_TOO_FAST;
			if((readData[4] & 0x02) == 1)
			return MT_MAGNETIC_WEAK;
			if((readData[4] & 0x04) == 1)
			return MT_LOW_POWER;
		}
	}
	else
	{
		Serial.println("crc err");
		return MT_CRC_ERR;
	}
	return MT_SUCSESS;

}


/**
 ****************************************************************************************
 * @brief   MT6835 读取校准状态
 *
 * @param 	
 * 
 * @retval 错误码，详见calibration_status_t
 *
 ****************************************************************************************
 */
calibration_status_t MT6835::readCalibrationStatus(void) {
	uint8_t _readData = readRegister(CALIBRATION_STATUS);
	uint8_t readData = _readData >> 6;
	switch(readData)
	{
		case 0:
		{
			Serial.println("CALI_NO");
			return CALI_NO;
		}
		break;
		case 1:
		{
			Serial.println("CALI_ING");
			return CALI_ING;
		}
		break;
		case 2:
		{
			Serial.println("CALI_FAIL");
			return CALI_FAIL;
		}
		break;
		case 3:
		{
			Serial.println("CALI_SUCSESS");
			return CALI_SUCSESS;
		}
		break;
		default:
		break;
	}
	return CALI_NO;
}


void MT6835::printDebugString() {
	uint8_t readData;
	static uint8_t testdata = 0;
	static uint8_t flag = 1;
	// Serial.println("======== MT6835 Debug ========");
	// readData = readRegister(0x0007);
	// Serial.printf("0007 Register : %02x\r\n",readData);

	// readData = readRegister(0x0008);
	// Serial.printf("0008 Register : %02x\r\n",readData);

	// readData = readRegister(0x0009);
	// Serial.printf("0009 Register : %02x\r\n",readData);

	// readData = readRegister(0x000A);
	// Serial.printf("000A Register : %02x\r\n",readData);

	// readData = readRegister(0x000B);
	// Serial.printf("000B Register : %02x\r\n",readData);

	// readData = readRegister(0x000C);
	// Serial.printf("000C Register : %02x\r\n",readData);

	// readData = readRegister(0x000D);
	// Serial.printf("000D Register : %02x\r\n",readData);

	// readData = readRegister(0x000E);
	// Serial.printf("000E Register : %02x\r\n",readData);

	// readData = readRegister(0x0011);
	// Serial.printf("0011 Register : %02x\r\n",readData);


	// readData = readRegister(0x001);
	// Serial.printf("001 Register : %02x\r\n",readData);

	if(flag == 0)
		return;

	testdata++;
	writeRegister(0x001, testdata);
	readData = readRegister(0x001);
	Serial.printf("001 Register : %02x\r\n",readData);

	if(testdata != readData)
	{
		Serial.printf("err   ==============================  %d  %d",testdata,readData);
		flag = 0;
	}

	//Serial.println(spi.getclock);

}


