TESTDIR=./build/test
SUBDIRS=src

export ENABLED_FEATURES="-D ENABLE_UNIX_GATHERERS"

release:
	mkdir -p $(TESTDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir release; \
	done

debug:
	mkdir -p $(TESTDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir debug; \
	done

release-cross:
	mkdir -p $(TESTDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir release-cross; \
	done

clean:
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	cd src && make clean