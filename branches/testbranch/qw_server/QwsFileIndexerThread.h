#ifndef QWSFILEINDEXERTHREAD_H
#define QWSFILEINDEXERTHREAD_H

#include <QThread>

class QwsFileIndexerThread : public QThread
{
    Q_OBJECT

public:
    QwsFileIndexerThread(QObject *parent=0);
    void run();

    /*! The path to the 'files' directory. Files are indexed relative to this path. */
    QString filesRootPath;

signals:
    /*! Emitted when the file indexing has completed. */
    void indexingCompleted(qint64 totalFileSize, int totalFileCount);
    /*! Emitted when a new log message is available to be written to the log. */
    void logMessage(QString message);

};


#endif // QWSFILEINDEXERTHREAD_H
