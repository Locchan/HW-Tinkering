BLDDIR=./build
SUBDIRS=src

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

clean:
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	rm -f $(BLDDIR)/*
	cd src && make clean