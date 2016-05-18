/*
 *  client.h
 *  OpenSSL
 *
 *  Created by Thirumal Venkat on 18/05/16.
 *  Copyright © 2016 Thirumal Venkat. All rights reserved.
 */

#ifndef client_h
#define client_h

int client(const char *conn_str,
           const char *ca_pem,
           const char *cert_pem,
           const char *key_pem);

#endif /* client_h */
