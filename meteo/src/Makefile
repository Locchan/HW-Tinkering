CC=gcc

OBJS=config.o meteo.o util.o gather.o metstor.o

BLDDIR=$(PWD)/build
ORIGDIR=$(PWD)

BLDDIROBJS=$(patsubst %,$(BLDDIR)/%, $(OBJS))

release: prepare build_release
debug: prepare build_debug

prepare:
	$(info Objects to build: [${BLDDIROBJS}])

build_release: CFLAGS = -O2 -fdata-sections -ffunction-sections -flto -std=c99 -Wall --pedantic
build_release: LDFLAGS = -Wl,--gc-sections,-flto -lpthread -lrt

build_debug: CFLAGS = -g -std=c99 -Wall --pedantic
build_debug: LDFLAGS = -Wl,-g -lpthread -lrt

build_release: $(OBJS) 
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(BLDDIROBJS) -o $(BLDDIR)/meteo.elf $(LDFLAGS)
	rm -f $(BLDDIR)/*.o

build_debug: $(OBJS)
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(BLDDIROBJS) -o $(BLDDIR)/meteo.elf $(LDFLAGS)
	rm -f $(BLDDIR)/*.o


.PHONY: clean

clean:
	rm -rf ./*.o ./*.bin ./*.so ./*.elf