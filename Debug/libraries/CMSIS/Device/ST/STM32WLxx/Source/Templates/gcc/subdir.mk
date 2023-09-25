################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../libraries/CMSIS/Device/ST/STM32WLxx/Source/Templates/gcc/startup_stm32wle5xx.S 

OBJS += \
./libraries/CMSIS/Device/ST/STM32WLxx/Source/Templates/gcc/startup_stm32wle5xx.o 

S_UPPER_DEPS += \
./libraries/CMSIS/Device/ST/STM32WLxx/Source/Templates/gcc/startup_stm32wle5xx.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/CMSIS/Device/ST/STM32WLxx/Source/Templates/gcc/%.o: ../libraries/CMSIS/Device/ST/STM32WLxx/Source/Templates/gcc/%.S
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -x assembler-with-cpp -I"D:\RT-ThreadStudio\workspace\STM32WLE5CBU6_RT-Thread" -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

