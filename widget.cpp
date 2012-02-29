/**
 * CommandoJikkyouSennyou - Commando Jikkyou Sennyou Client for twitter for Qt.
 *
 * Author: amayav (vamayav@yahoo.co.jp)
 *
 *
 *  CommandoJikkyouSennyou is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CommandoJikkyouSennyou is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with CommandoJikkyouSennyou.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "widget.h"
#include "ui_widget.h"
#include <QTextCodec>
#include <QDebug>
#include <QKeyEvent>
#include "extrapushbutton.h"
#include <cmath>
#include <QLayoutItem>
#include <QDateTime>
#include "picojson.h"
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QDirIterator>

using namespace std;
using namespace picojson;

#define LATEST_NUMBER_FOR_TIME 128

//QString _consumerKey("nXrjGagSLIQxCC4AmJ1J3g");
//QString _consumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
/*
  publidc functions
  */
Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget),
      _oauthManager(new KQOAuthManager(parent)),
      _oauthRequest(new KQOAuthRequest),
      _oauthSettings(new QSettings(qApp->applicationDirPath() + "/verification.ini",
                                   QSettings::IniFormat)),
      _shortCutKeysSettings(new QSettings(qApp->applicationDirPath() + "/key.txt",
                                          QSettings::IniFormat)),
      _sizeSettings(new QSettings(qApp->applicationDirPath() + "/size.ini",
                                  QSettings::IniFormat)),
      _proxy(new QNetworkProxy()),
      _consumerKey(),
      _consumerSecretKey(),
      _clockCountTimer(new QTimer),
      _latestTweetsForTime(),
      _subject(new WidgetDomain)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    ui->setupUi(this);
    this->setPostShortcutKeys();
    this->setJikkyouShortcutKeys();
    this->setWindowSize();
    this->setProxyLineEdits();
    this->setConsumerKeyAndConsumerSecretLineEdits();
    this->setConsumerKeyAndConsumerSecret();

    // OAuth Settings
    _oauthSettings->setIniCodec("utf-8");
    if (_oauthSettings->childKeys().size() == 3) { //number, consumer_key, consumer_secret_key
        ui->lineEdit->setText(_oauthSettings->value("number").toString());
    }

    this->initializeKQOAuth();
    this->initializeOwnPostsWidgets();
    this->updateData();

    _clockCountTimer->setInterval(1000);
    _clockCountTimer->start();
    connect(_clockCountTimer, SIGNAL(timeout()), this, SLOT(countDownClock()), Qt::UniqueConnection);
}

Widget::~Widget() {
    delete _oauthManager;
    delete _oauthRequest;
    delete _oauthSettings;
    delete _shortCutKeysSettings;
    delete _sizeSettings;
    delete _proxy;
    delete _clockCountTimer;
    delete _subject;
    delete ui;
}

bool Widget::initializeKQOAuth(){
    _oauthManager->setNetworkManager(new QNetworkAccessManager);
    connect(_oauthManager->networkManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(receiveSSLError(QNetworkReply*,QList<QSslError>)), Qt::UniqueConnection);
    _oauthRequest->setEnableDebugOutput(false);
    return true;
}

bool Widget::setPostShortcutKeys(){
    // Short Cut Key for Post.
    QList<QShortcut*> postShortCutList;
    postShortCutList << new QShortcut(ui->lineEdit_2)
                     << new QShortcut(ui->lineEdit_3)
                     << new QShortcut(ui->lineEdit_4)
                     << new QShortcut(ui->lineEdit_5)
                     << new QShortcut(ui->lineEdit_6);
    for(QList<QShortcut*>::const_iterator it = postShortCutList.begin();it != postShortCutList.end();++it){
        this->setShortCutKeyAndContext(*it,
                                       QKeySequence(Qt::CTRL + Qt::Key_Return),
                                       Qt::WidgetShortcut);
        connect(*it, SIGNAL(activated()), ui->postButton, SLOT(click()));
    }
    return true;
}

bool Widget::setJikkyouShortcutKeys(){
    _shortCutKeysSettings->setIniCodec("utf-8");
    ExtraPushButton *button;
    for(int i=0;i<_shortCutKeysSettings->childKeys().size();++i){
        _subject->getKeysHash()->insert(_shortCutKeysSettings->childKeys().at(i),
                                        _shortCutKeysSettings->value(_shortCutKeysSettings->childKeys().at(i)).toString());
        button = new ExtraPushButton(_shortCutKeysSettings->childKeys().at(i),
                                     _shortCutKeysSettings->value(_shortCutKeysSettings->childKeys().at(i)).toString(),
                                     this);
        button->setToolTip(_shortCutKeysSettings->value(_shortCutKeysSettings->childKeys().at(i)).toString());
        ui->buttonsGridLayout->addWidget(button,0,i);
        // Succession of slots can not go well.
        connect(button, SIGNAL(clicked()), button, SLOT(click()));
        connect(button, SIGNAL(clicked(QString)), this, SLOT(sendTweet(QString)));
        connect(button, SIGNAL(showingToolTip(QString)), ui->plainTextEdit, SLOT(setPlainText(QString)));
    }
    return true;
}

