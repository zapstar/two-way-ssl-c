# Two-way SSL authentication example in C

A simple example program that demonstrates two-way authentication between a client
and the server.

There are a couple of things to be noted here:

1. Hostname verification is not performed. If you want to perform it, you should take a look at [this repository](https://github.com/iSECPartners/ssl-conservatory).
2. Don't plugin this code directly into multi-threaded applications, you need to call some additional routines so that OpenSSL routines become reentrant.
3. You can generate the keys by looking at [this gist](https://gist.github.com/zapstar/4b51d7cfa74c7e709fcdaace19233443).

## Steps to run the example

### Pre-requisities
* Any decent C compiler
* OpenSSL development library

### Build
```
make
```

### Server
```
./openssl server 8888 ./keys/ca/ca_cert.pem ./keys/server/server_cert.pem ./keys/server/private/server_key.pem
```

### Client
```
./openssl client 127.0.0.1:8888 ./keys/ca/ca_cert.pem ./keys/client/client_cert.pem ./keys/client/private/client_key.pem
```
