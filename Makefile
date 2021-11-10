# Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# $@ (first?) target
# $< (first?) prerequisite
# $^ all prerequisites

CC:=riscv64-unknown-elf-gcc
LD:=riscv64-unknown-elf-ld

SRC_DIR:=kernel
BUILD_DIR:=build
INCLUDES_DIR:=include

TARGET:=$(BUILD_DIR)/kernel.bin
default: $(TARGET)

LINKERSCRIPT:=f9.ld
# CFLAGS:=-g -ffreestanding -O0 -Wl,--gc-sections \
#     -nostartfiles -nostdlib -nodefaultlibs -Wl,-T,riscv64-virt.ld

CFLAGS = -Wall -O -fno-omit-frame-pointer -ggdb
# CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I$(INCLUDES_DIR)
CFLAGS += -I$(INCLUDES_DIR)/l4
CFLAGS += -I$(INCLUDES_DIR)/lib
CFLAGS += -I$(INCLUDES_DIR)/platform


SOURCES:=$(shell find $(SRC_DIR) -name '*.c')
$(info $$SOURCES is [${SOURCES}])


# OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:%.c=%.o))
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
$(info $$OBJECTS is [${OBJECTS}])

.SECONDEXPANSION:

$(OBJECTS) : $$(patsubst $(BUILD_DIR)/%.o,$(SRC_DIR)/%.c,$$@)
	mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -T $(LINKERSCRIPT) -o $(TARGET) $(OBJECTS)
