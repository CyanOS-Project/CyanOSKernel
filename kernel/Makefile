MKDIR   := mkdir -p
RMDIR   := rm -rf
CC      := g++
LD      := ld
OBCP	:= objcopy
QEMU	:= qemu-system-i386

BIN     := $(BUILD)/bin
OBJ     := $(BUILD)/obj
INCLUDE := .
SRCS    := $(wildcard *.cpp */*.cpp */*/*.cpp)
OBJS    := $(patsubst %.cpp,$(OBJ)/%.o,$(SRCS))
DIRS    := $(sort  $(abspath $(dir $(OBJS)))) 
EXE     := $(BIN)/kernel.out
IMG		:= 	$(BIN)/kernel.img
CFLAGS  := -T linker.ld -I $(INCLUDE) -m32 -g --std=c++17 -fno-builtin -Og -MMD -fno-pie -ffreestanding -nostdlib
QMFLAGS := -S -gdb tcp::1234


.PHONY: all run debug clean

all: $(EXE)

debug: $(EXE)
	$(QEMU) -kernel $(IMG) $(QMFLAGS)

run: $(EXE)
	$(QEMU) -kernel $(IMG)

$(EXE): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBCP) -O binary $@ $(IMG)


$(OBJ)/%.o: %.cpp | $(DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(DIRS):
	$(MKDIR) $@

-include $(OBJ)/*.d

clean:
	$(RMDIR) "$(BUILD)"