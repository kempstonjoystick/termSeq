################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/audioTrack.cpp \
../src/eventListener.cpp \
../src/termSeq.cpp \
../src/thread.cpp 

OBJS += \
./src/audioTrack.o \
./src/eventListener.o \
./src/termSeq.o \
./src/thread.o 

CPP_DEPS += \
./src/audioTrack.d \
./src/eventListener.d \
./src/termSeq.d \
./src/thread.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include -I/usr/include/SDL2/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


