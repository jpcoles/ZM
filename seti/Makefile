CC=gcc
CFLAGS=-Wall -O3 -fnested-functions
LDFLAGS=-lsndfile

signal2wave: signal2wave.o
	$(CC) $(LDFLAGS) signal2wav.o -o signal2wav

signal2wave.o: signal2wav.c
	$(CC) $(CFLAGS) -c signal2wav.c
