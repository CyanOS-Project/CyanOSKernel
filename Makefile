
.PHONY: all run debug clean

all:
	$(MAKE) BUILD=$(CURDIR)/build -C "./kernel"
	
debug:
	$(MAKE) debug  BUILD=$(CURDIR)/build -C "./kernel"
	
run:
	$(MAKE) run  BUILD=$(CURDIR)/build -C "./kernel"

clean:
	$(MAKE) clean BUILD=$(CURDIR)/build -C "./kernel"

