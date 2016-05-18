/*
 *  server.h
 *  OpenSSL
 *
 *  Created by Thirumal Venkat on 18/05/16.
 *  Copyright © 2016 Thirumal Venkat. All rights reserved.
 */

#ifndef server_h
#define server_h

int server(const char *port_str,
           const char *ca_pem,
           const char *cert_pem,
           const char *key_pem);

#endif /* server_h */
