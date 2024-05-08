CC=gcc

OBJS=config.o meteo.o util.o gather.o metstor.o main.o
OBJSNOMAIN=config.o meteo.o util.o gather.o metstor.o tests.o

TESTOBJS=test/tests.o

BLDDIR=$(PWD)/build
TESTDIR=$(BLDDIR)/test
ORIGDIR=$(PWD)

BLDDIROBJS=$(patsubst %,$(BLDDIR)/%, $(OBJS))
BLDDIROBJSNOMAIN=$(patsubst %,$(BLDDIR)/%, $(OBJSNOMAIN))

release: prepare build_release build_test run_tests cleanobjs
debug: prepare build_debug build_test cleanobjs
clean: cleanobjs clean_all

prepare:
	@printf "\n++++++++++++\tPREPARATION\t++++++++++++\n\n"
	@printf "Objects to build: ${BLDDIROBJS}"
	@printf "\n\n++++++++++++\tCOMPILATION\t++++++++++++\n\n"

build_release: CFLAGS = -O2 -fdata-sections -ffunction-sections -flto -std=c99 -Wall --pedantic
build_release: LDFLAGS = -Wl,--gc-sections,-flto -lpthread -lrt

build_debug: CFLAGS = -g -std=c99 -Wall --pedantic
build_debug: LDFLAGS = -Wl,-g -lpthread -lrt

build_test: CFLAGS = -g -std=c99 -Wall --pedantic
build_test: LDFLAGS = -Wl,-g -lpthread -lrt -lcriterion

build_release: $(OBJS)
	@printf "\n++++++++++++\tBUILDING RELEASE\t++++++++++++\n\n"
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(BLDDIROBJS) -o $(BLDDIR)/meteo.elf $(LDFLAGS)

build_debug: $(OBJS)
	@printf "\n++++++++++++\tBUILDING DEBUG\t++++++++++++\n\n"
	mv *.o $(BLDDIR)
	$(CC) $(CFLAGS) $(BLDDIROBJS) -o $(BLDDIR)/meteo.elf $(LDFLAGS)

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