bool Widget::setWindowSize(){
    _sizeSettings->beginGroup("size");
    if(_sizeSettings->value("width").isNull())
        resize(200,200);
    else
        resize(_sizeSettings->value("width").toInt(), _sizeSettings->value("height").toInt());
    _sizeSettings->endGroup();
    return true;
}

bool Widget::setProxyLineEdits(){
    _sizeSettings->beginGroup("proxy");
    if(_sizeSettings->value("hostName").isNull()){
        _sizeSettings->endGroup();
        return false;
    }
    _proxy->setHostName(_sizeSettings->value("hostName").toString());
    _proxy->setPort(_sizeSettings->value("port").toInt());
    _proxy->setUser(_sizeSettings->value("user").toString());
    _proxy->setPassword(_sizeSettings->value("password").toString());
    ui->proxyHostNameLineEdit->setText(_sizeSettings->value("hostName").toString());
    ui->proxyPortLineEdit->setText(_sizeSettings->value("port").toString());
    ui->proxyUserLineEdit->setText(_sizeSettings->value("user").toString());
    ui->proxyPasswordLineEdit->setText(_sizeSettings->value("password").toString());
    _sizeSettings->endGroup();
    return true;
}

bool Widget::setConsumerKeyAndConsumerSecretLineEdits(){
    _sizeSettings->beginGroup("consumerKey");
    if(_sizeSettings->value("consumerKey").isNull()){
        _sizeSettings->endGroup();
        return false;
    }
    ui->consumerKeyLineEdit->setText(_sizeSettings->value("consumerKey").toString());
    ui->consumerSecretKeyLineEdit->setText(_sizeSettings->value("consumerSecretKey").toString());
    _sizeSettings->endGroup();
    return true;
}

bool Widget::setConsumerKeyAndConsumerSecret(){
    _consumerKey = ui->consumerKeyLineEdit->text();
    _consumerSecretKey = ui->consumerSecretKeyLineEdit->text();
    return true;
}

bool Widget::SSLconnect(KQOAuthRequest *request)
{
    try{
        _oauthRequest = request;
        QNetworkRequest networkRequest;
        networkRequest.setUrl( _oauthRequest->requestEndpoint() );
        QSslConfiguration sslcon;
        sslcon.setCaCertificates(QSslCertificate::fromPath("./veri\\.cer",
                                                           QSsl::Pem,
                                                           QRegExp::RegExp));
        if(sslcon.caCertificates().size()==0){
            throw "There is not verification file \"veri.cer\".";
        }
        networkRequest.setSslConfiguration(sslcon);

        //_oauthRequest->setCallbackUrl(QUrl("oob"));

        networkRequest.setRawHeader("Authorization", makeAuthHeaderFrom(_oauthRequest->requestParameters()));

        connect(_oauthManager->networkManager(), SIGNAL(finished(QNetworkReply *)),
                _oauthManager, SLOT(onRequestReplyReceived(QNetworkReply *)), Qt::UniqueConnection);

        if (_oauthRequest->httpMethod() == KQOAuthRequest::GET) {
            // Get the requested additional params as a list of pairs we can give QUrl
            QList< QPair<QString, QString> > urlParams;
            for(int i=0; i<_oauthRequest->additionalParameters().keys().size(); i++) {
                urlParams.append( qMakePair(_oauthRequest->additionalParameters().keys().at(i),
                                            _oauthRequest->additionalParameters().values().at(i)) );
            }

            // Take the original URL and append the query params to it.
            QUrl urlWithParams = networkRequest.url();
            urlWithParams.setQueryItems(urlParams);
            networkRequest.setUrl(urlWithParams);

            // Submit the request including the params.
            //_replyOfUserStreamsAPI = _oauthManager->networkManager()->get(networkRequest);
            _subject->setReplyOfUserStreamsAPI(_oauthManager->networkManager()->get(networkRequest));
            connect(_subject->getReplyOfUserStreamsAPI(), SIGNAL(readyRead()),
                    this, SLOT(onReadyRead()), Qt::UniqueConnection);
            connect(_subject->getReplyOfUserStreamsAPI(), SIGNAL(error(QNetworkReply::NetworkError)),
                    _oauthManager, SLOT(slotError(QNetworkReply::NetworkError)), Qt::UniqueConnection);
            return true;
        } else if (_oauthRequest->httpMethod() == KQOAuthRequest::POST) {
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, _oauthRequest->contentType());

            qDebug() << networkRequest.rawHeaderList();
            qDebug() << networkRequest.rawHeader("Authorization");
            qDebug() << networkRequest.rawHeader("Content-Type");

            QNetworkReply *reply;
            if (_oauthRequest->contentType() == "application/x-www-form-urlencoded") {
                reply = _oauthManager->networkManager()->post(networkRequest, _oauthRequest->requestBody());
            } else {
                reply = _oauthManager->networkManager()->post(networkRequest, _oauthRequest->rawData());
            }

            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                    this, SLOT(slotError(QNetworkReply::NetworkError)), Qt::UniqueConnection);
            return true;
        }
        qDebug() << "Unexpected HTTP Method.";
        return false;
    }catch(QString str){
        qDebug() << "error" << str;
        return false;
    }
}

