MKDIR   := mkdir -p
RMDIR   := rm -rf
CC      := g++
ASM		:= nasm
LD      := ld
OBCP	:= objcopy

INCLUDE := .
CPP_SRCS := $(wildcard *.cpp */*.cpp */*/*.cpp)
ASM_SRCS := $(wildcard  */*/*.s)
CPP_OBJS := $(patsubst %.cpp,$(OBJ)/%.o,$(CPP_SRCS))
ASM_OBJS := $(patsubst %.s,$(OBJ)/%.o2,$(ASM_SRCS))
DIRS     := $(sort  $(abspath $(dir $(ASM_OBJS) $(CPP_OBJS)))) 

CFLAGS  := -I $(INCLUDE) -m32 -g -Wall --std=c++17 -Og -MMD -fno-pie -ffreestanding -fno-rtti -fno-exceptions -fno-leading-underscore
LDFLAGS := -T linker.ld -m32 -nostdlib

.PHONY: all

all: $(OUT)

$(OUT): $(ASM_OBJS) $(CPP_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(OBCP) -O binary $@ $(IMG)


$(OBJ)/%.o: %.cpp | $(DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o2: %.s | $(DIRS)
	$(ASM) -fwin32 $< -o $@

$(DIRS):
	$(MKDIR) $@

-include $(OBJ)/*.d

