TESTDIR=./build/test
SUBDIRS=src

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

clean:
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	cd src && make clean