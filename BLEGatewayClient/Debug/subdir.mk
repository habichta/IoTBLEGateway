################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../client.c \
../cmd_def.c \
../response_callbacks.c \
../tpl.c \
../uart_driver.c 

OBJS += \
./client.o \
./cmd_def.o \
./response_callbacks.o \
./tpl.o \
./uart_driver.o 

C_DEPS += \
./client.d \
./cmd_def.d \
./response_callbacks.d \
./tpl.d \
./uart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


