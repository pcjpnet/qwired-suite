#include "QwcPreferencesWidget.h"
#include "QwcGlobals.h"
#include "QwcMessageStyle.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QFileDialog>
#include <QtGui/QFontDialog>
#include <QtGui/QMessageBox>

QwcPreferencesWidget::QwcPreferencesWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);

    // Language menu
    generalLanguage->addItem(tr("Automatic (default)"), QString("_auto_"));
    generalLanguage->addItem("Dutch", QString("nl"));
    generalLanguage->addItem("English", QString(""));
    generalLanguage->addItem("French", QString("fr"));
    generalLanguage->addItem("German", QString("de"));
    generalLanguage->addItem("Italian", QString("it"));
    generalLanguage->addItem("Portuguese", QString("pt"));
    generalLanguage->addItem("Spanish", QString("es"));
    generalLanguage->addItem("Swedish", QString("se"));
    generalLanguage->addItem("Japanese", QString("jp"));

    initEventsMenu();
    initChatStyleMenu();
    loadPreferences();

}


QwcPreferencesWidget::~QwcPreferencesWidget()
{
}


/*! Populate the events popup menu with useful entries.
*/
void QwcPreferencesWidget::initEventsMenu()
{
    fEventsList->clear();
    QIcon eventIcon(":/icons/icn_event.png");
    fEventsList->addItem(eventIcon, tr("Connected"), QString("ServerConnected"));
    fEventsList->addItem(eventIcon, tr("Disconnected"), QString("ServerDisconnected"));
    fEventsList->addItem(eventIcon, tr("Server Error"), QString("ServerError"));
    fEventsList->insertSeparator(fEventsList->count());
    fEventsList->addItem(eventIcon, tr("User Joined"), QString("UserJoined"));
    fEventsList->addItem(eventIcon, tr("User Left"), QString("UserLeft"));
    fEventsList->addItem(eventIcon, tr("User Changed Nickname"), QString("UserChangedNick"));
    fEventsList->addItem(eventIcon, tr("User Changed Status"), QString("UserChangedStatus"));
    fEventsList->insertSeparator(fEventsList->count());
    fEventsList->addItem(eventIcon, tr("Chat Message Received"), QString("ChatReceived"));
    fEventsList->addItem(eventIcon, tr("Private Message Received"), QString("MessageReceived"));
    fEventsList->addItem(eventIcon, tr("News Posted"), QString("NewsPosted"));
    fEventsList->addItem(eventIcon, tr("Broadcast Message Received"), QString("BroadcastReceived"));
    fEventsList->insertSeparator(fEventsList->count());
    fEventsList->addItem(eventIcon, tr("Transfer Started"), QString("TransferStarted"));
    fEventsList->addItem(eventIcon, tr("Transfer Finished"), QString("TransferFinished"));
    fEventsList->addItem(eventIcon, tr("Transfer Error"), QString("TransferError"));
}


void QwcPreferencesWidget::initChatStyleMenu()
{
    QwcSingleton *singleton = &WSINGLETON::Instance();

    chatStyle->clear();
    chatStyle->addItem(tr("Default Style"), QVariant());

    QStringList availableStyles = singleton->messageStyles();
    foreach(QString style, availableStyles) {
        QDir styleDir(style);
        chatStyle->addItem(styleDir.dirName().remove(".QwiredChatStyle"), style);
    }
}


void QwcPreferencesWidget::on_buttonBox_clicked(QAbstractButton *button)
{
    if (!button) { return; }
    if (buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        savePreferences();
        this->close();
    } else if (buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        savePreferences();
    } else if (buttonBox->buttonRole(button) == QDialogButtonBox::RejectRole) {
        this->close();
    }
}


