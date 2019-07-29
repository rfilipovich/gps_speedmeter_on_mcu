#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := gps_speedmeter_on_mcu

COMPONENT_ADD_INCLUDEDIRS := components/include

EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/src
EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/libs/u8g2
EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/libs/minmea

include $(IDF_PATH)/make/project.mk
