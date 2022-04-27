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

KERNEL_DIR:=kernel
PLATFORM_DIR:=platform
INCLUDES_DIR:=include
USER_DIR:=user
USER_INCLUDES_DIR:=user/include
USER_CLISP_DIR:=user/clisp/src
USER_CLISP_INCLUDE_DIR:=user/clisp/include

BUILD_DIR:=build

TARGET:=$(BUILD_DIR)/kernel.bin
default: $(TARGET)

LINKERSCRIPT:=platform/esp32_c3/f9.ld

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
CFLAGS += -fno-jump-tables # Need this to make switch statement not cause access faults
CFLAGS += -I$(INCLUDES_DIR)
CFLAGS += -I$(INCLUDES_DIR)/kernel
CFLAGS += -I$(INCLUDES_DIR)/l4
CFLAGS += -I$(INCLUDES_DIR)/lib
CFLAGS += -I$(INCLUDES_DIR)/platform
CFLAGS += -I$(USER_INCLUDES_DIR)
CFLAGS += -I$(USER_CLISP_INCLUDE_DIR)
CFLAGS += $(CFLAGS_MISC_DEFINE)
$(info $$CLISP_INCLUDE is [${USER_CLISP_INCLUDE_DIR}])
$(info $$CFLAGS is [${CFLAGS}])

LDFLAGS = --gc-sections
# LDLAGS += -march=rv32imc
# LDLAGS += -mabi=ilp32


KERNEL_SOURCES:=$(shell find $(KERNEL_DIR) -name "*.c")
ASSEMBLY:=$(shell find $(KERNEL_DIR) -name "*.S")
PLATFORM_SOURCES:=$(shell find $(PLATFORM_DIR) -name "*.c")
USER_SOURCES:=$(shell find $(USER_DIR) -name "*.c")
USER_CLISP_SOURCES:=$(shell find $(USER_CLISP_DIR) -name "*.c")
$(info $$ASSEMBLY is [${ASSEMBLY}])


# OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:%.c=%.o))
KERNEL_SOURCE_OBJECTS = $(KERNEL_SOURCES:$(KERNEL_DIR)/%.c=$(BUILD_DIR)/%.o)
ASSEMBLY_OBJECTS = $(ASSEMBLY:$(KERNEL_DIR)/%.S=$(BUILD_DIR)/%.o)
PLATFORM_SOURCE_OBJECTS = $(PLATFORM_SOURCES:$(PLATFORM_DIR)/%.c=$(BUILD_DIR)/%.o)
USER_SOURCE_OBJECTS = $(USER_SOURCES:$(USER_DIR)/%.c=$(BUILD_DIR)/%.o)
USER_CLISP_SOURCE_OBJECTS = $(USER_CLISP_SOURCES:$(USER_CLISP_DIR)/%.c=$(BUILD_DIR)/%.o)
$(info $$KERNEL_SOURCE_OBJECTS is [${KERNEL_SOURCE_OBJECTS}])
$(info $$ASSEMBLY_OBJECTS is [${ASSEMBLY_OBJECTS}])
$(info $$USER_SOURCE_OBJECTS is [${USER_SOURCE_OBJECTS}])
$(info $$USER_CLISP_SOURCE_OBJECTS is [${USER_CLISP_SOURCE_OBJECTS}])
$(info $$PLATFORM_SOURCE_OBJECTS is [${PLATFORM_SOURCE_OBJECTS}])

.SECONDEXPANSION:

$(KERNEL_SOURCE_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(KERNEL_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<
$(ASSEMBLY_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(KERNEL_DIR)/%.S,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<
$(PLATFORM_SOURCE_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(PLATFORM_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<
$(USER_SOURCE_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(USER_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<
$(USER_CLISP_SOURCE_OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(USER_CLISP_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(TARGET): $(KERNEL_SOURCE_OBJECTS) $(ASSEMBLY_OBJECTS) $(PLATFORM_SOURCE_OBJECTS) $(USER_SOURCE_OBJECTS) $(USER_CLISP_SOURCE_OBJECTS) $(LINKERSCRIPT) $(INCLUDES_DIR) $(USER_INCLUDES_DIR) $(USER_CLISP_INCLUDE_DIR)
	$(LD) $(LDFLAGS) -T $(LINKERSCRIPT) -o $(TARGET) $(KERNEL_SOURCE_OBJECTS) $(ASSEMBLY_OBJECTS) $(PLATFORM_SOURCE_OBJECTS) $(USER_SOURCE_OBJECTS)

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
