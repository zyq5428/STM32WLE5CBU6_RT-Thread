################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/common/backtrace.c \
../rt-thread/libcpu/arm/common/div0.c \
../rt-thread/libcpu/arm/common/showmem.c 

OBJS += \
./rt-thread/libcpu/arm/common/backtrace.o \
./rt-thread/libcpu/arm/common/div0.o \
./rt-thread/libcpu/arm/common/showmem.o 

C_DEPS += \
./rt-thread/libcpu/arm/common/backtrace.d \
./rt-thread/libcpu/arm/common/div0.d \
./rt-thread/libcpu/arm/common/showmem.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/common/%.o: ../rt-thread/libcpu/arm/common/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32WL -DUSE_HAL_DRIVER -DSTM32WLE5xx -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\drivers\include\wl" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\Device\ST\STM32WLxx\Include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\CMSIS\RTOS\Template" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\STM32WLxx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\libraries\STM32WLxx_HAL_Driver\Inc\Legacy" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\cubemx" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\cubemx\Inc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications\SubGHz\App" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications\SubGHz\common" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications\SubGHz\SubGHz_Phy" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications\SubGHz\Target" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\applications\SubGHz\SubGHz_Phy\stm32_radio_driver" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\compilers\common\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\compilers\newlib" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\io\poll" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\io\stdio" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\libc\posix\ipc" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\components\utilities\ulog" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

