#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)..
# Components name restrictions https://github.com/espressif/esp-idf/issues/2087 
#

DIRECTORIES = $(sort $(dir $(wildcard ../src/*/ )))


APP_SOURCES = $(DIRECTORIES) ./

COMPONENT_ADD_INCLUDEDIRS  += $(APP_SOURCES)
COMPONENT_SRCDIRS          += $(APP_SOURCES)

#COMPONENT_EMBED_FILES 	   := ../Resources/data.zip
#CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_VERBOSE

CFLAGS += -g -O0
