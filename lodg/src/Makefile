CC=gcc

# For cross-compiling to mipsel
export STAGING_DIR=/home/builder/omega/source/staging_dir
export LD_LIBRARY_PATH=$(STAGING_DIR)/target-mipsel_24kc_musl/usr/lib
TOOLCHAIN_BINDIR=$(STAGING_DIR)/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin
INCLUDES=
INCLUDES_CROSS=-I$(STAGING_DIR)/target-mipsel_24kc_musl/usr/include

EXTERNAL_LIBS=-lpthread -lrt

ifneq (,$(findstring ENABLE_MQTT,$(ENABLED_FEATURES)))
    EXTERNAL_LIBS:=$(EXTERNAL_LIBS) -lmosquitto
endif

CSTD=c99

OBJS=		$(patsubst %.c,%.o,$(shell find . -name "*.c"))
BLDDIROBJS= $(patsubst %.c,../build/%.o,$(shell find . -name "*.c" -exec basename {} \;))

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

build_release_cross: CFLAGS = -L$(LD_LIBRARY_PATH) $(INCLUDES_CROSS) $(INCLUDES) $(ENABLED_FEATURES) -O2 -fdata-sections -ffunction-sections -flto -std=$(CSTD) -Wall --pedantic
build_release_cross: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)
build_release_cross: CC = $(TOOLCHAIN_BINDIR)/mipsel-openwrt-linux-musl-gcc
build_release_cross: LD = $(TOOLCHAIN_BINDIR)/mipsel-openwrt-linux-musl-ld

build_release: CFLAGS = $(INCLUDES) $(ENABLED_FEATURES) -O2 -fdata-sections -ffunction-sections -flto -std=$(CSTD) -Wall --pedantic
build_release: LDFLAGS = -Wl,--gc-sections,-flto $(EXTERNAL_LIBS)

build_debug: CFLAGS = $(INCLUDES) $(ENABLED_FEATURES) -g -std=$(CSTD) -Wall --pedantic
build_debug: LDFLAGS = -Wl,-g $(EXTERNAL_LIBS)


build_release_cross: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv $(shell find . -name "*.o") $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)
	$(TOOLCHAIN_BINDIR)/mipsel-openwrt-linux-musl-strip $(BLDDIR)/lodg.elf

build_release: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv $(shell find . -name "*.o") $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)
	strip $(BLDDIR)/lodg.elf

build_debug: $(OBJS)
	@printf "\n++++++++++++\tBUILDING DEBUG\t++++++++++++\n\n"
	mv $(shell find . -name "*.o") $(BLDDIR)
	$(CC) $(CFLAGS) $(ENABLED_FEATURES) $(BLDDIROBJS) -o $(BLDDIR)/lodg.elf $(LDFLAGS)
	cp ../testdata/* $(BLDDIR)

.PHONY: clean

cleanobjs:
	@printf "\n++++++++++++\tCLEANING UP OBJECTS\t++++++++++++\n\n"
	find ../ -name "*.o" -delete

clean_all:
	@printf "\n++++++++++++\tCLEANING UP EVERYTING ELSE\t++++++++++++\n\n"
	find ../ -name "*.elf" -delete