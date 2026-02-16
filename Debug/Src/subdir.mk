################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio.c \
../Src/led_pwm.c \
../Src/lighting.c \
../Src/main.c \
../Src/pwm.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timer.c \
../Src/uart.c 

OBJS += \
./Src/gpio.o \
./Src/led_pwm.o \
./Src/lighting.o \
./Src/main.o \
./Src/pwm.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timer.o \
./Src/uart.o 

C_DEPS += \
./Src/gpio.d \
./Src/led_pwm.d \
./Src/lighting.d \
./Src/main.d \
./Src/pwm.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timer.d \
./Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I../Inc -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Core/Include -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/gpio.cyclo ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/led_pwm.cyclo ./Src/led_pwm.d ./Src/led_pwm.o ./Src/led_pwm.su ./Src/lighting.cyclo ./Src/lighting.d ./Src/lighting.o ./Src/lighting.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/pwm.cyclo ./Src/pwm.d ./Src/pwm.o ./Src/pwm.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timer.cyclo ./Src/timer.d ./Src/timer.o ./Src/timer.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su

.PHONY: clean-Src

