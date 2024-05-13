CC=gcc

# For cross-compiling to mipsel
export STAGING_DIR=/home/builder/omega/source/staging_dir

EXTERNAL_LIBS=-lpthread -lrt -lcriterion -lmosquitto

OBJS=		config.o lodg.o util.o gather.o metstor.o gath_unix.o mqtt.o main.o 
OBJSNOMAIN=	config.o lodg.o util.o gather.o metstor.o gath_unix.o mqtt.o tests.o

TESTOBJS=test/tests.o

BLDDIR=$(PWD)/build
TESTDIR=$(BLDDIR)/test
ORIGDIR=$(PWD)

BLDDIROBJS=$(patsubst %,$(BLDDIR)/%, $(OBJS))

BLDDIROBJSNOMAIN=$(patsubst %,$(BLDDIR)/%, $(OBJSNOMAIN))

release-cross: prepare build_release_cross cleanobjs
release: prepare build_release build_test run_tests cleanobjs
debug: prepare build_debug build_test cleanobjs
clean: cleanobjs clean_all

prepare:
	@printf "\n++++++++++++\tPREPARATION\t++++++++++++\n\n"
	@printf "Objects to build: ${BLDDIROBJS}"
	@printf "\n\n++++++++++++\tCOMPILATION\t++++++++++++\n\n"

build_release_cross: CFLAGS = $(ENABLED_FEATURES) -D_GNU_SOURCE -O2 -fdata-sections -ffunction-sections -flto -std=c99 -Wall --pedantic
build_release_cross: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)
build_release_cross: CC = $(STAGING_DIR)/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin/mipsel-openwrt-linux-musl-gcc
build_release_cross: LD = $(STAGING_DIR)/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin/mipsel-openwrt-linux-musl-ld

build_release: CFLAGS = $(ENABLED_FEATURES) -O2 -fdata-sections -ffunction-sections -flto -std=c99 -Wall --pedantic
build_release: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)

build_debug: CFLAGS = $(ENABLED_FEATURES) -g -std=c99 -Wall --pedantic
build_debug: LDFLAGS = -Wl,-g $(EXTERNAL_LIBS)

build_test: CFLAGS = $(ENABLED_FEATURES) -g -std=c99 -Wall --pedantic
build_test: LDFLAGS = -Wl,-g $(EXTERNAL_LIBS)


build_release_cross: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)

build_release: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)

build_debug: $(OBJS)
	@printf "\n++++++++++++\tBUILDING DEBUG\t++++++++++++\n\n"
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)

build_test: $(TESTOBJS)
	@printf "\n++++++++++++\tBUILDING TESTS\t++++++++++++\n\n"
	mv test/*.o $(BLDDIR)
	$(CC) $(CFLAGS) $(BLDDIROBJSNOMAIN) -o $(TESTDIR)/tests.elf $(LDFLAGS)
	cp test/testcfg.cfg $(TESTDIR)/

run_tests:
	@printf "\n++++++++++++\tRUNNING TESTS\t++++++++++++\n\n"
	$(TESTDIR)/tests.elf --verbose

.PHONY: clean

cleanobjs:
	@printf "\n++++++++++++\tCLEANING UP OBJECTS\t++++++++++++\n\n"
	rm -f ./*.o ./*.bin ./*.so
	rm -f $(BLDDIR)/*.o $(BLDDIR)/*.bin $(BLDDIR)/*.so
	rm -f $(TESTDIR)/*.o $(TESTDIR)/*.bin $(TESTDIR)/*.so

clean_all:
	@printf "\n++++++++++++\tCLEANING UP EVERYTING ELSE\t++++++++++++\n\n"
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	rm -f $(BLDDIR)/*.o $(BLDDIR)/*.bin $(BLDDIR)/*.so $(BLDDIR)/*.elf
	rm -f $(TESTDIR)/*.o $(TESTDIR)/*.bin $(TESTDIR)/*.so $(TESTDIR)/*.elf