void Widget::onReadyRead()
{
    if( this->isOAuthed() == false) {
        qDebug() << "There are not oauth_token or oauth_token_secret";
        return;
    }
    onRequestReadyTimeline(_subject->getReplyOfUserStreamsAPI()->readAll());
}

void Widget::receiveSSLError(QNetworkReply *reply, const QList<QSslError> &errors)
{
    for(int i=0;i<errors.size();++i) {
        qDebug() << errors.at(i);
    }
}

bool Widget::setShortCutKeyAndContext(QShortcut *sc, QKeySequence s, Qt::ShortcutContext c)
{
    sc->setKey(s);
    sc->setContext(c);
    return true;
}

bool Widget::initializeRequest(QUrl url,
                               KQOAuthRequest::RequestHttpMethod requestMethod,
                               KQOAuthRequest::RequestType requestType)
{
    _oauthRequest->initRequest(requestType, url);
    _oauthRequest->setHttpMethod(requestMethod);
    _oauthRequest->setConsumerKey(_consumerKey);
    _oauthRequest->setConsumerSecretKey(_consumerSecretKey);
    return true;
}

bool Widget::setTokenAndSecretTokenToRequest()
{
    _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
    _oauthRequest->setTokenSecret(
                _oauthSettings->value("oauth_token_secret").toString());
    return true;
}

bool Widget::initializeOwnPostsWidgets()
{
    if( this->isOAuthed() == false){
        qDebug() << "Not Verified Yet.";
        return false;
    }

    qDebug("Initializing Own Posts");
    initializeRequest(QUrl("http://api.twitter.com/1/statuses/user_timeline.json"),
                      KQOAuthRequest::GET);
    setTokenAndSecretTokenToRequest();
    KQOAuthParameters params;
    params.insert("count", QByteArray::number(LATEST_NUMBER_FOR_TIME).constData());
    _oauthRequest->setAdditionalParameters(params);

    disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)), 0, 0);

    // I cannot understand why this connection is invalid.
    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReadyOwnPosts(QByteArray)), Qt::UniqueConnection);

    _oauthManager->executeRequest(_oauthRequest);

    connect(_oauthManager->networkManager(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(on_showTimeLineButton_clicked()), Qt::UniqueConnection);

    connect(_oauthManager->networkManager(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readOwnPosts(QNetworkReply*)), Qt::UniqueConnection);
    return true;
}

void Widget::readOwnPosts(QNetworkReply *r)
{
    onRequestReadyOwnPosts(r->readAll());
}

QByteArray Widget::makeAuthHeaderFrom(QList<QByteArray> requestParameters)
{
    bool first = true;
    QByteArray result;
    foreach (const QByteArray header, requestParameters) {
        if (!first) {
            result.append(", ");
        } else {
            result.append("OAuth ");
            first = false;
        }
        result.append(header);
    }
    return result;
}

bool Widget::isOAuthed()
{
    return _oauthSettings->value("oauth_token").toString().size()
            + _oauthSettings->value("oauth_token_secret").toString().size() > 0;
}

QString Widget::getValueOfMatchedKey()
{
    QStringList list(_subject->getKeysHash()->keys());
    unsigned int check = 0;
    for(int i=0;i<list.size();++i){
        if(list.at(i).startsWith(_subject->getKeyString()))
            ++check;
        if(check==2){
            qDebug() << "There are too many candities.";
            return "";
        }
    }
    if(check==0){
        qDebug() << "There is not such a short cut key.";
        return "";
    }
    return _subject->getKeysHash()->value(_subject->getKeyString());
}

WidgetDomain* Widget::getWidgetDomain()
{
    return _subject;
}

