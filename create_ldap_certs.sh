#!/bin/bash

# 1. Create CA (Certificate Authority)
openssl genrsa -out certs/ca.key 4096
openssl req -x509 -new -nodes -key certs/ca.key -sha256 -days 3650 \
  -subj "/C=DE/ST=Hessen/L=Darmstadt/O=xxx/CN=my-ca" -out certs/ca.crt

# 2. Create LDAP Server Key and CSR
openssl genrsa -out certs/ldap.key 2048
openssl req -new -key certs/ldap.key \
  -subj "/C=DE/ST=Hessen/L=Darmstadt/O=xxx/CN=localhost" -out certs/ldap.csr

# 3. Sign the LDAP Server certificate with our CA
openssl x509 -req -in certs/ldap.csr -CA certs/ca.crt -CAkey certs/ca.key \
  -CAcreateserial -out certs/ldap.crt -days 365 -sha256
