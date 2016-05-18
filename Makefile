CC = /usr/bin/gcc
CFLAGS = -Wall -Werror -g
LDFLAGS = -lcrypto -lssl

all: build

build: client.h server.h
	$(CC) $(CFLAGS) -o openssl main.c client.c server.c $(LDFLAGS)

clean:
	rm -f *.o core openssl
