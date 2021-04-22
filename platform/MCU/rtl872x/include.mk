PLATFORM_DEPS = third_party/ambd_sdk third_party/littlefs third_party/miniz
PLATFORM_DEPS_INCLUDE_SCRIPTS =$(foreach module,$(PLATFORM_DEPS),$(PROJECT_ROOT)/$(module)/import.mk)
include $(PLATFORM_DEPS_INCLUDE_SCRIPTS)

PLATFORM_LIB_DEP += $(AMBD_SDK_LIB_DEP) $(LITTLEFS_LIB_DEP) $(MINIZ_LIB_DEP)
LIBS += $(notdir $(PLATFORM_DEPS))
LIB_DIRS += $(AMBD_SDK_LIB_DIR) $(LITTLEFS_LIB_DIR) $(MINIZ_LIB_DIR)
