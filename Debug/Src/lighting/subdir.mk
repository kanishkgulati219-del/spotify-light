################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/lighting/led_pwm.c \
../Src/lighting/lighting_engine.c \
../Src/lighting/lighting_modes.c 

OBJS += \
./Src/lighting/led_pwm.o \
./Src/lighting/lighting_engine.o \
./Src/lighting/lighting_modes.o 

C_DEPS += \
./Src/lighting/led_pwm.d \
./Src/lighting/lighting_engine.d \
./Src/lighting/lighting_modes.d 


# Each subdirectory must supply rules for building sources it contributes
Src/lighting/%.o Src/lighting/%.su Src/lighting/%.cyclo: ../Src/lighting/%.c Src/lighting/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I../Inc -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Core/Include -I/Users/kanishkgulati/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-lighting

clean-Src-2f-lighting:
	-$(RM) ./Src/lighting/led_pwm.cyclo ./Src/lighting/led_pwm.d ./Src/lighting/led_pwm.o ./Src/lighting/led_pwm.su ./Src/lighting/lighting_engine.cyclo ./Src/lighting/lighting_engine.d ./Src/lighting/lighting_engine.o ./Src/lighting/lighting_engine.su ./Src/lighting/lighting_modes.cyclo ./Src/lighting/lighting_modes.d ./Src/lighting/lighting_modes.o ./Src/lighting/lighting_modes.su

.PHONY: clean-Src-2f-lighting

