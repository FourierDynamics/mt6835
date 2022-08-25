
# MT6835  Arduino 库文件

该库文件是MT6835磁性编码器连接到ESP32 Arduino 板的代码库，可以读取从编码器测量到的角度值，和配置编码器的功能


## 功能

 - [x] SPI 与编码器通信
 - [x] 所有寄存器的读写操作
 - [x] 读取角度数据
 - [x] 校准



## 下载
在platformio.ini中：
lib_deps =  https://github.com/FourierDynamics/mt6835.git
### 示例
库中提供了两个示例 :

 - **ReadAngle.ino**  显示如何连接传感器以及如何读取测量角度.
 - **AutoCalibration.ino** 显示如何校准，并查看校准结果. 
 
