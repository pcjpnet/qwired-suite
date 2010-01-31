#include "QwsFile.h"

#include <QtDebug>
#include <QDir>
#include <QCryptographicHash>
#include <QtSql>


QwsFile::QwsFile() : QwFile()
{
    offset = 0;
}


/*! This method returns true if the path is within the current folder root - and thus secure.
    localAbsolutePath is updated after this method has been called.
*/
bool QwsFile::isWithinLocalRoot()
{
    QString localPath = this->localFilesRoot + "/" + this->m_remotePath;
    QFileInfo localFilesRootInfo(this->localFilesRoot);
    QFileInfo localPathInfo(localPath);
    localAbsolutePath = localPathInfo.absoluteFilePath();

    // Check if the file is part of the files root
    qDebug() << "LocalRoot:" << localFilesRoot << "---" << localAbsolutePath;
    qDebug() << "LocalAbsoluteRoot:" << localFilesRootInfo.absoluteFilePath();
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absoluteFilePath())) {
        qDebug() << this << "Blocking access above files root:" << m_remotePath;
        return false;
    }
    return true;
}


/*! This method uses \a localFilesRoot and \a path to find a file on the local file system. If the
    file or directory exists, this method returns true and loads available information of the path
    into its properties.
    This method also returns false if the targeted file attempts to access a path above the defined
    \a localFilesRoot path.
    If \a quickCheck is true, no checksum will be calculated for the file.
*/
bool QwsFile::updateLocalPath(bool quickCheck)
{
    QString localPath = this->localFilesRoot + "/" + this->m_remotePath;
    QFileInfo localFilesRootInfo(this->localFilesRoot);
    QFileInfo localPathInfo(localPath);
    localAbsolutePath = localPathInfo.absoluteFilePath();


    // Check if the file is part of the files root
//    qDebug() << "LocalRoot:" << localFilesRootInfo.absoluteFilePath() << "---" << localAbsolutePath;
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absoluteFilePath())) {
        qDebug() << this << "Blocking access above files root:" << m_remotePath;
        return false;
    }

    // Check if the local path exists
    if (!localPathInfo.exists() && !localPathInfo.isReadable()) {
        // The file does not exist
        qDebug() << this << "Path is not readable:" << m_remotePath;
        return false;
    }

    // Exists - now read some info

    this->modified = localPathInfo.lastModified();
    this->created = localPathInfo.created();

    if (localPathInfo.isDir()) {
        // Read directory information
        QDir localPathDir(localAbsolutePath);
        localPathDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        this->type = Qw::FileTypeFolder;
        this->m_size = localPathDir.count();
        this->checksum = "";
    } else {
        // Read regular file
        this->type = Qw::FileTypeRegular;
        this->m_size = localPathInfo.size();
        if (!quickCheck) {
            updateLocalChecksum();
        }
    }

    return true;
}

/*! Load information about this \a path from the database. Information includes user comments, the
    folder type and other information.
*/
bool QwsFile::loadMetaInformation()
{
    qDebug() << this << "Loading file information from database.";
    QSqlQuery query;
    query.prepare("SELECT file_type, file_comment "
                  "FROM qws_files "
                  "WHERE file_path = :_path AND file_name = :_name");
    query.bindValue(":_path", this->m_remotePath.section("/", 0, -2).prepend("/") );
    query.bindValue(":_name", this->m_remotePath.section("/", -1, -1));

    qDebug() << this->m_remotePath.section("/", 0, -2) << this->m_remotePath.section("/", 1, -1);

    if (!query.exec()) {
        qDebug() << this << "Unable to load file info from database:" << query.lastError().text();
        return false;
    }

    query.first();
    if (!query.isValid()) {
        qDebug() << this << "Unable to load file info from database: Not found";
        return false;
    }

    this->comment = query.value(1).toString();
    this->type = (Qw::FileType)query.value(0).toInt();
    return true;
}


/*! Write information about this \a path to the database. Information includes user comments, the
    folder type and other information.
*/
bool QwsFile::saveMetaInformation()
{
    qDebug() << this << "Write file information to database.";
    QString fileDirPath = this->m_remotePath.section("/", 0, -2).prepend("/");
    QString fileName = this->m_remotePath.section("/", -1, -1);

    QSqlQuery query;

    // Delete the old file data
    query.prepare("DELETE FROM qws_files "
                  "WHERE file_path = :_path AND file_name = :_name");
    query.bindValue(":_path", fileDirPath);
    query.bindValue(":_name", fileName);
    if (!query.exec()) {
        qDebug() << this << "Unable to delete old file data from database:" << query.lastError().text();
        return false;
    }

    // Write the new one
    query.clear();
    query.prepare("INSERT INTO qws_files (file_path, file_name, file_type, file_comment) "
                  "VALUES (:_path, :_name, :_type, :_comment)");
    query.bindValue(":_path", fileDirPath);
    query.bindValue(":_name", fileName);
    query.bindValue(":_type", this->type);
    query.bindValue(":_comment", this->comment);
    if (!query.exec()) {
        qDebug() << this << "Unable to write file info to database: " << query.lastError().text();
        return false;
    }

    return true;
}


/*! Remove a specific item from the database.
*/
bool QwsFile::clearMetaInformation()
{
    qDebug() << this << "Remove file information from database.";
    QString fileDirPath = this->m_remotePath.section("/", 0, -2).prepend("/");
    QString fileName = this->m_remotePath.section("/", -1, -1);

    QSqlQuery query;

    // Delete the old file data
    query.prepare("DELETE FROM qws_files "
                  "WHERE file_path = :_path AND file_name = :_name");
    query.bindValue(":_path", fileDirPath);
    query.bindValue(":_name", fileName);
    if (!query.exec()) {
        qDebug() << this << "Unable to delete old file data from database:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