/*! Write the preferences permanently to the settings store.
*/
void QwcPreferencesWidget::savePreferences()
{
    QSettings settings;

    // General
    settings.setValue("general/icon", *generalIcon->pixmap());
    settings.setValue("general/nickname", generalNickname->text());
    settings.setValue("general/status", generalStatus->text());
    settings.setValue("general/language", generalLanguage->itemData(
            generalLanguage->currentIndex()).toString() );


    settings.setValue("general/downloadLocation", transferDownloadLocation->text());
    settings.setValue("general/queueTransfers", transferQueue->isChecked());

    // Interface
    settings.setValue("interface/userlist/font", userlistFont->property("fontString").toString());
    settings.setValue("interface/userlist/opacity", userlistOpacity->value());
    settings.setValue("interface/userlist/size", userlistIconSizeLarge->isChecked() ? "large" : "small");

    settings.setValue("interface/chat/style", chatStyle->currentText());
    settings.setValue("interface/chat/emoticons", chatEmoticons->isChecked());
    settings.setValue("interface/chat/font", chatFont->property("fontString").toString());

    settings.setValue("interface/news/font", newsFont->property("fontString").toString());


    // Trackers
    settings.beginWriteArray("trackers");
    for(int i=0; i<fTrackerList->count(); i++) {
        settings.setArrayIndex(i);
        QListWidgetItem *item = fTrackerList->item(i);
        if(item) {
            settings.setValue("name", item->text());
            settings.setValue("address", item->data(Qt::UserRole).toString() );
        }
    }
    settings.endArray();

    // FAVOURITES TAB
    //
    settings.beginWriteArray("bookmarks");
    for(int i=0; i<fBookmarksList->count(); i++) {
        settings.setArrayIndex(i);
        QListWidgetItem *item = fBookmarksList->item(i);
        if(item) {
            settings.setValue("name", item->text());
            settings.setValue("address", item->data(Qt::UserRole).toString() );
            settings.setValue("login", item->data(Qt::UserRole+1).toString() );
            settings.setValue("password", item->data(Qt::UserRole+2).toString() );
            settings.setValue("autoconnect", item->data(Qt::UserRole+3).toBool() );
        }
    }
    settings.endArray();

    // Proxy
    settings.setValue("proxy/type", proxyType->currentIndex());
    settings.setValue("proxy/host", proxyHost->text());
    settings.setValue("proxy/port", proxyPort->text().toInt());
    settings.setValue("proxy/username", proxyLogin->text());
    settings.setValue("proxy/password", proxyPassword->text());

    // Notify everyone
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    tmpS->emitApplicationSettingsChanged();
}


