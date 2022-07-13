ifeq ($(MODULE),prebootloader-mbr)

ASRC += $(COMMON_BUILD)/arm/startup/startup_$(MCU_DEVICE_LC).S
ASFLAGS += -I$(COMMON_BUILD)/arm/startup

# Linker flags
LDFLAGS += -Wl,-Map,$(TARGET_BASE).map

LINKER_DEPS += $(COMMON_BUILD)/arm/linker/linker_module_start.ld
LINKER_DEPS += $(COMMON_BUILD)/arm/linker/linker_module_end.ld
LINKER_DEPS += $(COMMON_BUILD)/arm/linker/linker_module_info.ld

ifeq (,$(PREBOOTLOADER_MBR_MODULE_DEPENDENCY))
PREBOOTLOADER_MBR_MODULE_DEPENDENCY=0,0,0
endif

ifeq (,$(PREBOOTLOADER_MBR_MODULE_DEPENDENCY2))
PREBOOTLOADER_MBR_MODULE_DEPENDENCY2=0,0,0
endif

ifeq (,$(PREBOOTLOADER_MBR_MODULE_INDEX))
PREBOOTLOADER_MBR_MODULE_INDEX=1
endif

GLOBAL_DEFINES += PREBOOTLOADER_MBR_VERSION=$(PREBOOTLOADER_MBR_VERSION)
GLOBAL_DEFINES += MODULE_VERSION=$(PREBOOTLOADER_MBR_VERSION)
GLOBAL_DEFINES += MODULE_FUNCTION=$(MODULE_FUNCTION_BOOTLOADER)
GLOBAL_DEFINES += MODULE_DEPENDENCY=$(PREBOOTLOADER_MBR_MODULE_DEPENDENCY)
GLOBAL_DEFINES += MODULE_DEPENDENCY2=$(PREBOOTLOADER_MBR_MODULE_DEPENDENCY2)
GLOBAL_DEFINES += MODULE_INDEX=$(PREBOOTLOADER_MBR_MODULE_INDEX)

# select sources from platform

# import the sources from the platform
include $(call rwildcard,$(PREBOOTLOADER_MBR_MODULE_PATH)/,sources.mk)

endif