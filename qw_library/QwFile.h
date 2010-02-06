#ifndef QWFILE_H
#define QWFILE_H

#include <QMetaType>
#include <QDateTime>
#include <QString>

namespace Qw {
    enum FileType {
        /*! A regular file. */
        FileTypeRegular = 0,
        /*! A regular folder/directory. */
        FileTypeFolder = 1,
        /*! A folder where guest users can upload data to. */
        FileTypeUploadsFolder = 2,
        /*! A folder where guest users can upload data to, but can't see contents. */
        FileTypeDropBox = 3
    };
}


class QwFile
{

public:
    QwFile();

    bool loadFromLocalFile();
    QString calculateLocalChecksum() const;

    QString fileName() const;
    QString directoryPath() const;

    QString remotePath() const;
    void setRemotePath(const QString &path);

    QString localPath() const;
    void setLocalPath(const QString &path);

    qint64 size() const;
    void setSize(qint64 size);

    qint64 transferredSize() const;
    void setTransferredSize(qint64 size);

    QString checksum() const;
    void setChecksum(const QString &checksum);

    void setComment(const QString &comment);
    QString comment() const;

    void setCreated(QDateTime created);
    QDateTime created() const;

    void setModified(QDateTime modified);
    QDateTime modified() const;

    Qw::FileType type() const;
    void setType(Qw::FileType type);

    static QString humanReadableSize(qint64 size);


protected:
    /*! The relative location of the file or directory in the server's [protocol] file system. */
    QString m_remotePath;
    /*! The path to the file on the local file system. */
    QString m_localPath;
    /*! The length of the complete file or directory. */
    qint64 m_size;
    /*! The amount of bytes already transferred when used in a transfer context. */
    qint64 m_transferredSize;
    /*! The SHA-1 checksum of the first Megabyte of the file contents. */
    QString m_checksum;
    /*! The type of the file. See \a Qw::FileType for more information. */
    Qw::FileType m_type;
    /*! A user-defined comment about the file. */
    QString m_comment;
    /*! The creation date for the file or directory. */
    QDateTime m_created;
    /*! The modification date for the file or directory. */
    QDateTime m_modified;

};

Q_DECLARE_METATYPE(QwFile);

#endif // QWFILE_H