/*! Load the settings and populate the preferences dialog with them.
*/
void QwcPreferencesWidget::loadPreferences()
{
    QSettings settings;


    // General
    generalIcon->setPixmap(settings.value("general/icon",
                                          QPixmap(":/icons/qwired_logo_32.png")).value<QPixmap>());
    generalNickname->setText(settings.value("general/nickname", tr("Qwired Freshling")).toString());
    generalStatus->setText(settings.value("general/status", tr("Just getting wired.")).toString());
    generalLanguage->setCurrentIndex(generalLanguage->findData(
            settings.value("general/language").toString()));

    transferDownloadLocation->setText(settings.value("general/downloadLocation",
                                                     QDir::home().absoluteFilePath("Downloads")).toString());
    transferQueue->setChecked(settings.value("general/queueTransfers", false).toBool());


    // Interface
    QFont defaultFont;
    QString defaultFontString = QString("%1 %2").arg(defaultFont.family()).arg(defaultFont.pointSizeF());

    QFont fontInfo;
    fontInfo.fromString(settings.value("interface/userlist/font", defaultFont.toString()).toString());
    userlistFont->setText(QString("%1 %2").arg(fontInfo.family()).arg(fontInfo.pointSizeF()));
    userlistFont->setProperty("fontString", fontInfo.toString());
    userlistFont->setFont(fontInfo);

    userlistOpacity->setValue(settings.value("interface/userlist/opacity", 100).toInt());
    userlistIconSizeLarge->setChecked(settings.value("interface/userlist/size", "large").toString() == "large");
    userlistIconSizeSmall->setChecked(settings.value("interface/userlist/size", "large").toString() == "small");

    chatStyle->setCurrentIndex(chatStyle->findText(settings.value("interface/chat/style",
                                                                  tr("Default Style")).toString()));

    chatEmoticons->setChecked(settings.value("interface/chat/emoticons", true).toBool());

    fontInfo.fromString(settings.value("interface/chat/font", defaultFont.toString()).toString());
    chatFont->setText(QString("%1 %2").arg(fontInfo.family()).arg(fontInfo.pointSizeF()));
    chatFont->setProperty("fontString", fontInfo.toString());
    chatFont->setFont(fontInfo);

    fontInfo.fromString(settings.value("interface/news/font", defaultFont.toString()).toString());
    newsFont->setText(QString("%1 %2").arg(fontInfo.family()).arg(fontInfo.pointSizeF()));
    newsFont->setProperty("fontString", fontInfo.toString());
    newsFont->setFont(fontInfo);

    // FAVOURITES TAB
    //
    fBookmarksList->clear();
    int tmpCnt = settings.beginReadArray("bookmarks");
    for(int i=0; i<tmpCnt; ++i) {
        settings.setArrayIndex(i);
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(settings.value("name").toString());
        item->setData(Qt::UserRole, settings.value("address").toString());
        item->setData(Qt::UserRole+1, settings.value("login").toString());
        item->setData(Qt::UserRole+2, settings.value("password").toString());
        item->setData(Qt::UserRole+3, settings.value("autoconnect").toBool());
        item->setIcon(QIcon(":/icons/icn_trackerserver.png"));
        fBookmarksList->addItem(item);
    }
    settings.endArray();
    fBookmarksList->setCurrentRow(-1);



    // INTERFACE TAB
    //
    QFont f;


    // Trackers
    int count = settings.beginReadArray("trackers");
    for(int i=0; i<count; i++) {
        settings.setArrayIndex(i);
        QListWidgetItem *item = new QListWidgetItem(fTrackerList);
        item->setText( settings.value("name").toString() );
        item->setIcon(QIcon(":/icons/icn_tracker.png"));
        item->setData( Qt::UserRole, settings.value("address").toString() );
    }
    settings.endArray();

    // Proxy
    proxyType->setCurrentIndex(settings.value("proxy/type").toInt());
    proxyHost->setText(settings.value("proxy/host").toString());
    proxyPort->setText(QString::number(settings.value("proxy/port").toInt()));
    proxyLogin->setText(settings.value("proxy/username").toString());
    proxyPassword->setText(settings.value("proxy/password").toString());

}




/*! Show a file browser dialog and allow the user to select a new icon.
*/
void QwcPreferencesWidget::on_btnGeneralSetIcon_clicked()
{
    QString imageFile = QFileDialog::getOpenFileName(
            this, tr("Select Icon"), QDir::homePath(), tr("Images (*.png)"));
    if (imageFile.isEmpty()) { return; }

    QPixmap iconPixmap(imageFile);
    if (imageFile.isNull()) {
         QMessageBox::warning(this, tr("Invalid Image File"),
                              tr("Could not open the selected image because it could be read."));
         return;
    }
    generalIcon->setPixmap(iconPixmap);
}


/*! Remove the user icon and restore the default one.
*/
void QwcPreferencesWidget::on_btnGeneralDefaultIcon_clicked()
{
    generalIcon->setPixmap(QPixmap(":/icons/qwired_logo_32.png"));
}


void QwcPreferencesWidget::on_btnTransferDownloadLocation_clicked()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this, tr("Select Download Location"),
                                            transferDownloadLocation->text(),
                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (selectedDir.isEmpty()) { return; }
    transferDownloadLocation->setText(selectedDir);
}


