#include "QwcServerInfoWidget.h"

QwcServerInfoWidget::QwcServerInfoWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}


/*! Set the contents of the server information widget from \a socket.
*/
void QwcServerInfoWidget::setInformationFromSocket(const QwcSocket *socket)
{
    if(!socket) { return; }

    const QwServerInfo &serverInfo = socket->serverInfo;

    fBanner->setPixmap(QPixmap::fromImage(socket->serverImage));
    fServerName->setText(serverInfo.name);
    fUptime->setText(serverInfo.startTime.toString());
    fDescription->setText(serverInfo.description);
    fURL->setText(QString("qwired://%1/").arg(socket->sslSocket()->peerAddress().toString()));
    fFiles->setText(QString::number(serverInfo.filesCount));
    fSize->setText(QwFile::humanReadableSize(socket->serverInfo.filesSize));
    fVersion->setText(serverInfo.serverVersion );
    fProtocol->setText(serverInfo.protocolVersion);

    /*: The milliseconds ping-time info for the server. */
    fLatency->setText(tr("%1 ms").arg(socket->pingLatency()));

    // SSL Protocol
    QSsl::SslProtocol sslProtocol = socket->sslSocket()->protocol();

    /*: If the encryption (SSL) protocol is unknown, this is shown instead. */
    QString protocolName = tr("Unknown Protocol");

    switch(sslProtocol) {
        case QSsl::SslV3: protocolName = "SSLv3"; break;
        case QSsl::SslV2: protocolName = "SSLv2"; break;
        case QSsl::TlsV1: protocolName = "TLSv1"; break;
        default: break;
        }

    fSslProtocol->setText(protocolName);
};

