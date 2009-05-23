#include "QwsFileIndexerThread.h"

#include <QtSql>

QwsFileIndexerThread::QwsFileIndexerThread(QObject *parent) : QThread(parent)
{
}


void QwsFileIndexerThread::run()
{
    emit logMessage(tr("Regenerating file index database..."));

    qint64 totalFilesSize = 0;
    int totalFilesCount = 0;

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    // First delete all existing entries from the file index table
    db.exec("DELETE FROM qws_files_index");

    QDir filesRootDir(filesRootPath);
    if (!filesRootDir.exists() || !filesRootDir.isReadable()) {
        emit logMessage(tr("Warning: Unable to run file indexer - files directory is not readable."));
        return;
    }

    QDirIterator i(filesRootPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (i.hasNext()) {
        i.next();
        QString localAbsoluteFilePath = i.fileInfo().absoluteFilePath();
        if (i.fileInfo().isHidden()) { continue; }
        QString relativePath = filesRootDir.relativeFilePath(localAbsoluteFilePath);
//        CREATE TABLE qws_files_index (id INTEGER PRIMARY KEY AUTOINCREMENT, file_dir_path TEXT, file_name TEXT, file_size INTEGER);

        qDebug() << relativePath;
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO qws_files_index (file_dir_path, file_name, file_size) "
                            "VALUES (:_dir, :_name, :_size)");
        insertQuery.bindValue(":_dir", "/"+relativePath.section("/", 0, -2, QString::SectionSkipEmpty));
        insertQuery.bindValue(":_name", relativePath.section("/", -1, -1, QString::SectionSkipEmpty));
        insertQuery.bindValue(":_size", i.fileInfo().size());
        if (!insertQuery.exec()) {
            emit logMessage(tr("Warning: Unable to write to file index database: %1")
                            .arg(insertQuery.lastError().text()));
            break;
        }

        totalFilesSize += i.fileInfo().size();
        totalFilesCount += 1;
    }

    db.commit();

    emit logMessage(tr("File indexing compelete - %1 files have been indexed.").arg(totalFilesCount));
    emit indexingCompleted(totalFilesSize, totalFilesCount);
    this->deleteLater();
}
