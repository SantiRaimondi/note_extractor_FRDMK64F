################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\drivers" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\CMSIS\DSP\Include" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\utilities" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\serial_manager" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\uart" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\lists" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\CMSIS" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\device" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\board" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\source" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\drivers" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\CMSIS\DSP\Include" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\utilities" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\serial_manager" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\uart" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\component\lists" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\CMSIS" -I"C:\Users\santi\Desktop\Datardos\Facultad\Segundo semestre Quinto\DSP\workspace\note_extractor_FRDMK64F\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


