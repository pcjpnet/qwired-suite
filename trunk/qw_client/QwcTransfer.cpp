#include "QwcTransfer.h"

QwcTransfer::QwcTransfer(Qwc::TransferType type, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_state = Qwc::TransferStateInvalid;
}


/*! Returns the type of the transfer object. */
Qwc::TransferType QwcTransfer::type() const
{ return m_type; }

/*! Returns the current state of the transfer. */
Qwc::TransferState QwcTransfer::state() const
{ return m_state; }


QString QwcTransfer::localPath() const
{ return m_localPath; }

void QwcTransfer::setLocalPath(const QString &path)
{ m_localPath = path; }

QString QwcTransfer::remotePath() const
{ return m_remotePath; }

void QwcTransfer::setRemotePath(const QString &path)
{ m_remotePath = path; }
