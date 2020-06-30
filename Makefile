MKDIR   := mkdir -p
RMDIR   := rm -rf
QEMU	:= qemu-system-i386
QMFLAGS := -S -gdb tcp::1234

BUILD	:= $(CURDIR)/build
BIN     := $(BUILD)/bin
OUT		:= $(BIN)/kernel.out
IMG		:= $(BIN)/kernel.img
ISO		:= $(BUILD)/CyanOS.iso
ROOT	:= $(BUILD)/CyanOS_root

.PHONY: all run debug clean kernel compile

all: $(ISO)


debug: $(ISO)
	$(QEMU) -cdrom $(ISO) $(QMFLAGS)


run: $(ISO)
	$(QEMU) -kernel $(IMG)

clean:
	$(RMDIR) "$(BUILD)"

$(ISO): $(OUT)
	python utils/make_bootable_iso.py $(BIN) $(ROOT) $(ISO)

$(OUT): | $(BIN)
	$(MAKE) OBJ=$(BUILD)/obj/kernel OUT=$(OUT) IMG=$(IMG) -C "./kernel"
	
	
$(BIN):
	$(MKDIR) $@