void Widget::countDownClock(){
    if(ui->tableWidget_2->rowCount() < LATEST_NUMBER_FOR_TIME)
        return;
    if(_latestTweetsForTime.getPostedDateTimeList().size() == 0)
        return;
    for(int i=0; i < ui->tableWidget_2->rowCount(); ++i){
        //latest number for time
        if(ui->tableWidget_2->item(i, 0) == NULL)
            break;

        QDateTime postedTime = _latestTweetsForTime.getPostedDateTimeList().at(i);
        QDateTime fromNow = QDateTime::currentDateTime();
        fromNow = fromNow.addYears(-postedTime.date().year());
        fromNow = fromNow.addMonths(-postedTime.date().month());
        fromNow = fromNow.addDays(-postedTime.date().day());
        fromNow = fromNow.addSecs(-postedTime.time().hour()*60*60
                                  -postedTime.time().minute()*60
                                  -postedTime.time().second()
                                  );
        ui->tableWidget_2->setItem(
                    LATEST_NUMBER_FOR_TIME - i - 1 , 1, new QTableWidgetItem(
                        ( (postedTime.secsTo(QDateTime::currentDateTime()) > 60*60*24)
                          ? (
                                QString::number(postedTime.secsTo(QDateTime::currentDateTime())/(60*60*24))
                                + (
                                    (postedTime.secsTo(QDateTime::currentDateTime())/(60*60*24) == 1)
                                    ? ( QString("day +") )
                                    : ( QString("days +") )
                                      )
                                )
                          : QString("")
                            )
                        + fromNow.toString("hh:mm:ss")
                        )
                    );
        if(postedTime.secsTo(QDateTime::currentDateTime()) <= 60*60*3){
            for(int j=0;j<ui->tableWidget_2->columnCount();++j){
                if(ui->tableWidget_2->item(LATEST_NUMBER_FOR_TIME - i - 1,j)==NULL)
                    continue;
                ui->tableWidget_2->item(LATEST_NUMBER_FOR_TIME - i - 1,j)->setBackground(QBrush(QColor(255, 0, 0), Qt::SolidPattern));
            }
        }
    }
}

void Widget::updateData()
{
}

bool Widget::setConsumerKeyAndConsumerSecretSettings(){
    _sizeSettings->beginGroup("consumerKey");
    _sizeSettings->setValue("consumerKey", ui->consumerKeyLineEdit->text());
    _sizeSettings->setValue("consumerSecretKey", ui->consumerSecretKeyLineEdit->text());
    _sizeSettings->endGroup();
    return true;
}

/*
  protected functions
  */
void Widget::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->isAutoRepeat()
            || (keyEvent->modifiers()!=Qt::ControlModifier)
            || (keyEvent->key()==Qt::Key_Control) )
        return;
    setAttribute(Qt::WA_KeyCompression);
    /*
          0x00:^@
          0x00-0x1a:^A-^Z
          0x1b:^[
          0x1c:^\
          0x1d:^]
          0x1e:^
          0x1f:^_

          0x40:@
          0x41-0x4a:A-Z
          ...
        */
    QString key = (keyEvent->text().toAscii().toHex().toInt(0, 16) <= 31)
            ? QByteArray::fromHex(QByteArray::number(keyEvent->text().toAscii().toHex().toInt(0, 16) + 64, 16))
            : keyEvent->text();
    _subject->setKeyString(_subject->getKeyString() + key.toLower());
    qDebug() << "Pressed Keys :" << keyEvent->text() << _subject->getKeyString() << keyEvent->count();

    for(int i=0;i<ui->buttonsGridLayout->count();++i){
        if(dynamic_cast<ExtraPushButton*>(ui->buttonsGridLayout->itemAt(i)->widget())->getShortCutKey().indexOf(_subject->getKeyString())==0){
            continue;
        }
        ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(false);
    }
}

void Widget::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->isAutoRepeat()
            ||  keyEvent->key()!=Qt::Key_Control
            || _subject->getKeyString().size()==0 )
        return;
    qDebug() << "ctrl!" << _subject->getKeyString();
    QString tweet = getValueOfMatchedKey();
    // Exception of Exchanging Strings
    if(tweet.contains(QRegExp("\\(\\d.*\\)"))){
        ui->plainTextEdit->setPlainText(tweet);
        for(int i=0;i<ui->buttonsGridLayout->count();++i)
            if(ui->buttonsGridLayout->itemAt(i)->widget()->isEnabled()==true)
                ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(false);
        ui->lineEdit_2->setFocus();
    }else{
        // Enable All Buttons
        for(int i=0;i<ui->buttonsGridLayout->count();++i)
            if(ui->buttonsGridLayout->itemAt(i)->widget()->isEnabled()==false)
                ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(true);
        sendTweet(tweet);
    }
    _subject->setKeyString("");
    tweet.clear();
}

