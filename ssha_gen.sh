#!/bin/bash
PASSWORD="$1"
SALT="$(openssl rand 4)"
SHA1="$(printf "%s%s" "$PASSWORD" "$SALT" | openssl dgst -binary -sha1)"
printf "{SSHA}%s\n" "$(printf "%s%s" "$SHA1" "$SALT" | base64)"