void QwcPreferencesWidget::on_btnFilesFolderSet_clicked()
{
    // Select a file as default download directory
    QString downloadDirectory = QFileDialog::getExistingDirectory(
            this, tr("Select Download Directory"), QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    if(downloadDirectory.isEmpty()) { return; }
    transferDownloadLocation->setText(downloadDirectory);
}


void QwcPreferencesWidget::on_btnChatFont_clicked()
{
    bool ok;
    QFont font;
    font.fromString(chatFont->property("fontString").toString());
    font = QFontDialog::getFont(&ok, font, this);
    if (!ok) { return; }
    chatFont->setText(QString("%1 %2").arg(font.family()).arg(font.pointSizeF()));
    chatFont->setProperty("fontString", font.toString());
    chatFont->setFont(font);
}



void QwcPreferencesWidget::on_btnUserlistFont_clicked()
{
    bool ok;
    QFont font;
    font.fromString(userlistFont->property("fontString").toString());
    font = QFontDialog::getFont(&ok, font, this);
    if (!ok) { return; }
    userlistFont->setText(QString("%1 %2").arg(font.family()).arg(font.pointSizeF()));
    userlistFont->setProperty("fontString", font.toString());
    userlistFont->setFont(font);
}



void QwcPreferencesWidget::on_btnNewsFont_clicked()
{
    bool ok;
    QFont font;
    font.fromString(newsFont->property("fontString").toString());
    font = QFontDialog::getFont(&ok, font, this);
    if (!ok) { return; }
    newsFont->setText(QString("%1 %2").arg(font.family()).arg(font.pointSizeF()));
    newsFont->setProperty("fontString", font.toString());
    newsFont->setFont(font);
}


//
// Trackers
//

void QwcPreferencesWidget::on_fTrackerName_editingFinished()
{
    QListWidgetItem *item = fTrackerList->currentItem();
    if (!item) { return; }
    item->setText( fTrackerName->text() );
}

void QwcPreferencesWidget::on_fTrackerAddress_editingFinished()
{
    QListWidgetItem *item = fTrackerList->currentItem();
    if (!item) { return; }
    item->setData(Qt::UserRole, fTrackerAddress->text() );
}

void QwcPreferencesWidget::on_fBtnTrackerNew_clicked()
{
    QListWidgetItem *item = new QListWidgetItem(fTrackerList);
    item->setText(tr("New Tracker"));
    item->setIcon(QIcon(":/icons/icn_tracker.png"));
    item->setData(Qt::UserRole, tr("my.tracker.com"));
    fTrackerList->setCurrentItem(item);
}

void QwcPreferencesWidget::on_fTrackerList_currentRowChanged(int current)
{
    if (current==-1) {
        // Disable gui
        fTrackerGroup->setEnabled(false);
        fBtnTrackerDelete->setEnabled(false);
    } else {
        QListWidgetItem *item = fTrackerList->item(current);
        fTrackerGroup->setEnabled(true);
        fTrackerName->setText( item->text() );
        fTrackerAddress->setText( item->data(Qt::UserRole).toString() );
        fBtnTrackerDelete->setEnabled(true);
    }
}


void QwcPreferencesWidget::on_fBtnTrackerDelete_clicked()
{
    int tmpRow = fTrackerList->currentRow();
    if (tmpRow > -1) {
        QListWidgetItem *item = fTrackerList->takeItem(tmpRow);
        delete item;
    }
    fTrackerList->setCurrentRow(-1);
}



// BOOKMARKS
//
void QwcPreferencesWidget::on_btnBMAdd_clicked() {
    QListWidgetItem *item = new QListWidgetItem(fBookmarksList);
    item->setText(tr("New Bookmark"));
    item->setIcon(QIcon(":/icons/icn_trackerserver.png"));
    fBookmarksList->setCurrentRow( fBookmarksList->count()-1);
}

void QwcPreferencesWidget::on_btnBMRemove_clicked() {
    int tmpRow = fBookmarksList->currentRow();
    if(tmpRow>-1) {
        QListWidgetItem *item = fBookmarksList->takeItem(tmpRow);
        delete item;
    }
    fBookmarksList->setCurrentRow(-1);
}

void QwcPreferencesWidget::on_fBookmarksList_currentRowChanged(int current) {
    if(current==-1) { // Disable gui
        fBMGroup->setEnabled(false);
        btnBMRemove->setEnabled(false);
    } else {
        QListWidgetItem *item = fBookmarksList->item(current);
        fBMGroup->setEnabled(true);
        btnBMRemove->setEnabled(true);
        fBMName->setText( item->text() );
        fBMAddr->setText( item->data(Qt::UserRole).toString() );
        fBMLogin->setText( item->data(Qt::UserRole+1).toString() );
        fBMPassword->setText( item->data(Qt::UserRole+2).toString() );
        fBMAutoConnect->setChecked( item->data(Qt::UserRole+3).toBool() );
    }
}

void QwcPreferencesWidget::on_fBMName_editingFinished() {
    QListWidgetItem *item = fBookmarksList->currentItem();
    if(!item) return;
    item->setText(fBMName->text());
}

void QwcPreferencesWidget::on_fBMAddr_editingFinished() {
    QListWidgetItem *item = fBookmarksList->currentItem();
    if(!item) return;
    item->setData(Qt::UserRole, fBMAddr->text());
}

void QwcPreferencesWidget::on_fBMLogin_editingFinished() {
    QListWidgetItem *item = fBookmarksList->currentItem();
    if(!item) return;
    item->setData(Qt::UserRole+1, fBMLogin->text());
}

void QwcPreferencesWidget::on_fBMPassword_editingFinished() {
    QListWidgetItem *item = fBookmarksList->currentItem();
    if(!item) return;
    item->setData(Qt::UserRole+2, fBMPassword->text());
}

void QwcPreferencesWidget::on_fBMAutoConnect_stateChanged(int state) {
    QListWidgetItem *item = fBookmarksList->currentItem();
    if(!item) return;
    item->setData(Qt::UserRole+3, state==Qt::Checked);
}

void QwcPreferencesWidget::on_fEventsList_currentIndexChanged(int index)
{
    if (index < 0) { return; }
    QSettings settings;
    QString eventKey = fEventsList->itemData(index, Qt::UserRole).toString();
    fEventsSound->setText(settings.value(QString("events/%1/sound").arg(eventKey), tr("(none)")).toString());
    fEventsPostChat->setChecked(settings.value(QString("events/%1/chat").arg(eventKey) ).toBool());
    fEventsDisplayTrayMsg->setChecked(settings.value(QString("events/%1/traymsg").arg(eventKey)).toBool());
    fEventsSysCmdEnabled->setChecked(settings.value(QString("events/%1/syscmd_enabled").arg(eventKey)).toBool());
    fEventsSysCmd->setText(settings.value(QString("events/%1/syscmd").arg(eventKey)).toString());
}

void QwcPreferencesWidget::on_fEventsDisplayTrayMsg_stateChanged(int state)
{
    QSettings settings;
    settings.setValue(QString("events/%1/traymsg").arg(
            fEventsList->itemData(fEventsList->currentIndex(), Qt::UserRole).toString()), state == Qt::Checked);
}


void QwcPreferencesWidget::on_fEventsSysCmd_editingFinished()
{
    QSettings settings;
    settings.setValue(QString("events/%1/syscmd").arg(
            fEventsList->itemData(fEventsList->currentIndex(), Qt::UserRole).toString()), fEventsSysCmd->text());
}

void QwcPreferencesWidget::on_fEventsSysCmdEnabled_stateChanged(int state)
{
    QSettings settings;
    settings.setValue(QString("events/%1/syscmd_enabled").arg(
            fEventsList->itemData(fEventsList->currentIndex(), Qt::UserRole).toString()), state==Qt::Checked);
}

void QwcPreferencesWidget::on_fEventsPostChat_stateChanged(int state)
{
    QSettings settings;
    settings.setValue(QString("events/%1/chat").arg(
            fEventsList->itemData(fEventsList->currentIndex(), Qt::UserRole).toString()), state==Qt::Checked);
}


void QwcPreferencesWidget::on_fEventsSound_clicked()
{
    QString eventKey = fEventsList->itemData(fEventsList->currentIndex(), Qt::UserRole).toString();
    QSettings settings;
    QString selectedFile = QFileDialog::getOpenFileName(
            this, tr("Select Sound File"), QDir::homePath(),
            "Sound files (*.*)");

    if (selectedFile.isEmpty()) {
        settings.remove(QString("events/%1/sound").arg(eventKey));
        fEventsSound->setText(tr("(none)"));
    } else {
        settings.setValue(QString("events/%1/sound").arg(eventKey), selectedFile);
        fEventsSound->setText(selectedFile);
    }
}




