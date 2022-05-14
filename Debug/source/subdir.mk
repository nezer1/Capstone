################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Bitmaps.c \
../source/Final.c \
../source/Human_Touch.c \
../source/LED_Indicator.c \
../source/OLED_FACE.c \
../source/SD.c \
../source/SPI.c \
../source/Session_Selection_Unit.c \
../source/audio.c \
../source/diskio.c \
../source/ff.c \
../source/hand_movement.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/Bitmaps.o \
./source/Final.o \
./source/Human_Touch.o \
./source/LED_Indicator.o \
./source/OLED_FACE.o \
./source/SD.o \
./source/SPI.o \
./source/Session_Selection_Unit.o \
./source/audio.o \
./source/diskio.o \
./source/ff.o \
./source/hand_movement.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/Bitmaps.d \
./source/Final.d \
./source/Human_Touch.d \
./source/LED_Indicator.d \
./source/OLED_FACE.d \
./source/SD.d \
./source/SPI.d \
./source/Session_Selection_Unit.d \
./source/audio.d \
./source/diskio.d \
./source/ff.d \
./source/hand_movement.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"D:\MCU_XPRESSS\Capstone_Project\board" -I"D:\MCU_XPRESSS\Capstone_Project\source" -I"D:\MCU_XPRESSS\Capstone_Project" -I"D:\MCU_XPRESSS\Capstone_Project\startup" -I"D:\MCU_XPRESSS\Capstone_Project\CMSIS" -I"D:\MCU_XPRESSS\Capstone_Project\drivers" -I"D:\MCU_XPRESSS\Capstone_Project\utilities" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


