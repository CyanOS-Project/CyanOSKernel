MKDIR   := mkdir -p
RMDIR   := rm -rf
CC      := g++
ASM		:= nasm
LD      := ld
OBCP	:= objcopy
QEMU	:= qemu-system-i386

BIN     := $(BUILD)/bin
OBJ     := $(BUILD)/obj
INCLUDE := .
CPP_SRCS := $(wildcard *.cpp */*.cpp */*/*.cpp)
ASM_SRCS := $(wildcard  */*/*.s)
CPP_OBJS := $(patsubst %.cpp,$(OBJ)/%.o,$(CPP_SRCS))
ASM_OBJS := $(patsubst %.s,$(OBJ)/%.o2,$(ASM_SRCS))
DIRS    := $(sort  $(abspath $(dir $(ASM_OBJS) $(CPP_OBJS)))) 
EXE     := $(BIN)/kernel.out
IMG		:= 	$(BIN)/kernel.img
CFLAGS  := -T linker.ld -I $(INCLUDE) -m32 -g --std=c++17 -fno-builtin -Og -MMD -fno-pie -ffreestanding -nostdlib -fno-rtti -fno-exceptions -fno-leading-underscore
QMFLAGS := -S -gdb tcp::1234
NFLAGS	:= 


.PHONY: all run debug clean

all: $(EXE)

debug: $(EXE)
	$(QEMU) -kernel $(IMG) $(QMFLAGS)

run: $(EXE)
	$(QEMU) -kernel $(IMG)

$(EXE): $(ASM_OBJS) $(CPP_OBJS) | $(BIN)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBCP) -O binary $@ $(IMG)


$(OBJ)/%.o: %.cpp | $(DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o2: %.s | $(DIRS)
	$(ASM) -fwin32 $< -o $@



$(BIN) $(DIRS):
	$(MKDIR) $@

-include $(OBJ)/*.d

clean:
	$(RMDIR) "$(BUILD)"