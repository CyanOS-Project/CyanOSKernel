COLON	:= ::
MKDIR   := mkdir -p
RMDIR   := rm -rf
QEMU	:= qemu-system-i386
QMFLAGS := -no-reboot -no-shutdown -m 128 -d cpu_reset -boot d -cdrom
QMDEBUG := -S -gdb tcp$(COLON)1234


BUILD	:= $(CURDIR)/build
BIN     := $(BUILD)/bin
OUT		:= $(BIN)/kernel.out
IMG		:= $(BIN)/kernel.img
ISO		:= $(BUILD)/CyanOS.iso
ROOT	:= $(BUILD)/CyanOS_root
KRL_SRC := ./kernel

.PHONY: all run debug clean kernel compile

all: compile


debug: compile
	$(QEMU) $(QMFLAGS) $(ISO) $(QMDEBUG)


run: compile
	$(QEMU) $(QMFLAGS) $(ISO)

clean:
	$(RMDIR) "$(BUILD)"

compile: $(ISO)

$(ISO): $(IMG)
	python utils/make_bootable_iso.py $(BIN) $(ROOT) $(ISO)

$(IMG): $(KRL_SRC) | $(BIN)
	$(MAKE) OBJ=$(BUILD)/obj/kernel OUT=$(OUT) IMG=$(IMG) -C $(KRL_SRC)	
	
$(BIN):
	$(MKDIR) $@