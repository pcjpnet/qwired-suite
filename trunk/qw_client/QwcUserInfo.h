#ifndef QWCUSERINFO_H
#define QWCUSERINFO_H

#include "QwUser.h"
//#include "QwMessage.h"
#include "QwTransferInfo.h"

#include <QtGui/QImage>
#include <QtCore/QCryptographicHash>

class QwMessage;

class QwcUserInfo
    : public QwUser
{

public:
    QwcUserInfo();

    static QwcUserInfo fromMessage308(const QwMessage *message);
    static QwcUserInfo fromMessage310(const QwMessage *message);

    QList<QwTransferInfo> & transfers();
    const QList<QwTransferInfo> & transfers() const;

    void parseTransfersFromData(const QByteArray &data, Qw::TransferType type);

    void setPrivilegesFromMessage602(const QwMessage *message);
    void appendPrivilegesFlags(QwMessage *message) const;

    bool setImageFromData(const QByteArray data);
    const QImage & userImage() const;
    void setUserImage(const QImage &image);

private:
    QList<QwTransferInfo> m_transfers;
    QImage m_userIcon;

};

Q_DECLARE_METATYPE(QwcUserInfo);

#endif
