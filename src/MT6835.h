/** @file MT6835.h
 *
 * @brief A library for Arduino boards that reads angles from MT6835 and MT6835 sensors.
 * 		  Also support configuration of the sensor parameters.
 *
 * @par
 * COPYRIGHT NOTICE: MIT License
 *
 * 	Copyright (c) 2020 Adrien Legrand <contact@adrien-legrand.com>
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



#ifndef MT6835_h
#define MT6835_h

#include "Arduino.h"
#include <SPI.h>

// Volatile Registers Addresses
#define USER_ID			    0x001  //客户保留寄存器，可任意使用
#define NOT_USED		    0x002  //未使用
#define ANGLE_H 		    0x003  //角度数据寄存器 ANGLE[20:13]
#define ANGLE_M 		    0x004  //角度数据寄存器 ANGLE[12:5]
#define ANGLE_L 		    0x005  //角度数据寄存器 ANGLE[4:0]  STATUS[2:0]
#define ANGLE_CRC 		    0x006  //角度数据寄存器 CRC[7:0]
#define ABZ_RESOLUTION_H	0x007  //ABZ分辨率及相关寄存器 ABZ_RES[13:6]
#define ABZ_RESOLUTION_L	0x008  //ABZ分辨率及相关寄存器 ABZ_RES[5:0]  bit1:ABZ_OFF  bit0:AB_SWAP
#define AUTOCAL_FREQ    	0x00E  //自校准所采用的转速区间 bit7:GPIO_DS  bit4~6: AUTOCAL_FREQ[2:0]
#define CALIBRATION_STATUS	0x113  //校准状态寄存器 0x113[7:6] = 00：没有进行校准  01：正在进行校准  10：自校准失败  11：自校准成功

//commends
#define READ			        0x03  //读取
#define WRITE			        0x06  //写入
#define WRITE_EEPROM			0x0C  //写EEPROM
#define SET_ZERO_POINT			0x05  //自动设置零点
#define CONTINOUS_READ			0x0A  //连续读取
//ack
#define ACK_DATA            0x55  //写操作返回的确认码

typedef enum
{
    MT_SUCSESS = 0x00,      //成功
    MT_SEEP_TOO_FAST,       //转速过快（超过12万转/分钟）
    MT_MAGNETIC_WEAK,       //外加磁场太弱
    MT_LOW_POWER,           //芯片供电欠压
    MT_CRC_ERR              // crc校验错误
} mt_status_t;

typedef enum
{
    CALI_NO = 0x00,      //没有进行校准
    CALI_ING,            //正在校准
    CALI_FAIL,           //校准失败
    CALI_SUCSESS,        //校准成功
} calibration_status_t;




// ABZ RES Register Definition
typedef union {
    uint16_t raw;
    struct __attribute__ ((packed)) {
        uint16_t ab_swap:1;//A信号B信号互换  0：不互换   1：互换
        uint16_t abz_off:1;//ABZ输出引脚的开关状态  0：开  1：关
        uint16_t res:14;//AB输出的分辨率  0x0~0x3FFF
    } values;
} ABZ_RES;



// Command Frame  Definition
typedef union {
    uint16_t raw;
    struct __attribute__ ((packed)) {
        uint16_t address:12; //寄存器地址
        uint16_t cmd:4; //命令字段
    } values;
} CommandFrame;




class MT6835 {
  public:
    MT6835(uint32_t clock);
    uint8_t readRegister(uint16_t registerAddress);
    void readRegisterContinus(uint16_t registerAddress, uint8_t* re_data, uint32_t len);
    void writeRegister(uint16_t registerAddress, uint8_t registerValue);
    void writeAbzRes(uint16_t abzResValue);
    uint8_t writeEEprom(void);
    uint8_t autoSetZeroPoint(void);
    mt_status_t readAngleByte(float *angle);
    mt_status_t readAngle(float *angle);
    calibration_status_t readCalibrationStatus(void);
    void printDebugString();
};

#endif // #MT6835_h

