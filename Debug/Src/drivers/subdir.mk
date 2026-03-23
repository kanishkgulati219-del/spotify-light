################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/drivers/gpio.c \
../Src/drivers/pwm.c \
../Src/drivers/timer.c \
../Src/drivers/uart.c 

OBJS += \
./Src/drivers/gpio.o \
./Src/drivers/pwm.o \
./Src/drivers/timer.o \
./Src/drivers/uart.o 

C_DEPS += \
./Src/drivers/gpio.d \
./Src/drivers/pwm.d \
./Src/drivers/timer.d \
./Src/drivers/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/drivers/%.o Src/drivers/%.su Src/drivers/%.cyclo: ../Src/drivers/%.c Src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I../Inc -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Core/Include -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-drivers

clean-Src-2f-drivers:
	-$(RM) ./Src/drivers/gpio.cyclo ./Src/drivers/gpio.d ./Src/drivers/gpio.o ./Src/drivers/gpio.su ./Src/drivers/pwm.cyclo ./Src/drivers/pwm.d ./Src/drivers/pwm.o ./Src/drivers/pwm.su ./Src/drivers/timer.cyclo ./Src/drivers/timer.d ./Src/drivers/timer.o ./Src/drivers/timer.su ./Src/drivers/uart.cyclo ./Src/drivers/uart.d ./Src/drivers/uart.o ./Src/drivers/uart.su

.PHONY: clean-Src-2f-drivers

