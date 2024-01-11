CC = /usr/bin/gcc
CFLAGS = -Wall -Werror -g
LDFLAGS = -lcrypto -lssl

all: build

build: client.h server.h
	$(CC) $(CFLAGS) -o ssl-2way main.c client.c server.c $(LDFLAGS)

clean:
	rm -f *.o core ssl-2way
