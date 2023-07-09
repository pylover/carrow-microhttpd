CC=gcc
CFLAGS=-Wall -Werror -g

all: build

build:
	$(CC) $(CFLAGS) carrow_microhttpd.c -o carrow_microhttpd -lmicrohttpd \
		-lcarrow -lclog

run:
	./carrow_microhttpd

clean:
	rm ./carrow_microhttpd
