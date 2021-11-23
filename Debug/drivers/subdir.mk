################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_dac.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_dac.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_dac.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\drivers" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\CMSIS\DSP\Include" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\utilities" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\serial_manager" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\uart" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\lists" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\CMSIS" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\device" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\board" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\source" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\drivers" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\CMSIS\DSP\Include" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\utilities" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\serial_manager" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\uart" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\component\lists" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\CMSIS" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\fft_note_extractor\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


