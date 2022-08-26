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


*/

// Include the library
#include <MT6835.h>

#define  CAL_EN_PIN    32   //自校准模式控制引脚,根据实际情况修改

// Define spi clock. 
uint32_t spiClock = 16000000;  //MT6835最高支持16MHz的SPI时钟

// Start connection to the sensor.
MT6835 mt6835(spiClock);

void setup() {
  // Initialize a Serial Communication in order to
  // print the measured angle.
  Serial.begin(115200);
  pinMode(CAL_EN_PIN, OUTPUT);
  digitalWrite(CAL_EN_PIN, LOW);//初始化时需要将自校准控制引脚拉低，防止进入自校准模式


  mt6835.writeRegister(AUTOCAL_FREQ,0x30);  //设置客户端自校准所采用的的校准区间 400~800 转/分钟，详见mt6835 datasheet

  //设置AB脉冲分辨率 16384脉冲/圈，详见mt6835 datasheet
  ABZ_RES abz_res ;
  abz_res.values.res = 0X3FFF;
  abz_res.values.ab_swap = 0;
  abz_res.values.abz_off = 0;

  Serial.printf("abz_res : %04x\r\n",abz_res.raw);

  mt6835.writeRegister(ABZ_RESOLUTION_H,(uint8_t) (abz_res.raw>>8));
  mt6835.writeRegister(ABZ_RESOLUTION_L,(uint8_t) abz_res.raw);
}

void loop() {
  // Read the measured angle
  float angle = 0;
  mt6835.readAngle(&angle);

  // Show the measure angle on the Serial Port
  Serial.println(angle);

  // Wait 1 second (1000 milliseconds) before reading again.
  delay(1000);
}
