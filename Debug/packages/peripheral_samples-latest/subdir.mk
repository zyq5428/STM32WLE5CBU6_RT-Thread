################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../packages/peripheral_samples-latest/rtc_sample.c 

OBJS += \
./packages/peripheral_samples-latest/rtc_sample.o 

C_DEPS += \
./packages/peripheral_samples-latest/rtc_sample.d 


# Each subdirectory must supply rules for building sources it contributes
packages/peripheral_samples-latest/%.o: ../packages/peripheral_samples-latest/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32WL -DUSE_HAL_DRIVER -DSTM32WLE5xx -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers\include\wl" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\Device\ST\STM32WLxx\Include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\RTOS\Template" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\STM32WLxx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\STM32WLxx_HAL_Driver\Inc\Legacy" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\cubemx" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\cubemx\Inc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\packages\peripheral_samples-latest" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\compilers\common\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\compilers\newlib" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\io\poll" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\io\stdio" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\ipc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\utilities\ulog" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

