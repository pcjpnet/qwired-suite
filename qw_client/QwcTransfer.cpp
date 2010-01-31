#include "QwcTransfer.h"
#include "QwcTransferSocket.h"

QwcTransfer::QwcTransfer(Qwc::TransferType type, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_state = Qwc::TransferStateInactive;
    m_id = qrand();
    m_transferSocket = new QwcTransferSocket(this);
}


/*! Returns the type of the transfer object. */
Qwc::TransferType QwcTransfer::type() const
{ return m_type; }

/*! Returns the current state of the transfer. */
Qwc::TransferState QwcTransfer::state() const
{ return m_state; }

/*! Returns the unique ID for this transfer. */
Qwc::TransferId QwcTransfer::id() const
{ return m_id; }

/*! Starts the transfer.
*/
void QwcTransfer::start()
{
    m_transferSocket->reset();
}

/*! Stops the running transfer.
*/
void QwcTransfer::stop()
{

}


QString QwcTransfer::localPath() const
{ return m_localPath; }

void QwcTransfer::setLocalPath(const QString &path)
{ m_localPath = path; }

QString QwcTransfer::remotePath() const
{ return m_remotePath; }

void QwcTransfer::setRemotePath(const QString &path)
{ m_remotePath = path; }


const QList<QwFile> & QwcTransfer::transferFiles() const
{ return m_transferFiles; }

void QwcTransfer::setTransferFiles(const QList<QwFile> &files)
{ m_transferFiles = files; }
