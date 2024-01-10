#!/bin/sh


# Certificate Authority
CA_DIR=./certs/ca
CA_PDIR=${CA_DIR}/private
CA_CERT=${CA_DIR}/ca_cert.pem
CA_KEY=${CA_PDIR}/ca_key.pem

# Generate a self signed certificate for the CA along with a key.
echo -n "Generating auhtority private key and certificate... "
if [ ! -f "${CA_CERT}" ]; then
  mkdir -p "${CA_PDIR}" > /dev/null 2>&1
  chmod 700 "${CA_PDIR}"

  # NOTE: I'm using -nodes, this means that once anybody gets
  # their hands on this particular key, they can become this CA.
  openssl req \
      -x509 \
      -nodes \
      -days 3650 \
      -newkey rsa:4096 \
      -keyout certs/ca/private/ca_key.pem \
      -out certs/ca/ca_cert.pem \
      -subj "/C=US/ST=Acme State/L=Acme City/O=Acme Inc./CN=example.com" > /dev/null 2>&1

  [ $? -ne 0 ] && echo "FAILED" && exit 1
  echo "DONE"
else
  echo "SKIPPED"
fi

# Server
SERVER_DIR=./certs/server
SERVER_PDIR=${SERVER_DIR}/private
SERVER_CERT=${SERVER_DIR}/server_cert.pem
SERVER_CSR=${SERVER_DIR}/server_cert.csr
SERVER_KEY=${SERVER_PDIR}/server_key.pem

echo -n "Generating server private key and certificate... "
if [ ! -f "${SERVER_CSR}" ]; then
  mkdir -p "${SERVER_PDIR}" > /dev/null 2>&1
  chmod 700 "${SERVER_PDIR}"

  # private key
  openssl genrsa -out "${SERVER_KEY}" 4096 > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED TO GENERATE KEY" && exit 1

  # certificate request
  openssl req -new \
      -key "${SERVER_KEY}" \
      -out "${SERVER_CSR}" \
      -subj "/C=US/ST=Acme State/L=Acme City/O=Acme Inc./CN=server.example.com" > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED TO GENERATE CERTIFICATE REQUEST" && exit 1

  # certificate
  openssl x509 -req -days 1460 -in "${SERVER_CSR}" \
      -CA "${CA_CERT}" -CAkey "${CA_KEY}" \
      -CAcreateserial -out "${SERVER_CERT}" > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED" && exit 1

  echo "DONE"
else
  echo "SKIPPED"
fi

# Client
CLIENT_DIR=./certs/client
CLIENT_PDIR=${CLIENT_DIR}/private
CLIENT_CERT=${CLIENT_DIR}/client_cert.pem
CLIENT_CSR=${CLIENT_DIR}/client_cert.csr
CLIENT_KEY=${CLIENT_PDIR}/client_key.pem

echo -n "Generating client private key and certificate... "
if [ ! -f "${CLIENT_CSR}" ]; then
  mkdir -p "${CLIENT_PDIR}" > /dev/null 2>&1
  chmod 700 "${CLIENT_PDIR}"

  # private key
  openssl genrsa -out "${CLIENT_KEY}" 4096 > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED TO GENERATE KEY" && exit 1

  # certificate request
  openssl req -new \
      -key "${CLIENT_KEY}" \
      -out "${CLIENT_CSR}" \
      -subj "/C=US/ST=Acme State/L=Acme City/O=Acme Inc./CN=client.example.com" > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED TO GENERATE CERTIFICATE REQUEST" && exit 1

  # certificate
  openssl x509 -req -days 1460 -in "${CLIENT_CSR}" \
      -CA "${CA_CERT}" -CAkey "${CA_KEY}" \
      -CAcreateserial -out "${CLIENT_CERT}" > /dev/null 2>&1
  [ $? -ne 0 ] && echo "FAILED" && exit 1

  echo "DONE"
else
  echo "SKIPPED"
fi

exit 0