void Widget::resizeEvent(QResizeEvent *)
{
    _sizeSettings->beginGroup("size");
    _sizeSettings->setValue("width", width());
    _sizeSettings->setValue("height", height());
    _sizeSettings->endGroup();
    //replace buttons.
    int beforeColumnsNumber =0;
    for(int i=ui->buttonsGridLayout->columnCount()-1;i>=0;--i)
    {
        if(ui->buttonsGridLayout->itemAtPosition(0,i)!=NULL)
        {
            beforeColumnsNumber = i+1;
            break;
        }
    }
    if(beforeColumnsNumber==0) {
        return;
    }

    QRect r;
    for(int i=beforeColumnsNumber-1;i>=0;--i){
        if(ui->buttonsGridLayout->itemAtPosition(0, i)!=NULL){
            r = ui->buttonsGridLayout->itemAtPosition(0, i)->widget()->frameGeometry();
            break;
        }
    }

    // When widget size becomes too small.
    if(r.x()+r.width() > ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width()){
        int afterColumnsNumber = 0;
        for(int i=beforeColumnsNumber-1;i>=0;--i){
            r = ui->buttonsGridLayout->itemAtPosition(0,i)->widget()->frameGeometry();
            if(r.x()+r.width()
                    < ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width()){
                afterColumnsNumber = i+1;
                break;
            }
        }
        if(afterColumnsNumber==0) {
            return;
        }
        QList<QLayoutItem*> *list = new QList<QLayoutItem*>();
        while(ui->buttonsGridLayout->itemAt(0)!=NULL){
            list->append(ui->buttonsGridLayout->itemAt(0)); // There are only extraPushButtons.
            ui->buttonsGridLayout->removeItem(ui->buttonsGridLayout->itemAt(0));
        }
        for(int i=0;i<list->size();++i){
            ui->buttonsGridLayout->addItem(list->at(i),
                                           floor((double)i/(double)afterColumnsNumber),
                                           i%afterColumnsNumber);
        }
    }
    // When widget size becomes too big
    else if(r.x() + r.width() + ui->buttonsGridLayout->horizontalSpacing() + r.width()
            < ui->groupBox->x() + ui->groupBox->frameGeometry().width()){
        int afterColumnsNumber = 0;
        for(int i=1;i<200;++i){
            if(r.x()+r.width() + i*(ui->buttonsGridLayout->horizontalSpacing() + ui->buttonsGridLayout->itemAt(0)->widget()->width())
                    > ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width()){
                afterColumnsNumber = beforeColumnsNumber+i;
                break;
            }
        }
        if(afterColumnsNumber > ui->buttonsGridLayout->count() || afterColumnsNumber==0){
            return;
        }
        QList<QLayoutItem*> *list = new QList<QLayoutItem*>();
        while(ui->buttonsGridLayout->itemAt(0)!=NULL){
            list->append(ui->buttonsGridLayout->itemAt(0)); // There are only extraPushButtons.
            ui->buttonsGridLayout->removeItem(ui->buttonsGridLayout->itemAt(0));
        }
        for(int i=0;i<list->size();++i){
            ui->buttonsGridLayout->addItem(list->at(i),
                                           floor((double)i/(double)afterColumnsNumber),
                                           i%afterColumnsNumber);
        }
    }
}

void Widget::showEvent(QShowEvent *)
{
    // Here is for use of resizeEvent() when program is going to start.
    resize(size()-QSize(1,0));
    resize(size()+QSize(1,0));
}

/*
  private slots
  */
void Widget::onTemporaryTokenReceived(QString token, QString tokenSecret)
{
    if( _oauthManager->lastError() != KQOAuthManager::NoError) {
        return;
    }
    qDebug() << "Opening authorization web site: " << QUrl("https://api.twitter.com/oauth/authorize");
    _oauthManager->getUserAuthorization(QUrl("https://api.twitter.com/oauth/authorize"));
}

void Widget::onAuthorizationReceived(QString token, QString verifier)
{
    qDebug() << "User authorization received: " << token << verifier;

    _oauthManager->getUserAccessTokens(QUrl(tr("https://api.twitter.com/oauth/access_token")));
    if (_oauthManager->lastError() != KQOAuthManager::NoError) {
        showOAuthError(_oauthManager->lastError());
        //QCoreApplication::exit();
    }
}

void Widget::onAccessTokenReceived(QString token, QString tokenSecret)
{
    qDebug() << "Access token received: " << token << tokenSecret;

    _oauthSettings->setValue(tr("oauth_token"), token);
    _oauthSettings->setValue(tr("oauth_token_secret"), tokenSecret);

    qDebug() << "Access tokens now stored. "
                "You are ready to send Tweets from user's account!";

    this->initializeOwnPostsWidgets();
    //QCoreApplication::exit();
}

void Widget::onAuthorizedRequestDone() {
    qDebug() << "Request sent to Twitter!";
    //QCoreApplication::exit(0);
}

void Widget::onRequestReady(QByteArray response)
{
    if(response.size()<=0){
        showOAuthError(_oauthManager->lastError());
        return;
    }
    qDebug() << "Response from the service: " << response;
}

