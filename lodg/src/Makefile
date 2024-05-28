CC=gcc

# For cross-compiling to mipsel
export STAGING_DIR=/home/builder/omega/source/staging_dir

EXTERNAL_LIBS=-lpthread -lrt

ifneq (,$(findstring ENABLE_MQTT,$(ENABLED_FEATURES)))
    EXTERNAL_LIBS:=$(EXTERNAL_LIBS) -lmosquitto
endif

CSTD=c11

OBJS=		$(patsubst %.c,%.o,$(shell find . -name '*.c'))
BLDDIROBJS= $(patsubst %.c,../build/%.o,$(shell find . -name '*.c' -exec basename {} \;))

BLDDIR=$(PWD)/build
ORIGDIR=$(PWD)

release-cross: prepare build_release_cross cleanobjs
release: prepare build_release cleanobjs
debug: prepare build_debug cleanobjs
clean: cleanobjs clean_all

prepare:
	@printf "\n++++++++++++\tPREPARATION\t++++++++++++\n\n"
	@printf "Objects to build: ${BLDDIROBJS}"
	@printf "\n\n++++++++++++\tCOMPILATION\t++++++++++++\n\n"

build_release_cross: CFLAGS = $(ENABLED_FEATURES) -D_GNU_SOURCE -O2 -fdata-sections -ffunction-sections -flto -std=$(CSTD) -Wall --pedantic
build_release_cross: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)
build_release_cross: CC = $(STAGING_DIR)/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin/mipsel-openwrt-linux-musl-gcc
build_release_cross: LD = $(STAGING_DIR)/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin/mipsel-openwrt-linux-musl-ld

build_release: CFLAGS = $(ENABLED_FEATURES) -O2 -fdata-sections -ffunction-sections -flto -std=$(CSTD) -Wall --pedantic
build_release: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)

build_debug: CFLAGS = $(ENABLED_FEATURES) -g -std=$(CSTD) -Wall --pedantic
build_debug: LDFLAGS = -Wl,-g $(EXTERNAL_LIBS)


build_release_cross: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv $(shell find . -name '*.o') $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)
	strip $(BLDDIR)/lodg.elf

build_release: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv $(shell find . -name '*.o') $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)
	strip $(BLDDIR)/lodg.elf

build_debug: $(OBJS)
	@printf "\n++++++++++++\tBUILDING DEBUG\t++++++++++++\n\n"
	mv $(shell find . -name '*.o') $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)

.PHONY: clean

cleanobjs:
	@printf "\n++++++++++++\tCLEANING UP OBJECTS\t++++++++++++\n\n"
	rm -f ./*.o ./*.bin ./*.so
	rm -f $(BLDDIR)/*.o $(BLDDIR)/*.bin $(BLDDIR)/*.so

clean_all:
	@printf "\n++++++++++++\tCLEANING UP EVERYTING ELSE\t++++++++++++\n\n"
	rm -f ./*.o ./*.bin ./*.so ./*.elf
	rm -f $(BLDDIR)/*.o $(BLDDIR)/*.bin $(BLDDIR)/*.so $(BLDDIR)/*.elf