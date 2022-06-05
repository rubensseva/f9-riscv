# Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Inspiration for auto dependency management taken from http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

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

DEP_DIR:=$(BUILD_DIR)/.deps
DEP_FLAGS=-MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

KERNEL_DIR:=kernel
PLATFORM_DIR:=platform
INCL_DIRS:= include $(shell find include/* -type d) user/include user/hoppus/include

LINKER_SCRIPT:=platform/esp32_c3/f9.ld

CFLAGS += -march=rv32imc -mabi=ilp32
# CFLAGS += -ggdb3 -O0
CFLAGS += -Og -gdwarf-2
CFLAGS += -Wall -Wundef -Werror-implicit-function-declaration
CFLAGS += -fno-toplevel-reorder -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -mcmodel=medany
CFLAGS += -fno-jump-tables # Need this to make switch statement not cause access faults

# Tell gcc to not touch registers used for MRs
CFLAGS += -ffixed-s4
CFLAGS += -ffixed-s5
CFLAGS += -ffixed-s6
CFLAGS += -ffixed-s7
CFLAGS += -ffixed-s8
CFLAGS += -ffixed-s9
CFLAGS += -ffixed-s10
CFLAGS += -ffixed-s11

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

$(BUILD_DIR)/%.o: %.c $(DEP_DIR)/%.d | $(DEP_DIR)
	$(CC) $(DEP_FLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ALL_OBJECTS) $(LINKER_SCRIPT) $(INCL_DIRS) $(HEADERS)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(TARGET) $(ALL_OBJECTS)

.PHONY: clean
clean:
	rm -r build/

DEP_FILES := $(C_SOURCES:%.c=$(DEP_DIR)/%.d)
$(DEP_DIR): ; @mkdir -p $@

# Only create directories when required
$(BUILD_DIR)%/:
	mkdir -p $@
$(foreach OBJECT,$(ALL_OBJECTS),$(eval $(OBJECT): | $(dir $(OBJECT))))
$(DEP_DIR)%/:
	mkdir -p $@
$(foreach DEP,$(DEP_FILES),$(eval $(DEP): | $(dir $(DEP))))

$(DEP_FILES):
include $(wildcard $(DEP_FILES))
