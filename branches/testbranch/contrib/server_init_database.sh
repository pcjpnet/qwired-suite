#!/bin/sh

TARGETDIR=$1

if [ ! -d $TARGETDIR ]; then
  echo "No target directory specified."
  exit 1
fi


echo "Creating empty Qwired Server database..."

SERVERDB=$TARGETDIR/qwired_server.db

if [ ! -f $SERVERDB ]; then
  touch "$SERVERDB"
  cat server_db.sql | sqlite3 "$SERVERDB"
else
  echo "Database exists ($SERVERDB) - skipping."
fi


CERTIFICATE=$TARGETDIR/qwired_server.crt
HOSTNAME=$(hostname)

echo "Generating new RSA key and certificate..."

if [ ! -f $CERTIFICATE ]; then
  openssl req -x509 -newkey rsa:1024 -subj "/CN=$HOSTNAME" -days 365 -nodes -keyout "$CERTIFICATE" -out "$CERTIFICATE" > /dev/null
else
  echo "Certificate ($CERTIFICATE) already exists - skipping."
fi

echo "---"
echo "Done. You now can start Qwired Server and log in."
echo "The default login and password is admin/admin."
echo "---"
