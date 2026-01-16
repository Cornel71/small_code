#!/bin/bash
echo -n | openssl s_client -servername $1 -connect $1:443 2>/dev/null | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' > $1.crt
date=$(openssl x509 -in $1.crt -enddate -noout | sed "s/.*=\(.*\)/\1/" | awk -F " " '{print $1,$2,$3,$4}')
date_s=$(date -d "%b %d %T %Y" -d "$date" "+%s")
now_s=$(date +%s)
date_diff=$(( (date_s - now_s) / 86400 ))
echo "Certificate for $1 will expire in $date_diff days"
