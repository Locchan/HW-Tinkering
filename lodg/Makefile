BLDDIR=./build
SUBDIRS=src

export ENABLED_FEATURES=-DENABLE_UNIX_GATHERERS -DENABLE_MQTT

release:
	mkdir -p $(BLDDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir release; \
	done

debug:
	mkdir -p $(BLDDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir debug; \
	done

release-cross:
	mkdir -p $(BLDDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir release-cross; \
	done

clean:
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	cd src && make clean