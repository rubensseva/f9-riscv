# Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

CC:=riscv32-unknown-elf-gcc
LD:=riscv32-unknown-elf-ld

GIT_HEAD = $(shell git rev-parse HEAD)
MACH_TYPE = $(shell uname -m)
BUILD_TIME = $(shell date +%FT%T%z)
CFLAGS_MISC_DEFINE = \
	-DGIT_HEAD=\"$(GIT_HEAD)\" \
	-DMACH_TYPE=\"$(MACH_TYPE)\" \
	-DBUILD_TIME=\"$(BUILD_TIME)\"
CFLAGS_HOPPUS_DEFINE = -DHOPPUS_RISCV_F9

BUILD_DIR:=build
TARGET:=$(BUILD_DIR)/kernel.bin
default: $(TARGET)

KERNEL_DIR:=kernel
PLATFORM_DIR:=platform
INCL_DIRS:= include $(shell find include/* -type d) user/include user/hoppus/include

LINKER_SCRIPT:=platform/esp32_c3/f9.ld

CFLAGS += -march=rv32imc -mabi=ilp32
CFLAGS += -ggdb3 -O0
CFLAGS += -Wall -Wundef -Werror-implicit-function-declaration
CFLAGS += -fno-toplevel-reorder -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -mcmodel=medany
CFLAGS += -fno-jump-tables # Need this to make switch statement not cause access faults
CFLAGS += $(foreach d, $(INCL_DIRS), -I$d)
CFLAGS += $(CFLAGS_MISC_DEFINE)
CFLAGS += $(CFLAGS_HOPPUS_DEFINE)

LDFLAGS = --gc-sections

HEADERS := $(shell find * -name "*.h")
C_SOURCES := $(shell find * -name "*.c")
ASSEMBLY_SOURCES := $(shell find * -name "*.S")
ALL_SOURCES = $(C_SOURCES) $(ASSEMBLY_SOURCES)

C_OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
ASSEMBLY_OBJECTS = $(addprefix $(BUILD_DIR)/, $(ASSEMBLY_SOURCES:.S=.o))
ALL_OBJECTS = $(C_OBJECTS) $(ASSEMBLY_OBJECTS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ALL_OBJECTS) $(LINKER_SCRIPT) $(INCL_DIRS) $(HEADERS)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(TARGET) $(ALL_OBJECTS)

# Only create directories when required
$(BUILD_DIR)%/:
	mkdir -p $@
$(foreach OBJECT,$(ALL_OBJECTS),$(eval $(OBJECT): | $(dir $(OBJECT))))

.PHONY: clean
clean:
	rm -r build/
