#ifndef QWCFILEINFO_H
#define QWCFILEINFO_H

#include <QtCore>
#include <QtGui>

namespace WiredTransfer {
	enum FileType { RegularFile, Directory, Uploads, DropBox };
}

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class QwcFileInfo
{

public:
    QwcFileInfo(QList<QByteArray> theParams);
    QwcFileInfo();
    ~QwcFileInfo();
    
	static QString humanReadableSize(qlonglong theBytes);
    void setFromStat(QList<QByteArray> theParams);
	
    
    // Default parameters
    QString path;
	WiredTransfer::FileType type;
    qlonglong size;
    QDateTime created;
    QDateTime modified;
    
    // STAT parameters
    QString checksum;
    QString comment;

    QString fileName() const;
	QIcon fileIcon() const;

	bool isIndexed; // used for recursive downloads
	QString localPath; // local path for folder uploads

};

Q_DECLARE_METATYPE(QwcFileInfo)

#endif
