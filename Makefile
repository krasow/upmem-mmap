### Modify here ###
PROJECT=/home/david/upmem-mmap
TASKLETS=16
DPUS=32
SIM=1
### Leave alone ###

DEFINES=-DNR_TASKLETS=$(TASKLETS) -DNR_DPUS=$(DPUS) -DSIMULATOR=$(SIM)

### setup CPU query binaries ###
CC=g++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -Wno-deprecated 
LDFLAGS=-lm
DPU_PKG=`dpu-pkg-config --cflags --libs dpu`

BUILD_DIR=$(PROJECT)/build

.phony: all 
all: $(BUILD_DIR)/main $(BUILD_DIR)/dpu_binary

$(BUILD_DIR)/main: main.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@ $(DPU_PKG) $(LDFLAGS)
### end of CPU binaries ###


### setup DPU binaries ####
CC_DPU=dpu-upmem-dpurte-clang
CFLAGS_DPU=-O2 -Wall $(DEFINES)
LDFLAGS_DPU=
DPU_COMPILE_COMMAND=$(CC_DPU) $(CFLAGS_DPU) $< -o $@ $(LDFLAGS_DPU)

$(BUILD_DIR)/dpu_binary: dpu_binary.c 
	$(DPU_COMPILE_COMMAND)
### end DPU binaries ####



$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
