MKDIR   := mkdir
RMDIR   := rmdir /S /Q
CC      := g++
LD      := ld
OBCP	:= objcopy
QEMU	:= qemu-system-i386

BIN     := $(BUILD)/bin
OBJ     := $(BUILD)/obj
INCLUDE := ./include
SRC     := ./src
SRCS    := $(wildcard *.cpp)
OBJS    := $(patsubst %.cpp,$(OBJ)/%.o,$(SRCS))
EXE     := $(BIN)/kernel.out
IMG		:= 	$(BIN)/kernel.img
CFLAGS  := -I$(INCLUDE) -m32 -g --std=c++17 -fno-builtin -Og -MMD
LDFLAGS := -T linker.ld
QMFLAGS := -S -gdb tcp::1234






.PHONY: all run debug clean

all: $(EXE)


debug: $(EXE)
	$(QEMU) -kernel $(IMG) $(QMFLAGS)

run: $(EXE)
	$(QEMU) -kernel $(IMG)

$(EXE): $(OBJS) | $(BIN)
	$(LD) $(LDFLAGS) $^ -o $@
	$(OBCP) -O binary $@ $(IMG)

-include $(OBJ)/*.d

$(OBJ)/%.o: %.cpp | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) "$@"


clean:
	$(RMDIR) "$(BUILD)"