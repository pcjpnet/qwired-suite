#ifndef QWSSLPROXYSOCKET_H
#define QWSSLPROXYSOCKET_H

#include <QSslSocket>
#include <QNetworkProxy>

namespace Qw {
    enum ProxySocketState { ProxySocketConnecting };
}

class QwSslProxySocket : public QSslSocket
{

public:
    QwSslProxySocket();
    void connectToHostEncrypted(const QString &hostName, quint16 port, OpenMode mode=ReadWrite);
    void setProxy(const QNetworkProxy &networkProxy);

private:
    QNetworkProxy proxyInfo;
    Qw::ProxySocketState proxyState;

};

#endif // QWSSLPROXYSOCKET_H
