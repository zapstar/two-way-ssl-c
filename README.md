# Two-way SSL authentication example in C

A simple example program that demonstrates two-way authentication between a client and the server.

There are a couple of things to be noted here:

1. Hostname verification is not performed. If you want to perform it, you should take a look at [this repository](https://github.com/iSECPartners/ssl-conservatory).
2. Don't plugin this code directly into multi-threaded applications, you need to call some additional routines so that OpenSSL routines become reentrant.
3. You can generate the keys by looking at [this gist](https://gist.github.com/zapstar/4b51d7cfa74c7e709fcdaace19233443) or by using the included script `certs_gen.sh` (Uniy based OS).

## Steps to run the example

### Pre-requisities
* Any decent C compiler
* OpenSSL development library

### Build
```
make
```

### Create the certificates

To create the certificates and the private keys call `./certs_gen.sh`. It creates a subdirectory `certs` which contains the privates keys, the certificate requests and the certificates.

### Server

The server side can be executed either using the openssl CLI or this example.

**Using this example**
```
./ssl-2way server 8888 ./certs/ca/ca_cert.pem ./certs/server/server_cert.pem ./certs/server/private/server_key.pem
```

**Using the opensll CLI**
```
openssl s_server -accept 8888 -cert ./certs/server/server_cert.pem -key ./certs/server/private/server_key.pem -CAfile ./certs/ca/ca_cert.pem
```

### Client

The client side can be executed either using the openssl CLI or this example.

**Using this example**
```
./ssl-2way client 0.0.0.0:8888 ./certs/ca/ca_cert.pem ./certs/client/client_cert.pem ./certs/client/private/client_key.pem
```

**Using the opensll CLI**
```
openssl s_client -connect 0.0.0.0:8888 -cert ./certs/client/client_cert.pem -key ./certs/client/private/client_key.pem -CAfile ./certs/ca/ca_cert.pem -showcerts  -msg
```