void Widget::on_authorizationButton_clicked() {
    qDebug() << "Attempting new consumer key and consumer secret.";

    _subject->getReplyOfUserStreamsAPI()->disconnect(this);
    _subject->getReplyOfUserStreamsAPI()->disconnect(SIGNAL(error(QNetworkReply::NetworkError)));

    this->setConsumerKeyAndConsumerSecretSettings();
    this->setConsumerKeyAndConsumerSecret();
    ui->lineEdit->clear();
    _oauthSettings->clear();
    ui->lineEdit->setEnabled(true);
    ui->verificationButton->setEnabled(true);

    delete _oauthManager;
    _oauthManager = new KQOAuthManager();
    this->initializeKQOAuth();
    this->initializeOwnPostsWidgets();

    // kQOAuth Slots
    connect(_oauthManager, SIGNAL(temporaryTokenReceived(QString,QString)),
            this, SLOT(onTemporaryTokenReceived(QString, QString)), Qt::UniqueConnection);

    connect(_oauthManager, SIGNAL(authorizationReceived(QString,QString)),
            this, SLOT(onAuthorizationReceived(QString, QString)), Qt::UniqueConnection);

    connect(_oauthManager, SIGNAL(accessTokenReceived(QString,QString)),
            this, SLOT(onAccessTokenReceived(QString,QString)), Qt::UniqueConnection);

    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReady(QByteArray)), Qt::UniqueConnection);

    // Start OAuth Verification.
    this->initializeRequest(QUrl("https://api.twitter.com/oauth/request_token"),
                            KQOAuthRequest::POST,
                            KQOAuthRequest::TemporaryCredentials);

    _oauthManager->setHandleUserAuthorization(true);
    //_oauthManager->setHandleUserAuthorization(false);

    _oauthManager->executeRequest(_oauthRequest);

    // Checking Error
    if (_oauthManager->lastError() != KQOAuthManager::NoError) {
        qDebug("authorization error");
        showOAuthError(_oauthManager->lastError());
    }
}

void Widget::on_showTimeLineButton_clicked() {
    disconnect(_oauthManager->networkManager(), SIGNAL(finished(QNetworkReply*)),
               this, SLOT(on_showTimeLineButton_clicked()));

    if( this->isOAuthed() == false){
        qDebug() << "Not Verified Yet.";
        return;
    }
    qDebug("Attempting reading TimeLine.");

    ui->listWidget->clear();

    //QUrl("https://api.twitter.com/1/statuses/home_timeline.json");
    initializeRequest(QUrl("https://userstream.twitter.com/2/user.json"),
                      KQOAuthRequest::GET);
    setTokenAndSecretTokenToRequest();

    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReadyTimeline(QByteArray)), Qt::UniqueConnection);

    //    connect(_oauthManager, SIGNAL(authorizedRequestDone()),
    //            this, SLOT(onAuthorizedRequestDone()));

    //_oauthManager->executeRequest(_oauthRequest);
    this->SSLconnect(_oauthRequest);

    if (_oauthManager->lastError() != KQOAuthManager::NoError) {
        qDebug("Showing timeline error.");
        showOAuthError(_oauthManager->lastError());
    }
}

