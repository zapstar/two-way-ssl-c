/*
 *  client.c
 *  OpenSSL
 *
 *  Created by Thirumal Venkat on 18/05/16.
 *  Copyright © 2016 Thirumal Venkat. All rights reserved.
 */

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>

#include "client.h"

#define BUFSIZE 128

static SSL_CTX *get_client_context(const char *ca_pem,
                              const char *cert_pem,
                              const char *key_pem) {
    SSL_CTX *ctx;

    /* Create a generic context */
    if (!(ctx = SSL_CTX_new(SSLv23_client_method()))) {
        fprintf(stderr, "Cannot create a client context\n");
        return NULL;
    }

    /* Load the client's CA file location */
    if (SSL_CTX_load_verify_locations(ctx, ca_pem, NULL) != 1) {
        fprintf(stderr, "Cannot load client's CA file\n");
        goto fail;
    }

    /* Load the client's certificate */
    if (SSL_CTX_use_certificate_file(ctx, cert_pem, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Cannot load client's certificate file\n");
        goto fail;
    }

    /* Load the client's key */
    if (SSL_CTX_use_PrivateKey_file(ctx, key_pem, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Cannot load client's key file\n");
        goto fail;
    }

    /* Verify that the client's certificate and the key match */
    if (SSL_CTX_check_private_key(ctx) != 1) {
        fprintf(stderr, "Client's certificate and key don't match\n");
        goto fail;
    }

    /* We won't handle incomplete read/writes due to renegotiation */
    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    /* Specify that we need to verify the server's certificate */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    /* We accept only certificates signed only by the CA himself */
    SSL_CTX_set_verify_depth(ctx, 1);

    /* Done, return the context */
    return ctx;

fail:
    SSL_CTX_free(ctx);
    return NULL;
}

int client(const char *conn_str, const char *ca_pem,
           const char *cert_pem, const char *key_pem) {
    static char buffer[BUFSIZE];
    SSL_CTX *ctx;
    BIO *sbio;
    SSL *ssl;
    size_t len;
    int rc = 0;

    /* Initialize OpenSSL */
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    /* Get a context */
    if (!(ctx = get_client_context(ca_pem, cert_pem, key_pem))) {
        return -1;
    }

    /* Get a BIO */
    if (!(sbio = BIO_new_ssl_connect(ctx))) {
        fprintf(stderr, "Could not get a BIO object from context\n");
        rc = -1;
        goto fail1;
    }

    /* Get the SSL handle from the BIO */
    BIO_get_ssl(sbio, &ssl);

    /* Connect to the server */
    if (BIO_set_conn_hostname(sbio, conn_str) != 1) {
        fprintf(stderr, "Could not connecto to the server\n");
        rc = -1;
        goto fail2;
    }

    /* Perform SSL handshake with the server */
    if (SSL_do_handshake(ssl) != 1) {
        fprintf(stderr, "SSL Handshake failed\n");
        rc = -1;
        goto fail2;
    }

    /* Verify that SSL handshake completed successfully */
    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        fprintf(stderr, "Verification of handshake failed\n");
        rc = -1;
        goto fail2;
    }

    /* Inform the user that we've successfully connected */
    printf("SSL handshake successful with %s\n", conn_str);

    /* Read a line from the user */
    if (!fgets(buffer, BUFSIZE, stdin)) {
        fprintf(stderr, "Could not read input from the user\n");
        rc = -1;
        goto fail3;
    }

    /* Get the length of the buffer */
    len = strlen(buffer);

    /* Write the input onto the SSL socket */
    if ((rc = SSL_write(ssl, buffer, (int) len)) != len) {
        fprintf(stderr, "Cannot write to the server\n");
        rc = -1;
        goto fail3;
    }

    /* Read from the server */
    if ((rc = SSL_read(ssl, buffer, BUFSIZE)) < 0) {
        fprintf(stderr, "Cannot read from the server\n");
        rc = -1;
        goto fail3;
    }

    /* Check if we've got back what we sent? (Not perfect, but OK for us) */
    if (len == rc) {
        /* Print it on the screen again */
        printf("%s", buffer);
    }

    /* Cleanup and exit */
fail3:
    BIO_ssl_shutdown(sbio);
fail2:
    BIO_free_all(sbio);
fail1:
    SSL_CTX_free(ctx);
    return -1;
}
