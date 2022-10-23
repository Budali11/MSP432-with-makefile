# 这是一个关于使用C/C++开发MCU的环境搭建教程

* ## make_test

	模仿stm32cubemx生成的makefile工程，尝试使用makefile对msp432进行开发，幸运成功的工程。教程文档在[此处](https://github.com/Budali11/MSP432-with-makefile-and-Cpp/tree/main/make_test/Doc/Windows下使用makefile%20%2B%20GNU%20tools%20for%20arm%20开发MSP432P401R.md)，也可以在make_test/Doc中找到。

* ## C_C++

	用C++对cortex-M4内核的外设做了一些封装，使用arm-none-eabi-g++编译即可。
	
	如若出现error：
	
	1. 复制C_C++下Core/Inc/CMSIS，替换自己工程里的CMSIS。
	2. Makefile里的变量 _LDFLASGS_ 中 ` -specs=nano.specs` 替换为 ` -specs=nosys.specs`，不要使用`-nostdlib`。
	3. 注意 _VPATH_ ，冒号':'后面不要紧接斜杠'/'。

* ## pwm_device_C++

	C++对UART和PWM设备做了一些简单封装。
	
* ## mpu6050_soft

  C++创建了一个抽象的IIC设备类，用以实现与IIC从机设备的通信。

  该类的用户接口有：

  * init:
    * 实现IIC设备的初始化。
  * add_slave:
    * 添加一个slave设备到类私有的设备链表。
    * 令该slave设备获得IIC操作方法。
  * select_slave:
    * 选择一个从机设备来进行操作。
  * write:
    * 向总线进行写操作。
  * read:
    * 向总线进行读操作。

  v0.1：初步实现IIC通信。

  v0.2：实现IIC总线-设备-驱动模型。（待实现）
