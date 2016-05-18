/*
 *  main.c
 *  OpenSSL
 *
 *  Created by Thirumal Venkat on 18/05/16.
 *  Copyright © 2016 Thirumal Venkat. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "client.h"
#include "server.h"

/*
 * Print the usage of the current command line tool
 */
void usage() {
    fprintf(stderr, "Usage: ./a.out "
            /* 2 */ "(server <port_num> | client <server_ip>:<server_port>) "
            /* 3 */ "<CAfile_pem> "
            /* 4 */ "<cert_pem> "
            /* 5 */ "<key_pem>\n");
}

/*
 * Main routine
 */
int main(int argc, const char * argv[]) {
    if (argc != 6) {
        usage(); return -1;
    } else if (!strcmp(argv[1], "server")) {
        return server(argv[2], argv[3], argv[4], argv[5]);
    } else if (!strcmp(argv[1], "client")) {
        return client(argv[2], argv[3], argv[4], argv[5]);
    } else {
        usage(); return -1;
    }
}
