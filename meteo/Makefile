CC=gcc
CFLAGS=-I. -O2 -fdata-sections -ffunction-sections -flto -std=c99 -Wall --pedantic
LDFLAGS=-Wl,--gc-sections,-flto

meteo: config.o meteo.o
	$(CC) $(CFLAGS) config.o meteo.o -o meteo.bin $(LDFLAGS)

.PHONY: clean

clean:
	rm -f ./*.o ./*.bin