void Widget::onRequestReadyTimeline(QByteArray response) {
    qDebug("Reading TimeLine");
    // For JSON
    qDebug("This is JSON format.");
    if(response.startsWith("{\"friends\":[")) { // User Streams
        response = QString(response).remove(QRegExp("(\\{\"friends\":\\[[\\d,]+\\]\\}|\\r|\\n)")).toUtf8();
    }
    value value;
    string error;
    parse(value, response.data(), response.data() + response.size(), &error);
    if (!error.empty()){
        cerr << error << endl;
        return;
    }
    object jsonObject;
    if(value.to_str() == string("array")){
        array jsonArray = value.get<array>();
        array::iterator it;
        ui->listWidget->clear();
        for (it = jsonArray.begin(); it != jsonArray.end(); ++it)
        {
            jsonObject = it->get<object>();
            qDebug() << QString::fromStdString(jsonObject["text"].to_str());
            if(!QString::fromStdString(jsonObject["text"].to_str()).contains("#commando")){
                ui->listWidget->addItem(QString::fromStdString(jsonObject["text"].to_str()));
            }
        }
    }
    else if(value.to_str() == string("object")){
        jsonObject = value.get<object>();
        qDebug() << QString::fromStdString(jsonObject["text"].to_str());
        if(QString::fromStdString(jsonObject["text"].to_str()).contains("#commando")){
            ui->listWidget->addItem(QString::fromStdString(jsonObject["text"].to_str()));
        }
    }
    ui->listWidget->scrollToBottom();
    /*
            qDebug() <<
                        ["contributors"] <<
                        "\n" <<
                        ["coordinates"] <<
                        "\n" <<
                        ["created_at"].toString() <<
                        "\n" <<
                        ["geo"]<<
                        "\n" <<
                        ["id"].toLongLong() <<
                        "\n" <<
                        ["id_str"].toString() <<
                        "\n" <<
                        ["in_reply_to_screen_name"] <<
                        "\n" <<
                        ["in_reply_to_status_id"]<<
                        "\n" <<
                        ["in_reply_to_status_id"] <<
                        "\n" <<
                        ["in_reply_to_user_id"] <<
                        "\n" <<
                        ["in_reply_to_user_id_str"] <<
                        "\n" <<
                        ["place"] <<
                        "\n" <<
                        ["possibly_sensitive"].toBool() <<
                        "\n" <<
                        ["retweet_count"].toULongLong() <<
                        "\n" <<
                        ["retweeted"].toBool() <<
                        "\n" <<
                        ["text"].toString() <<
                        "\n" <<
                        ["truncated"].toBool() <<
                        "\n" <<
                        ["user"]["contributors_enabled"].toBool() <<
                        "\n" <<
                        ["user"]["created_at"].toString() <<
                        "\n" <<
                        ["user"]["default_profile"].toBool() <<
                        "\n" <<
                        ["user"]["default_profile_image"].toBool() <<
                        "\n" <<
                        ["user"]["description"].toString() <<
                        "\n" <<
                        ["user"]["favourites_count"].toULongLong() <<
                        "\n" <<
                        ["user"]["followers_count"].toULongLong() <<
                        "\n" <<
                        ["user"]["following"].toBool() <<
                        "\n" <<
                        ["user"]["friends_count"].toULongLong() <<
                        "\n" <<
                        ["user"]["geo_enabled"].toBool() <<
                        "\n" <<
                        ["user"]["id"].toULongLong() <<
                        "\n" <<
                        ["user"]["id_str"].toString() <<
                        "\n" <<
                        ["user"]["is_translator"].toBool() <<
                        "\n" <<
                        ["user"]["lang"].toString() <<
                        "\n" <<
                        ["user"]["listed_count"].toULongLong() <<
                        "\n" <<
                        ["user"]["location"].toString() <<
                        "\n" <<
                        ["user"]["profile_background_color"].toString() <<
                        "\n" <<
                        ["user"]["profile_background_image_url"].toString() <<
                        "\n" <<
                        ["user"]["profile_background_image_url_https"].toString() <<
                        "\n" <<
                        ["user"]["profile_background_tile"].toBool() <<
                        "\n" <<
                        ["user"]["profile_image_url_https"].toString() <<
                        "\n" <<
                        ["user"]["profile_sidebar_border_color"].toString() <<
                        "\n" <<
                        ["user"]["profile_sidebar_fill_color"].toString() <<
                        "\n" <<
                        ["user"]["profile_text_color"].toString() <<
                        "\n" <<
                        ["user"]["profile_use_background_image"].toString() <<
                        "\n" <<
                        ["user"]["protected"].toBool() <<
                        "\n" <<
                        ["user"]["screen_name"].toBool() <<
                        "\n" <<
                        ["user"]["show_all_inline_media"].toString() <<
                        "\n" <<
                        ["user"]["statuses_count"].toULongLong() <<
                        "\n" <<
                        ["user"]["time_zone"].toString() <<
                        "\n" <<
                        ["user"]["url"].toString() <<
                        "\n" <<
                        ["user"]["utc_offset"].toULongLong() <<
                        "\n" <<
                        ["user"]["verified"].toBool() <<
                        "\n\n";
*/
}

void Widget::onRequestReadyOwnPosts(QByteArray response) {
    qDebug() << "Request ready own posts";
    // For JSON
    qDebug("This is JSON format.");
    value valueA;
    string error;
    parse(valueA, response.data(), response.data() + response.size(), &error);
    if (!error.empty()){
        qDebug() << response;
        cerr << error << endl;
        return;
    }
    object jsonObject;
    if(valueA.to_str() == string("array")){
        int tweetNumberBeforeNow=0;
        QHash<QString, int> monthsHash;
        monthsHash.insert("Jan", 1);
        monthsHash.insert("Feb", 2);
        monthsHash.insert("Mar", 3);
        monthsHash.insert("Apr", 4);
        monthsHash.insert("May", 5);
        monthsHash.insert("Jun", 6);
        monthsHash.insert("Jul", 7);
        monthsHash.insert("Aug", 8);
        monthsHash.insert("Sep", 9);
        monthsHash.insert("Oct", 10);
        monthsHash.insert("Nov", 11);
        monthsHash.insert("Dec", 12);

        for(array::iterator it = valueA.get<array>().begin(); it != valueA.get<array>().end(); ++it){
            jsonObject = it->get<object>();
            //latest 130
            // When latest 127 posts are in 3 hours, a new post is forbidden.
            // as "Sat Feb 25 15:55:23 +0000 2012"
            QStringList dateTimeElements = QString::fromStdString(jsonObject["created_at"].to_str()).split(" ");
            QStringList timeElements = dateTimeElements.at(3).split(":");
            QDateTime postedDateTime = QDateTime(QDate(dateTimeElements.at(5).toInt(),
                                                       monthsHash.value(dateTimeElements.at(1)),
                                                       dateTimeElements.at(2).toInt()
                                                       ),
                                                 QTime(timeElements.at(0).toInt(),
                                                       timeElements.at(1).toInt(),
                                                       timeElements.at(2).toInt()
                                                       ),
                                                 Qt::UTC
                                                 ).toTimeSpec(Qt::LocalTime);
            _latestTweetsForTime.appendPostedDateTimeList(postedDateTime);

            ui->tableWidget_2->setItem(
                        LATEST_NUMBER_FOR_TIME - tweetNumberBeforeNow - 1, 0,
                        new QTableWidgetItem(
                            postedDateTime.toString("dd,  hh:mm:ss")
                            )
                        );

            //latest 10
            if(tweetNumberBeforeNow<10){
                ui->tableWidget->setItem(10-tweetNumberBeforeNow-1, 0, new QTableWidgetItem(QString::fromStdString(jsonObject["text"].to_str()))); //posted contents
            }
            ++tweetNumberBeforeNow;
        }
    }
    else if(valueA.to_str() == string("object")){
        jsonObject = valueA.get<object>();
        qDebug() << QString::fromStdString(jsonObject["text"].to_str());
        if(QString::fromStdString(jsonObject["text"].to_str()).contains("#commando")){
            ui->listWidget->addItem(QString::fromStdString(jsonObject["text"].to_str()));
        }
    }
}

void Widget::sendTweet(QString tweet) {

    if( this->isOAuthed() == false) {
        qDebug() << "No access tokens. Aborting.";
        return;
    }
    if(tweet.size()==0)
    {
        qDebug() << "There are no characters.";
        return;
    }

    tweet.replace(QRegExp("\\(\\d+:([^()]+)\\)"), "\\1");
    tweet.append(" #commando");

    initializeRequest(QUrl("https://api.twitter.com/1/statuses/update.xml"));
    setTokenAndSecretTokenToRequest();
    KQOAuthParameters params;
    params.insert("status", tweet);
    _oauthRequest->setAdditionalParameters(params);

    disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)),0,0);

    qDebug() << "Attempting send tweet such as " << tweet;
    _oauthManager->executeRequest(_oauthRequest);

    ui->tableWidget->removeRow(0);
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(9,0,new QTableWidgetItem(tweet));

    ui->tableWidget_2->removeRow(0);
    ui->tableWidget_2->setRowCount(ui->tableWidget_2->rowCount()+1);
    ui->tableWidget_2->setItem(129,0, new QTableWidgetItem( (QDateTime::currentDateTime().toString("dd hh:mm:ss"))));
    //    connect(_oauthManager, SIGNAL(authorizedRequestReady()),
    //         this, SLOT(onAuthorizedRequestReady()));

    // LineEdits must clear contents.
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
}

void Widget::showOAuthError(const int nErrNum)
{
    qDebug() << "error is " << nErrNum;
}

void Widget::on_verificationButton_clicked()
{
    ui->lineEdit->setEnabled(false);
    ui->verificationButton->setEnabled(false);
    _oauthSettings->setValue("number", ui->lineEdit->text());
    _oauthManager->onVerificationReceived1(_oauthSettings->value("number").toString());
}

void Widget::on_postButton_clicked()
{
    QString tweet = ui->plainTextEdit->toPlainText();
    sendTweet(tweet);
}

void Widget::on_lineEdit_2_textChanged(const QString &arg1)
{
    replacePlainTextEditContent(1, arg1);
}

void Widget::on_lineEdit_3_textChanged(const QString &arg1)
{
    replacePlainTextEditContent(1, arg1);
}

void Widget::on_lineEdit_4_textChanged(const QString &arg1)
{
    replacePlainTextEditContent(1, arg1);
}

void Widget::on_lineEdit_5_textChanged(const QString &arg1)
{
    replacePlainTextEditContent(1, arg1);
}

void Widget::on_lineEdit_6_textChanged(const QString &arg1)
{
    replacePlainTextEditContent(1, arg1);
}

bool Widget::replacePlainTextEditContent(int i, QString str)
{
    if(str.length()>0)
    {
        QString text = ui->plainTextEdit->toPlainText();
        text.replace(QRegExp("(\\(" + QString::number(i) + ":)[^()]+(\\))"), "\\1" + str + "\\2");
        ui->plainTextEdit->setPlainText(text);
    }
    return true;
}

bool Widget::SetButtonsEnable(bool on)
{
    for(int i=0;i<ui->buttonsGridLayout->count();++i)
    {
        if(ui->buttonsGridLayout->itemAt(i)->widget()->isEnabled()== (!on))
        {
            ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(on);
        }
    }
    return true;
}

void Widget::on_proxyButton_clicked()
{
    _proxy->setHostName(ui->proxyHostNameLineEdit->text());
    _proxy->setPort(ui->proxyPortLineEdit->text().toInt());
    _proxy->setUser(ui->proxyUserLineEdit->text());
    _proxy->setUser(ui->proxyPasswordLineEdit->text());

    _sizeSettings->beginGroup("proxy");
    _sizeSettings->setValue("hostName", ui->proxyHostNameLineEdit->text());
    _sizeSettings->setValue("port", ui->proxyPortLineEdit->text());
    _sizeSettings->setValue("user", ui->proxyUserLineEdit->text());
    _sizeSettings->setValue("password", ui->proxyPasswordLineEdit->text());
    _sizeSettings->endGroup();
}
