CC=gcc
CFLAGS=-Wall -Werror -g

all: build

lint:
	prettyc \
		--recursive \
		--verbose=0 \
		--repository=. \
		--extensions=c,h,in \
		--linelength=80 \
		--headers=h,in \
		--includeorder=standardcfirst \
		--exclude=src \
		--exclude=doc \
		--exclude=w32 \
		--exclude=po \
		--exclude=libmicrohttpd.pc.in \
		--root=. \
		./

build:
	$(CC) $(CFLAGS) carrow_microhttpd.c -o carrow_microhttpd -lmicrohttpd \
		-lcarrow -lclog

run:
	./carrow_microhttpd

clean:
	rm ./carrow_microhttpd
