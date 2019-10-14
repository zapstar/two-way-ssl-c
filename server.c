/*
 *  server.c
 *  OpenSSL
 *
 *  Created by Thirumal Venkat on 18/05/16.
 *  Copyright © 2016 Thirumal Venkat. All rights reserved.
 */

#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

/* Global variable that indicates work is present */
static int do_work = 1;

/* Buffer size to be used for transfers */
#define BUFSIZE 128

/*
 * Prepare a SSL context for use by the server
 */
static SSL_CTX *get_server_context(const char *ca_pem,
                            const char *cert_pem,
                            const char *key_pem) {
    SSL_CTX *ctx;

    /* Get a default context */
    if (!(ctx = SSL_CTX_new(SSLv23_server_method()))) {
        fprintf(stderr, "SSL_CTX_new failed\n");
        return NULL;
    }

    /* Set the CA file location for the server */
    if (SSL_CTX_load_verify_locations(ctx, ca_pem, NULL) != 1) {
        fprintf(stderr, "Could not set the CA file location\n");
        goto fail;
    }

    /* Load the client's CA file location as well */
    SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(ca_pem));

    /* Set the server's certificate signed by the CA */
    if (SSL_CTX_use_certificate_file(ctx, cert_pem, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Could not set the server's certificate\n");
        goto fail;
    }

    /* Set the server's key for the above certificate */
    if (SSL_CTX_use_PrivateKey_file(ctx, key_pem, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Could not set the server's key\n");
        goto fail;
    }

    /* We've loaded both certificate and the key, check if they match */
    if (SSL_CTX_check_private_key(ctx) != 1) {
        fprintf(stderr, "Server's certificate and the key don't match\n");
        goto fail;
    }

    /* We won't handle incomplete read/writes due to renegotiation */
    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    /* Specify that we need to verify the client as well */
    SSL_CTX_set_verify(ctx,
                       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       NULL);

    /* We accept only certificates signed only by the CA himself */
    SSL_CTX_set_verify_depth(ctx, 1);

    /* Done, return the context */
    return ctx;

fail:
    SSL_CTX_free(ctx);
    return NULL;
}

static int get_socket(int port_num) {
    struct sockaddr_in sin;
    int sock, val;

    /* Create a socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Cannot create a socket\n");
        return -1;
    }

    /* We don't want bind() to fail with EBUSY */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        fprintf(stderr, "Could not set SO_REUSEADDR on the socket\n");
        goto fail;
    }

    /* Fill up the server's socket structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port_num);

    /* Bind the socket to the specified port number */
    if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        fprintf(stderr, "Could not bind the socket\n");
        goto fail;
    }

    /* Specify that this is a listener socket */
    if (listen(sock, SOMAXCONN) < 0) {
        fprintf(stderr, "Failed to listen on this socket\n");
        goto fail;
    }

    /* Done, return the socket */
    return sock;
fail:
    close(sock);
    return -1;
}

int server(const char *port_str, const char *ca_pem,
           const char *cert_pem, const char *key_pem) {
    static char buffer[BUFSIZE];
    struct sockaddr_in sin;
    socklen_t sin_len;
    SSL_CTX *ctx;
    SSL *ssl;
    int port_num, listen_fd, net_fd, rc, len;

    /* Parse the port number, and then validate it's range */
    port_num = atoi(port_str);
    if (port_num < 1 || port_num > 65535) {
        fprintf(stderr, "Invalid port number: %s\n", port_str);
        return -1;
    }

    /* Initialize OpenSSL */
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    /* Get a server context for our use */
    if (!(ctx = get_server_context(ca_pem, cert_pem, key_pem))) {
        return -1;
    }

    /* Get a socket which is ready to listen on the server's port number */
    if ((listen_fd = get_socket(port_num)) < 0) {
        goto fail;
    }

    /* Get to work */
    while (do_work != 0) {
        /* Hold on till we can an incoming connection */
        sin_len = sizeof(sin);
        if ((net_fd = accept(listen_fd,
                             (struct sockaddr *) &sin,
                             &sin_len)) < 0) {
            fprintf(stderr, "Failed to accept connection\n");
            continue;
        }

        /* Get an SSL handle from the context */
        if (!(ssl = SSL_new(ctx))) {
            fprintf(stderr, "Could not get an SSL handle from the context\n");
            close(net_fd);
            continue;
        }

        /* Associate the newly accepted connection with this handle */
        SSL_set_fd(ssl, net_fd);

        /* Now perform handshake */
        if ((rc = SSL_accept(ssl)) != 1) {
            fprintf(stderr, "Could not perform SSL handshake\n");
            if (rc != 0) {
                SSL_shutdown(ssl);
            }
            SSL_free(ssl);
            continue;
        }

        /* Print success connection message on the server */
        printf("SSL handshake successful with %s:%d\n",
                    inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

        /* Echo server... */
        while ((len = SSL_read(ssl, buffer, BUFSIZE)) != 0) {
            if (len < 0) {
                fprintf(stderr, "SSL read on socket failed\n");
                break;
            } else if ((rc = SSL_write(ssl, buffer, len)) != len) {

                break;
            }
        }

        /* Successfully echoed, print on our screen as well */
        printf("%s", buffer);

        /* Cleanup the SSL handle */
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    /* Close the listening socket */
    close(listen_fd);

fail:
    SSL_CTX_free(ctx);
    return 0;
}
