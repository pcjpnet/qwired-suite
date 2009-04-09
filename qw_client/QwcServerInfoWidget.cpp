#include "QwcServerInfoWidget.h"

QwcServerInfoWidget::QwcServerInfoWidget(QWidget * parent)
{
    Q_UNUSED(parent)
            setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

void QwcServerInfoWidget::loadInfo(QwcSocket *socket)
{
    if(!socket) this->close();
    fBanner->setPixmap(QPixmap::fromImage(socket->serverImage));
    fServerName->setText( socket->serverInfo.name );
    fUptime->setText( socket->serverInfo.startTime.toString() );
    fDescription->setText( socket->serverInfo.description );
    fURL->setText( QString("wired://%1/").arg(socket->sslSocket()->peerAddress().toString() ) );
    fFiles->setText( QString::number(socket->serverInfo.filesCount) );
    fSize->setText( QString("%1 GB").arg( (float)socket->serverInfo.filesSize/1024/1024/1024 )  );
    fVersion->setText( socket->serverInfo.serverVersion );
    fProtocol->setText(socket->serverInfo.protocolVersion);

    // Protocol
    QSsl::SslProtocol proto = socket->sslSocket()->protocol();
    QString protoname = "Unknown";
    switch(proto){
                case QSsl::SslV3: protoname="SSLv3"; break;
                case QSsl::SslV2: protoname="SSLv2"; break;
                case QSsl::TlsV1: protoname="TLSv1"; break;
                default: protoname="Unknown"; break;
                }
    fSslProtocol->setText( protoname );
};

