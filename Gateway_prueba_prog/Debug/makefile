################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/Task/subdir.mk
-include Src/State/subdir.mk
-include Src/Kernel/subdir.mk
-include Src/Drivers_Hd/subdir.mk
-include Src/Common/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Gateway_prueba_prog.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Gateway_prueba_prog.list \


# All Target
all: Gateway_prueba_prog.elf secondary-outputs

# Tool invocations
Gateway_prueba_prog.elf: $(OBJS) $(USER_OBJS) C:\Users\Julian\Desktop\TdeG_CESE\Gateway_CESE_2019\Gateway_prueba_prog\STM32L471RGTX_FLASH.ld
	arm-none-eabi-gcc -o "Gateway_prueba_prog.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Julian\Desktop\TdeG_CESE\Gateway_CESE_2019\Gateway_prueba_prog\STM32L471RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Gateway_prueba_prog.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Gateway_prueba_prog.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Gateway_prueba_prog.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
