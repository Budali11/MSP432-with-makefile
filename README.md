# 这是一个关于使用C++开发MCU的环境搭建教程

* ## make_test

	模仿stm32cubemx生成的makefile工程，尝试使用makefile对msp432进行开发，幸运成功的工程。教程文档在[此处](./make_test/Doc/Windows下使用makefile + GNU tools for arm 开发MSP432P401R.md)，也可以在make_test/Doc中找到。

* ## C_C++

	用C++对cortex-M4内核的外设做了一些封装，使用arm-none-eabi-g++编译即可。
	
	如若出现error：
	
	1. 复制C_C++下Core/Inc/CMSIS，替换自己工程里的CMSIS。
	2. Makefile里的变量 _LDFLASGS_ 中 ` -specs=nano.specs` 替换为 ` -specs=nosys.specs`，不要使用`-nostdlib`。
	3. 注意 _VPATH_ ，冒号':'后面不要紧接斜杠'/'。

* ## pwm_device_C++

	C++对UART和PWM设备做了一些简单封装。
