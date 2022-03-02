# Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# $@ (first?) target
# $< (first?) prerequisite
# $^ all prerequisites


GIT_HEAD = $(shell git rev-parse HEAD)
MACH_TYPE = $(shell uname -m)
BUILD_TIME = $(shell date +%FT%T%z)


CC:=~/CustomInstalledPrograms/riscv-from-sourc/bin/riscv32-unknown-elf-gcc
LD:=~/CustomInstalledPrograms/riscv-from-sourc/bin/riscv32-unknown-elf-ld

SRC_DIR:=kernel
BUILD_DIR:=build
INCLUDES_DIR:=include

TARGET:=$(BUILD_DIR)/kernel.bin
default: $(TARGET)

LINKERSCRIPT:=linker_scripts/esp32-c3/f9.ld

CFLAGS_MISC_DEFINE = \
	-DGIT_HEAD=\"$(GIT_HEAD)\" \
	-DMACH_TYPE=\"$(MACH_TYPE)\" \
	-DBUILD_TIME=\"$(BUILD_TIME)\"

CFLAGS += -march=rv32imc
CFLAGS += -mabi=ilp32
CFLAGS += -Wall -Wundef # -Wstrict-prototypes
CFLAGS += -fno-toplevel-reorder -fno-strict-aliasing
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -fno-omit-frame-pointer -ggdb3
CFLAGS += -O0
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I$(INCLUDES_DIR)
CFLAGS += -I$(INCLUDES_DIR)/l4
CFLAGS += -I$(INCLUDES_DIR)/lib
CFLAGS += -I$(INCLUDES_DIR)/platform
CFLAGS += $(CFLAGS_MISC_DEFINE)

LDFLAGS = --gc-sections
# LDLAGS += -march=rv32imc
# LDLAGS += -mabi=ilp32


SOURCES:=$(shell find $(SRC_DIR) -name "*.c")
ASSEMBLY:=$(shell find $(SRC_DIR) -name "*.S")
$(info $$SOURCES is [${SOURCES}])


# OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:%.c=%.o))
SOURCE_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
ASSEMBLY_OBJECTS = $(ASSEMBLY:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)
$(info $$OBJECTS is [${OBJECTS}])
$(info $$SOURCE_OBJECTS is [${SOURCE_OBJECTS}])
$(info $$ASSEMBLY_OBJECTS is [${ASSEMBLY_OBJECTS}])

.SECONDEXPANSION:

$(SOURCE_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(ASSEMBLY_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.S,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(TARGET): $(SOURCE_OBJECTS) $(ASSEMBLY_OBJECTS) $(LINKERSCRIPT) $(INCLUDES_DIR)
	$(LD) $(LDFLAGS) -T $(LINKERSCRIPT) -o $(TARGET) $(SOURCE_OBJECTS) $(ASSEMBLY_OBJECTS)

.PHONY: clean
clean:
	rm -r build/

.PHONY: qemu
qemu: $(TARGET)
	qemu-system-riscv32 -nographic -smp 1 -s -S -machine virt -bios none -kernel $(TARGET)

.PHONY: qemu-no-dbg
qemu-no-dbg: $(TARGET)
	qemu-system-riscv32 -nographic -smp 1 -machine virt -bios none -kernel $(TARGET)

.PHONY: gdb
dbg: $(TARGET)
	riscv32-unknown-elf-gdb $(TARGET)
