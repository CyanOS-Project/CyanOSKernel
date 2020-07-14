COLON	:= ::
MKDIR   := mkdir -p
RMDIR   := rm -rf
QEMU	:= qemu-system-i386
QMFLAGS := -no-reboot -no-shutdown -m 128 -d cpu_reset -boot d -cdrom
QMDEBUG := -S -gdb tcp$(COLON)5555


BUILD	:= $(CURDIR)/build
BIN		:= $(BUILD)/bin
BIN_KRNL:= $(BIN)/kernel
BIN_APPS:= $(BIN)/apps
DRV_BIN := $(BIN_APPS)/Drivers
USR_BIN := $(BIN_APPS)/UserBinary
OUT		:= $(BIN_KRNL)/kernel.out
IMG		:= $(BIN_KRNL)/kernel.img
ISO		:= $(BUILD)/CyanOS.iso
ROOT	:= $(BUILD)/CyanOS_root
KRL_DIR := kernel/


.PHONY: all run debug clean force

all: $(ISO)


debug: $(ISO)
	$(QEMU) $(QMFLAGS) $(ISO) $(QMDEBUG)


run: $(ISO)
	$(QEMU) $(QMFLAGS) $(ISO)

clean:
	$(RMDIR) "$(BUILD)"



$(ISO): $(IMG) $(BIN_APPS)
	python utils/make_bootable_iso.py $(BIN_KRNL) $(BIN_APPS) $(ROOT) $(ISO)

$(IMG): force | $(BIN_KRNL)
	$(MAKE) OBJ=$(BUILD)/obj/kernel OUT=$(OUT) IMG=$(IMG) -C $(KRL_DIR)	

$(BIN_APPS): | $(DRV_BIN) $(USR_BIN)
	echo "hello file 1" > $(DRV_BIN)/"file1.txt"
	echo "hello file 2" > $(DRV_BIN)/"file2.txt"
	echo "hello file 3" > $(USR_BIN)/"file3.txt"

$(BIN_KRNL) $(DRV_BIN) $(USR_BIN):
	$(MKDIR) $@

force:
