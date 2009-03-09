#!/bin/sh




echo "Creating empty Qwired Server database..."
echo ""
SERVERDB=../bin/qwired_server.db
rm "$SERVERDB"
touch "$SERVERDB"
cat server_db.sql | sqlite3 "$SERVERDB"
echo ""
echo "Generating new RSA key and certificate..."
echo ""

CERTIFICATE=../bin/qwired_server.crt
HOSTNAME=$(hostname)
rm "$CERTIFICATE"
openssl req -x509 -newkey rsa:1024 -subj "/CN=$HOSTNAME" -days 365 -nodes -keyout "$CERTIFICATE" -out "$CERTIFICATE" > /dev/null

echo ""
echo "Done. You now can start Qwired Server and log in."
echo "The default login and password is admin/admin."
