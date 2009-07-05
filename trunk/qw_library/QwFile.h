#ifndef QWFILE_H
#define QWFILE_H

#include <QMetaType>
#include <QDateTime>
#include <QString>

namespace Qw {
    enum FileType { /*! A regular file. */
                    FileTypeRegular,
                    /*! A regular folder/directory. */
                    FileTypeFolder,
                    /*! A folder where guest users can upload data to. */
                    FileTypeUploadsFolder,
                    /*! A folder where guest users can upload data to, but can't see contents. */
                    FileTypeDropBox
                };
}


class QwFile
{

public:
    QwFile();

    QString fileName() const;
    QString directoryPath() const;

    void updateLocalChecksum();

    /*! The relative location of the file or directory in the server's [protocol] file system. */
    QString path;
    /*! The length of the complete file or directory. */
    qint64 size;
    /*! The creation date for the file or directory. */
    QDateTime created;
    /*! The modification date for the file or directory. */
    QDateTime modified;
    /*! The SHA-1 checksum of the first Megabyte of the file contents. */
    QString checksum;
    /*! A user-defined comment about the file. */
    QString comment;
    /*! The type of the file. See \a Qw::FileType for more information. */
    Qw::FileType type;
    /*! The path to the file or directory on the local disk. This is used when generating checksums
        or when reading and writing to the file. A target path can be set when using this class for
        download-transfers in client-mode. If it is empty, a path should be generated automatically.
    */
    QString localAbsolutePath;

    static QString humanReadableSize(qint64 size);

};

Q_DECLARE_METATYPE(QwFile);

#endif // QWFILE_H
