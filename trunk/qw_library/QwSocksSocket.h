#ifndef QWSOCKSSOCKET_H
#define QWSOCKSSOCKET_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkProxy>

/*! This class provides support for SOCKS5 client connections and is used for Tor usage as long
    Qt's QNetworkProxy does not pass host name lookups to the SOCKS proxy.
*/

class QwSocksSocket : public QObject
{
public:
    QwSocksSocket();
    void setProxy(QNetworkProxy &proxy) { proxyInformation = proxy; }


private:
    QNetworkProxy proxyInformation;

};

#endif // QWSOCKSSOCKET_H
