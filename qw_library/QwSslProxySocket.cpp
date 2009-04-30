#include "QwSslProxySocket.h"

QwSslProxySocket::QwSslProxySocket() : QSslSocket()
{
}


/*! Establish a connection to a remote host using a SOCKS proxy.
    Overrides QSslSocket's implementation.
*/
void QwSslProxySocket::connectToHostEncrypted(const QString &hostName, quint16 port, OpenMode mode=ReadWrite)
{


}


/*! Set the proxy information to be used for this connection.
    Overrides QSslSocket's implementation.
*/
void setProxy(const QNetworkProxy &networkProxy)
{

}
