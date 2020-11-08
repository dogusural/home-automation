
BINARY_FOLDER := build
LIB_DIR := ./lib
SERVER_SOURCE_DIR := server
OBJECT_FOLDER := $(BINARY_FOLDER)/objects
LIB_PIGPIO := $(LIB_DIR)/libpigpio.so

SERVER_APP := $(BINARY_FOLDER)/automation_server


SCP := /usr/bin/scp
RPI_USER := pi
RPI_IP := 192.168.0.74
RPI_PATH := /home/pi/

TOOLCHAIN_FOLDER := /home/dodo/Desktop/workspace/tools/cross-gcc-8.3.0-pi_3+/cross-pi-gcc-8.3.0-2
CC := $(TOOLCHAIN_FOLDER)/bin/arm-linux-gnueabihf-g++
AR := $(TOOLCHAIN_FOLDER)/bin/arm-linux-gnueabihf-ar
SYSROOT := $(TOOLCHAIN_FOLDER)/arm-linux-gnueabihf/libc/


LIBS = -lpthread -lpigpio -ldl -lrt

DEFINES =

INCLUDE = \
-I./inc/ \
-I$(LIB_DIR)



CFLAGS = -O0 --sysroot=$(SYSROOT) -mfloat-abi=hard -march=armv8-a+crc -mfpu=neon-fp-armv8 -mtune=cortex-a53 -pedantic


FLAGS = $(INCLUDE) -L${LIB_DIR} $(LIBS) $(DEFINES)
SHARED_LIB_FLAGS = -shared
STATIC_LIB_FLAGS = rcs



$(OBJECT_FOLDER)/%.o: $(SERVER_SOURCE_DIR)/%.cpp
	@mkdir -p $(OBJECT_FOLDER)
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@



SERVER_SOURCE_CPP :=  $(shell find $(SERVER_SOURCE_DIR) -name '*.cpp')

SERVER_SOURCE_OBJECTS :=  $(addprefix $(OBJECT_FOLDER)/,$(notdir $(SERVER_SOURCE_CPP:.cpp=.o)))



$(SERVER_APP): $(SERVER_SOURCE_OBJECTS) $(LIB_PIGPIO)
	@mkdir -p $(BINARY_FOLDER)
	$(CC) $(FLAGS) $< -o $@



.PHONY: debug clean server install
 
all: $(SERVER_APP)



debug:
	@echo  $(SERVER_SOURCE_CPP)"\n\n"$(SERVER_SOURCE_OBJECTS)"\n\n"

install:
	@$(SCP) $(SERVER_APP) $(RPI_USER)@$(RPI_IP):$(RPI_PATH)

clean:
	rm -rf $(BINARY_FOLDER)
deep-clean: clean
	@echo "Not implemented Yet!"
