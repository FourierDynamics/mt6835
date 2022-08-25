/*
  ReadAngle

  Read the angle position from an MT6835 rotary encoder.
  Start the terminal with a speed of 9600 Bauds to see the measured angle.

  The encoder is connected as follows:
  MT6835         ESP32 Board
  3.3V <-------------> 3.3V
   GND <-------------> GND
  MOSI <-------------> MOSI (Pin 13 for ESP32 Board)
  MISO <-------------> MISO (Pin 12 for ESP32 Board)
   SCK <-------------> SCK  (Pin 14 for ESP32 Board)
   CSN <-------------> SS   (Pin 15 for ESP32 Board)
  See Arduino SPI Reference to see how to connect with other boards.
  https://www.arduino.cc/en/reference/SPI

自校准模式由引脚4（CAL_EN）控制，当引脚4由低电平加到高电平时，MT6835就进入自校准状态
此时让系统匀速旋转，MT6835自带的处理电路就会自动计算相关的非线性参数和校准，并将校准的参数自动烧录进EEPROM
校准步骤：
1. 配置自校准允许的转速区间，通过SPI接口配置AUTO_CAL_FREQ[2:0]（寄存器0x00E 的bit4~6）, 其值与转速区间关系如下：
    AUTO_CAL_FREQ[2:0]          自校准采用的速度区间（转/分钟）
         0x0                          3200<= 速度 <6400
         0x1                          1600<= 速度 <3200
         0x2                           800<= 速度 <1600
         0x3                           400<= 速度 <800
         0x4                           200<= 速度 <400
         0x5                           100<= 速度 <200
         0x6                            50<= 速度 <100
         0x7                            25<= 速度 <50
2. 将系统平稳运行在目标转速
3. 在系统已经平稳匀速运转的情况下，将引脚4接高电平进入自校准状态，保持这一转速继续运转64圈以上，
   可以通过SPI接口从寄存器0x113[7:6]读取自校准状态：
   0x113[7:6]         自校准状态
      00              没有进行自校准
      01              正在进行自校准
      10              自校准失败
      11              自校准成功
4. 如果自家准运行失败，请检查系统连接和配置，并重复上述步骤1~3
5. 自校准成功后，等待>6秒钟，请务必给芯片断电
6. 芯片从重新上电才可以进行别的操作


*/

// Include the library
#include <MT6835.h>

#define  CAL_EN_PIN    17   //自校准模式控制引脚

// Define spi clock. 
uint32_t spiClock = 16000000;  //MT6835最高支持16MHz的SPI时钟

// Start connection to the sensor.
MT6835 mt6835(spiClock);

void setup() {
  // Initialize a Serial Communication in order to
  // print the measured angle.
  Serial.begin(115200);
  pinMode(CAL_EN_PIN, OUTPUT);
  digitalWrite(CAL_EN_PIN, LOW);

  //设置客户端自校准所采用的的校准区间 400~800 转/分钟，详见mt6835 datasheet
  mt6835.writeRegister(AUTOCAL_FREQ,0x30);

  //设置AB脉冲分辨率 16384脉冲/圈，详见mt6835 datasheet
  ABZ_RES abz_res ;
  abz_res.values.res = 0X3FFF;
  abz_res.values.ab_swap = 0;
  abz_res.values.abz_off = 0;

  Serial.printf("abz_res : %04x\r\n",abz_res.raw);

  mt6835.writeRegister(ABZ_RESOLUTION_H,(uint8_t) (abz_res.raw>>8));
  mt6835.writeRegister(ABZ_RESOLUTION_L,(uint8_t) abz_res.raw);

  delay(1000);
  digitalWrite(CAL_EN_PIN, HIGH); //控制引脚由低电平接到高电平时，进入自校准模式
}

void loop() {
  // Read calibration status
  mt6835.readCalibrationStatus();

  // Wait 10 milliseconds  before reading again.
  delay(10);
}
