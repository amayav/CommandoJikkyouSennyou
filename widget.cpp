#include "widget.h"
#include "ui_widget.h"
#include <QTextCodec>
#include <QDir>
#include <QDebug>
#include <QtXml/QDomDocument>
#include <QUrl>
#include <QSettings>
#include <QKeyEvent>
#include "extrapushbutton.h"
#include <cmath>
#include <QLayoutItem>
#include <QSslCertificate>
#include <QSslSocket>
#include <QSslCipher>
#include <QHostAddress>
#include <QSslConfiguration>
#include <QtAlgorithms>
#include <qjson-qjson/src/parser.h>
#include <QDateTime>
#include <QShortcut>
/*
  publidc functions
  */
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget){
    //-------------------------
    // Qt特有のUTF-8設定
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    //-------------------------

    ui->setupUi(this);

    QString oauthIniName = qApp->applicationDirPath()
            + "/verification.ini";
    _oauthSettings = new QSettings(oauthIniName, QSettings::IniFormat);
    _oauthSettings->setIniCodec("utf-8");
    //-------------------------
    // kQAOuthの初期化
    _oauthRequest = new KQOAuthRequest;        // リクエスト用オブジェクト
    _oauthManager = new KQOAuthManager(this);  // マネージャオブジェクト

    _oauthManager->setNetworkManager(new QNetworkAccessManager);

    connect(_oauthManager, SIGNAL(authorizedRequestDone()),
            this, SLOT(onAuthorizedRequestDone()));

    _oauthRequest->setEnableDebugOutput(true); // デバッグ出力ON
    //  _oauthRequest->setEnableDebugOutput(false); // デバッグ出力OFF
    //-------------------------

    if (_oauthSettings->childKeys().size() == 3) {
        // 既に認証キー取得済みならボタン２を有効にする
        ui->pushButton_2->setEnabled(true);

        ui->lineEdit->setText(_oauthSettings->value("number").toString());
    }

    QString shortCutKeysIniName = qApp->applicationDirPath()
            + "/key.txt";
    _shortCutKeysSettings = new QSettings(shortCutKeysIniName, QSettings::IniFormat);
    _shortCutKeysSettings->setIniCodec("utf-8");
    QStringList keyKeys = _shortCutKeysSettings->childKeys();
    _keysHash = new QHash<QString,QString>();
    for(int i=0;i<_shortCutKeysSettings->childKeys().size();++i)
    {
        _keysHash->insert(keyKeys.at(i), _shortCutKeysSettings->value(keyKeys.at(i)).toString());
        ExtraPushButton *p = new ExtraPushButton(keyKeys.at(i),
                                                 _shortCutKeysSettings->value(keyKeys.at(i)).toString(),
                                                 this);
        p->setToolTip(_shortCutKeysSettings->value(keyKeys.at(i)).toString());
        ui->buttonsGridLayout->addWidget(p,0,i);
        // Succession of slots can not go well.
        connect(p, SIGNAL(clicked()), p, SLOT(click()));
        connect(p, SIGNAL(clicked(QString)), this, SLOT(sendTweet(QString)));
        connect(p, SIGNAL(showingToolTip(QString)), ui->plainTextEdit, SLOT(setPlainText(QString)));
    }

    QString sizeIniName = qApp->applicationDirPath()
            + "/size.ini";
    _sizeSettings = new QSettings(sizeIniName, QSettings::IniFormat);
    if(_sizeSettings->allKeys().count()==2)
    {
        resize(_sizeSettings->value("width").toInt(), _sizeSettings->value("height").toInt());
    }
    else
    {
        resize(200,200);
    }

    // Short Cut Key for Post.
    QShortcut *post1 = new QShortcut(ui->lineEdit_2);
    QShortcut *post2 = new QShortcut(ui->lineEdit_3);
    QShortcut *post3 = new QShortcut(ui->lineEdit_4);
    QShortcut *post4 = new QShortcut(ui->lineEdit_5);
    QShortcut *post5 = new QShortcut(ui->lineEdit_6);
    QList<QShortcut*> postShortCutList;
    postShortCutList << post1 << post2 << post3 << post4 << post5;
    for(int i=0;i<postShortCutList.size();++i)
    {
        this->setShortCutKeyAndContext(postShortCutList.at(i), QKeyStandard(Qt::CTRL + Qt::Key_Return), Qt::WidgetShortcut);
    }

    connect(post1, SIGNAL(activated()), ui->pushButton_4, SLOT(click()));
    connect(post2, SIGNAL(activated()), ui->pushButton_4, SLOT(click()));
    connect(post3, SIGNAL(activated()), ui->pushButton_4, SLOT(click()));
    connect(post4, SIGNAL(activated()), ui->pushButton_4, SLOT(click()));
    connect(post5, SIGNAL(activated()), ui->pushButton_4, SLOT(click()));

    //initializeOwnPostsWidgets();
    /*   QList<QSslCertificate> cList;

    QFile f1(qApp->applicationDirPath() + "\\" + "v.cer");
    f1.open(QIODevice::ReadOnly);
    QSslCertificate sc(f1.readAll());
    f1.close();

    QFile f2(qApp->applicationDirPath() + "\\"  + "veri.cer");
    f2.open(QIODevice::ReadOnly);
    QSslCertificate sr(f2.readAll());
    f2.close();

    QFile f3(qApp->applicationDirPath() + "\\"  + "vt.cer");
    f3.open(QIODevice::ReadOnly);
    QSslCertificate st(f3.readAll());
    f3.close();

    QFile f4(qApp->applicationDirPath() + "\\"  + "vtt.cer");
    f4.open(QIODevice::ReadOnly);
    QSslCertificate stt(f4.readAll());
    f4.close();

    cList << sc << sr << st <<stt;
    sock = new QSslSocket;
    connect(sock, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(receive(QList<QSslError>)));
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(cError(QAbstractSocket::SocketError)));
    connect(sock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(cState(QAbstractSocket::SocketState)));
    sock->setProtocol(QSsl::TlsV1);
    sock->setPeerVerifyMode(QSslSocket::VerifyPeer);
    sock->setPeerVerifyDepth(5);
    sock->setCaCertificates(cList);
    qDebug() << "[CLIENT]:  Attempting Connection...";
    //sock->connectToHostEncrypted("userstream.twitter.com/2/user.json", 443);
    sock->connectToHostEncrypted("userstream.twitter.com", 443);
    //sock->connectToHostEncrypted("api.twitter.com", 443);

    if(sock->waitForConnected())
    {
        qDebug("Connected");
    }
    else
    {
        qDebug("Disconnected");
        return;
    }
    if(sock->waitForEncrypted())
    {
        qDebug("Encrypted");
    }
    else
    {
        qDebug("Discrypted");
        return;
    }
    if(sock->open(QIODevice::ReadWrite)==false)
    {
        qDebug("Can not open.");
        return;
    }

    qDebug("Writing");
    if(false)
    {
        //QUrl url("https://api.twitter.com/1/statuses/home_timeline.json");
        //QUrl url("https://userstream.twitter.com/2/user.json");
        QUrl url("http://stream.twitter.com/1/filter.json");
        _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, url);
        _oauthRequest->setHttpMethod(KQOAuthRequest::GET); // GET指定
        _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
        _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
        _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
        _oauthRequest->setTokenSecret(
                    _oauthSettings->value("oauth_token_secret").toString());

        connect(sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
        disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)));
        connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
                this, SLOT(onRequestReadyTimeline(QByteArray)));
        connect(_oauthManager, SIGNAL(authorizedRequestDone()),
                this, SLOT(onAuthorizedRequestDone()));
        //    _oauthManager->executeRequest(_oauthRequest);
    }
    else if(false)
    {
        QUrl url("https://api.twitter.com/1/statuses/update.xml");
        _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest,url);
        _oauthRequest->setHttpMethod(KQOAuthRequest::POST); // POST指定
        _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
        _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
        _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
        _oauthRequest->setTokenSecret(_oauthSettings->value("oauth_token_secret").toString());

        QString tweet("UserStreamsAPIさっぱり通らぬワロタ。");
        KQOAuthParameters params;
        params.insert("status", tweet);
        _oauthRequest->setAdditionalParameters(params);
        //    _oauthManager->executeRequest(_oauthRequest);
    }
    else if(false)
    {
        QUrl url("http://stream.twitter.com/1/statuses/filter.json");
        _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest,url);
        _oauthRequest->setHttpMethod(KQOAuthRequest::POST); // POST指定
        _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
        _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
        _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
        _oauthRequest->setTokenSecret(_oauthSettings->value("oauth_token_secret").toString());

        KQOAuthParameters params;
        params.insert("follow", "2062439");
        _oauthRequest->setAdditionalParameters(params);

        connect(sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
        disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)));
        connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
                this, SLOT(onRequestReadyTimeline(QByteArray)));
        connect(_oauthManager, SIGNAL(authorizedRequestDone()),
                this, SLOT(onAuthorizedRequestDone()));
        //    _oauthManager->executeRequest(_oauthRequest);
    }
    else
    {
        QUrl url("http://stream.twitter.com/1/sample.json");
        _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, url);
        _oauthRequest->setHttpMethod(KQOAuthRequest::GET); // GET指定
        _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
        _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
        _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
        _oauthRequest->setTokenSecret(
                    _oauthSettings->value("oauth_token_secret").toString());

        connect(sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
        disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)));
        connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
                this, SLOT(onRequestReadyTimeline(QByteArray)));
        connect(_oauthManager, SIGNAL(authorizedRequestDone()),
                this, SLOT(onAuthorizedRequestDone()));
        //    _oauthManager->executeRequest(_oauthRequest);
    }
    QNetworkRequest networkRequest;
    networkRequest.setUrl( _oauthRequest->requestEndpoint() );

    QString serverString = "oob"; //To use client, "oauth_callback" value must be oob.
    _oauthRequest->setCallbackUrl(QUrl(serverString));

    // And now fill the request with "Authorization" header data.
    QList<QByteArray> requestHeaders = _oauthRequest->requestParameters();
    QByteArray authHeader;
    bool first = true;
    foreach (const QByteArray header, requestHeaders) {
        if (!first) {
            authHeader.append(", ");
        } else {
            authHeader.append("OAuth ");
            first = false;
        }

        authHeader.append(header);
    }
    networkRequest.setRawHeader("Authorization", authHeader);
    //connect(d->networkManager, SIGNAL(finished(QNetworkReply *)),
    //this, SLOT(onRequestReplyReceived(QNetworkReply *)));
    connect(_oauthManager->networkManager(), SIGNAL(finished(QNetworkReply *)),
            _oauthManager, SLOT(onRequestReplyReceived(QNetworkReply *)));

    networkRequest.setSslConfiguration(sock->sslConfiguration());

    if (_oauthRequest->httpMethod() == KQOAuthRequest::GET) {
        // Get the requested additional params as a list of pairs we can give QUrl
        QList< QPair<QString, QString> > urlParams;

        QList<QString> requestKeys = _oauthRequest->additionalParameters().keys();
        QList<QString> requestValues = _oauthRequest->additionalParameters().values();

        for(int i=0; i<requestKeys.size(); i++) {
            urlParams.append( qMakePair(requestKeys.at(i),
                                        requestValues.at(i))
                              );
        }
        // Take the original URL and append the query params to it.
        QUrl urlWithParams = networkRequest.url();
        urlWithParams.setQueryItems(urlParams);
        networkRequest.setUrl(urlWithParams);

        qDebug() << networkRequest.rawHeader("Authorization");

        // Submit the request including the params.
        //QNetworkReply *reply = d->networkManager->get(networkRequest);
        //QNetworkReply *reply = _oauthManager->networkManager()->get(networkRequest);
        //qDebug() << reply->readAll();

        //QByteArray array("GET /1/statuses/home_timeline.json");
        //QByteArray array("GET /2/user.json");
        QByteArray array("GET /1/statuses/sample.json");
        array.append("\r\nAuthorization: " + authHeader);
        //        array.append("HTTP/1.1\r\n");
        //        .append("Accept: *\/\* \r\n")
        //                .append("Accept-Language: ja\r\n")
        //                .append("If-Modified-Since: Mon,04 Dec 2000 00:11:45 GMT; length=35\r\n")
        //                .append("User-Agent: Mozilla/4.0(compatible; MSIE 5.5; Windows NT 5.0)\r\n");
        //        array.append("Host: ").append("api.twitter.com").append("\r\n");
        //        array.append("Host: ").append("userstream.twitter.com").append("\r\n");
        //        .append("Connection: Keep-Alive");
        qDebug() << array;
        sock->write(array);

//        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
//                this, SLOT(slotError(QNetworkReply::NetworkError)));
    }
    else if (_oauthRequest->httpMethod() == KQOAuthRequest::POST) {

        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, _oauthRequest->contentType());

        qDebug() << networkRequest.rawHeaderList();
        qDebug() << networkRequest.rawHeader("Authorization");
        qDebug() << networkRequest.rawHeader("Content-Type");

        QNetworkReply *reply;
        if (_oauthRequest->contentType() == "application/x-www-form-urlencoded") {
            //reply = d->networkManager->post(networkRequest, _oauthRequest->requestBody());
            reply = _oauthManager->networkManager()->post(networkRequest, _oauthRequest->requestBody());
            qDebug() << "pp" << _oauthRequest->requestBody() << reply->readAll();
        } else {
            //reply = d->networkManager->post(networkRequest, _oauthRequest->rawData());
            reply = _oauthManager->networkManager()->post(networkRequest, _oauthRequest->rawData());
            qDebug() << "QQ" << _oauthRequest->rawData();
        }

        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(slotError(QNetworkReply::NetworkError)));
    }

*/
    //        sock->disconnectFromHost();
    //    }
}

bool Widget::setShortCutKeyAndContext(QShortcut *sc, QKeySequence s, Qt::ShortcutContext c)
{
    sc->setKey(s);
    sc->setContext(c);
}

bool Widget::initializeOwnPostsWidgets()
{
    QUrl url("https://api.twitter.com/1/statuses/user_timeline.json");
    _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, url);
    _oauthRequest->setHttpMethod(KQOAuthRequest::GET); // GET指定
    _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
    _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
    _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
    _oauthRequest->setTokenSecret(
                _oauthSettings->value("oauth_token_secret").toString());

    KQOAuthParameters params;
    params.insert("count", "131");
    _oauthRequest->setAdditionalParameters(params);


    qDebug() << _oauthSettings->value("oauth_token").toString();
    qDebug() << _oauthSettings->value("oauth_token_secret").toString();

    disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)));
    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReadyOwnPosts(QByteArray)));

    connect(_oauthManager, SIGNAL(authorizedRequestDone()),
            this, SLOT(onAuthorizedRequestDone()));

    _oauthManager->executeRequest(_oauthRequest);

    return true;
}

void Widget::slotError(QNetworkReply::NetworkError e)
{
    qDebug() << "1" << e;
}

void Widget::cState(QAbstractSocket::SocketState s)
{
    qDebug() << "2" << s;
}

void Widget::readyRead()
{
    qDebug() << "3";
    qDebug() << sock->readAll();
}

void Widget::cError(QAbstractSocket::SocketError e)
{
    qDebug() << "oi" << e;
    if(e == QAbstractSocket::SslHandshakeFailedError)
    {
        qDebug("@@@@");
    }
}

void Widget::receive(QList<QSslError> e)
{
    for(int i=0;i<e.size();++i)
    {
        qDebug() << "ll" << i << e.at(i).errorString();
    }
}

Widget::~Widget() {
    //-------------------------
    // kQAOuthオブジェクトの破棄
    delete _oauthRequest;
    delete _oauthManager;
    //-------------------------
    delete _oauthSettings;

    delete ui;
}

/*
  protected functions
  */
void Widget::keyPressEvent(QKeyEvent *ke)
{
    if(ke->isAutoRepeat())
    {
        return;
    }
    setAttribute(Qt::WA_KeyCompression);
    QString key;
    if( (ke->modifiers()==Qt::ControlModifier) && (ke->key()!=Qt::Key_Control) )
    {
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
        if(ke->text().toAscii().toHex().toInt(0, 16) <= 31) //31 is 0x1f
        {
            key = QByteArray::fromHex(QByteArray::number(ke->text().toAscii().toHex().toInt(0, 16) + 64, 16));
        }
        else
        {
            key = ke->text();
        }
        _keyString.append(key.toLower());
        qDebug() << ke->text() << _keyString << ke->count();
        for(int i=0;i<ui->buttonsGridLayout->count();++i)
        {
            ExtraPushButton *p = dynamic_cast<ExtraPushButton*>(ui->buttonsGridLayout->itemAt(i)->widget());
            if(p->getShortCutKey().indexOf(_keyString)==-1)
            {
                ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(false);
            }
        }
    }
}

void Widget::keyReleaseEvent(QKeyEvent *ke)
{
    if(ke->isAutoRepeat())
    {
        return;
    }
    if( ( ke->key()==Qt::Key_Control) && (_keyString.size()>0) )
    {
        qDebug() << "ctrl!" << _keyString;
        QString tweet;
        int matchedKeysNumber = 0;
        QList<QString> list = _keysHash->keys();
        for(int i=0;i<list.size();++i)
        {
            if(list.at(i).indexOf(_keyString)==0)
            {
                // Match
                if(list.at(i).length() == _keyString.length())
                {
                    tweet = _keysHash->value(list.at(i));
                    matchedKeysNumber = -1;
                    break;
                }
                ++matchedKeysNumber;
                tweet = _keysHash->value(list.at(i));
            }
        }

        if(matchedKeysNumber>1)
        {
            tweet.clear();
        }
        _keyString.clear();
        if(tweet.contains(QRegExp("\\(\\d.*\\)")))
        {
            ui->plainTextEdit->setPlainText(tweet);
            for(int i=0;i<ui->buttonsGridLayout->count();++i)
            {
                if(ui->buttonsGridLayout->itemAt(i)->widget()->isEnabled()==true)
                {
                    ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(false);
                }
            }
            ui->lineEdit_2->setFocus();
        }
        else
        {
            for(int i=0;i<ui->buttonsGridLayout->count();++i)
            {
                if(ui->buttonsGridLayout->itemAt(i)->widget()->isEnabled()==false)
                {
                    ui->buttonsGridLayout->itemAt(i)->widget()->setEnabled(true);
                }
            }
            if(tweet.size()==0)
            {
                qDebug() << "There is not such a short cut key.";
                return;
            }
            sendTweet(tweet);
        }
        tweet.clear();
    }
}

void Widget::resizeEvent(QResizeEvent *)
{
    _sizeSettings->setValue("width", width());
    _sizeSettings->setValue("height", height());
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
    if(beforeColumnsNumber==0)
    {
        return;
    }

    QRect r;
    for(int i=beforeColumnsNumber-1;i>=0;--i)
    {
        if(ui->buttonsGridLayout->itemAtPosition(0, i)!=NULL)
        {
            r = ui->buttonsGridLayout->itemAtPosition(0, i)->widget()->frameGeometry();
            break;
        }
    }

    // When widget size becomes too small.
    if(r.x()+r.width() > ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width())
    {
        int afterColumnsNumber = 0;
        for(int i=beforeColumnsNumber-1;i>=0;--i)
        {
            r = ui->buttonsGridLayout->itemAtPosition(0,i)->widget()->frameGeometry();
            if(r.x()+r.width()
                    < ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width())
            {
                afterColumnsNumber = i+1;
                break;
            }
        }
        if(afterColumnsNumber==0)
        {
            return;
        }
        QList<QLayoutItem*> *list = new QList<QLayoutItem*>();
        while(ui->buttonsGridLayout->itemAt(0)!=NULL)
        {
            list->append(ui->buttonsGridLayout->itemAt(0)); // There are only extraPushButtons.
            ui->buttonsGridLayout->removeItem(ui->buttonsGridLayout->itemAt(0));
        }
        for(int i=0;i<list->size();++i)
        {
            ui->buttonsGridLayout->addItem(list->at(i),
                                           floor((double)i/(double)afterColumnsNumber),
                                           i%afterColumnsNumber);
        }
    }
    // When widget size becomes too big
    else if(r.x() + r.width() + ui->buttonsGridLayout->horizontalSpacing() + r.width()
            < ui->groupBox->x() + ui->groupBox->frameGeometry().width())
    {
        int afterColumnsNumber = 0;
        for(int i=1;i<200;++i)
        {
            if(r.x()+r.width() + i*(ui->buttonsGridLayout->horizontalSpacing() + ui->buttonsGridLayout->itemAt(0)->widget()->width())
                    > ui->groupBox->frameGeometry().x()+ui->groupBox->frameGeometry().width())
            {
                afterColumnsNumber = beforeColumnsNumber+i;
                break;
            }
        }
        if(afterColumnsNumber > ui->buttonsGridLayout->count() || afterColumnsNumber==0)
        {
            return;
        }
        QList<QLayoutItem*> *list = new QList<QLayoutItem*>();
        while(ui->buttonsGridLayout->itemAt(0)!=NULL)
        {
            list->append(ui->buttonsGridLayout->itemAt(0)); // There are only extraPushButtons.
            ui->buttonsGridLayout->removeItem(ui->buttonsGridLayout->itemAt(0));
        }
        for(int i=0;i<list->size();++i)
        {
            ui->buttonsGridLayout->addItem(list->at(i),
                                           floor((double)i/(double)afterColumnsNumber),
                                           i%afterColumnsNumber);
        }
    }
    else
    {
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
    QUrl userAuthURL("https://api.twitter.com/oauth/authorize");

    if( _oauthManager->lastError() == KQOAuthManager::NoError) {
        qDebug() << "Opening authorization web site: " << userAuthURL;
        _oauthManager->getUserAuthorization(userAuthURL);
    }
}

void Widget::onAuthorizationReceived(QString token, QString verifier)
{
    qDebug() << "User authorization received: " << token << verifier;

    _oauthManager->getUserAccessTokens(
                QUrl(tr("https://api.twitter.com/oauth/access_token")));
    if (_oauthManager->lastError() != KQOAuthManager::NoError) {
        QCoreApplication::exit();
    }
}

void Widget::onAccessTokenReceived(QString token, QString tokenSecret)
{
    qDebug() << "Access token received: " << token << tokenSecret;

    _oauthSettings->setValue(tr("oauth_token"), token);
    _oauthSettings->setValue(tr("oauth_token_secret"), tokenSecret);

    qDebug() << "Access tokens now stored. "
                "You are ready to send Tweets from user's account!";

    //QCoreApplication::exit();
}

void Widget::onAuthorizedRequestDone() {
    qDebug() << "Request sent to Twitter!";
    //QCoreApplication::exit(0);
}

void Widget::onRequestReady(QByteArray response)
{
    qDebug() << "Response from the service: " << response;
}

void Widget::on_pushButton_clicked() {
    ui->lineEdit->setEnabled(true);
    ui->pushButton_3->setEnabled(true);

    //--------------------------
    // kQOAuthのスロットの登録
    connect(_oauthManager, SIGNAL(temporaryTokenReceived(QString,QString)),
            this, SLOT(onTemporaryTokenReceived(QString, QString)));

    connect(_oauthManager, SIGNAL(authorizationReceived(QString,QString)),
            this, SLOT( onAuthorizationReceived(QString, QString)));

    connect(_oauthManager, SIGNAL(accessTokenReceived(QString,QString)),
            this, SLOT(onAccessTokenReceived(QString,QString)));

    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReady(QByteArray)));
    //--------------------------

    //----------------
    // OAuth認証実行

    // リクエストの初期化
    _oauthRequest->initRequest(KQOAuthRequest::TemporaryCredentials,
                               QUrl("https://api.twitter.com/oauth/request_token"));

    // コンシューマキー設定
    _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
    _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");

    // 認証処理のハンドリングを有効にする
    _oauthManager->setHandleUserAuthorization(true);
    //  _oauthManager->setHandleUserAuthorization(false);

    // 認証実行
    _oauthManager->executeRequest(_oauthRequest);

    // エラーチェック
    int e = _oauthManager->lastError();
    if (e != KQOAuthManager::NoError) {
        showOAuthError(e);
    }
    //----------------
}

void Widget::on_pushButton_2_clicked() {
    //ui->pushButton_2->setEnabled(false);
    ui->listWidget->clear();

    QUrl url("https://api.twitter.com/1/statuses/home_timeline.json");
    _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, url);
    _oauthRequest->setHttpMethod(KQOAuthRequest::GET); // GET指定
    _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
    _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
    _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
    _oauthRequest->setTokenSecret(
                _oauthSettings->value("oauth_token_secret").toString());

    qDebug() << _oauthSettings->value("oauth_token").toString();
    qDebug() << _oauthSettings->value("oauth_token_secret").toString();

    disconnect(_oauthManager, SIGNAL(requestReady(QByteArray)),0,0);
    connect(_oauthManager, SIGNAL(requestReady(QByteArray)),
            this, SLOT(onRequestReadyTimeline(QByteArray)));

    //    connect(_oauthManager, SIGNAL(authorizedRequestDone()),
    //            this, SLOT(onAuthorizedRequestDone()));

    _oauthManager->executeRequest(_oauthRequest);
}

void Widget::onRequestReadyTimeline(QByteArray response) {
    qDebug() << "requestReady-Timeline";
    QDomDocument dom;
    // For JSON
    if(dom.setContent(response)==false)
    {
        QJson::Parser parser;
        bool ok =false;
        QVariant top = parser.parse(response, &ok);
        if(!ok)
        {
            qDebug() << response.size() << response << "json error";
            return;
        }

        //QVariantMap result = top.toMap();
        QVariantList contentList = top.toList();

        foreach ( QVariant content, contentList ){
            /*
            qDebug() <<
                        content.toMap()["contributors"] <<
                        "\n" <<
                        content.toMap()["coordinates"] <<
                        "\n" <<
                        content.toMap()["created_at"].toString() <<
                        "\n" <<
                        content.toMap()["geo"]<<
                        "\n" <<
                        content.toMap()["id"].toLongLong() <<
                        "\n" <<
                        content.toMap()["id_str"].toString() <<
                        "\n" <<
                        content.toMap()["in_reply_to_screen_name"] <<
                        "\n" <<
                        content.toMap()["in_reply_to_status_id"]<<
                        "\n" <<
                        content.toMap()["in_reply_to_status_id"] <<
                        "\n" <<
                        content.toMap()["in_reply_to_user_id"] <<
                        "\n" <<
                        content.toMap()["in_reply_to_user_id_str"] <<
                        "\n" <<
                        content.toMap()["place"] <<
                        "\n" <<
                        content.toMap()["possibly_sensitive"].toBool() <<
                        "\n" <<
                        content.toMap()["retweet_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["retweeted"].toBool() <<
                        "\n" <<
                        content.toMap()["text"].toString() <<
                        "\n" <<
                        content.toMap()["truncated"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["contributors_enabled"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["created_at"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["default_profile"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["default_profile_image"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["description"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["favourites_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["followers_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["following"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["friends_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["geo_enabled"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["id"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["id_str"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["is_translator"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["lang"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["listed_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["location"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_background_color"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_background_image_url"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_background_image_url_https"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_background_tile"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_image_url_https"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_sidebar_border_color"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_sidebar_fill_color"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_text_color"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["profile_use_background_image"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["protected"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["screen_name"].toBool() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["show_all_inline_media"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["statuses_count"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["time_zone"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["url"].toString() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["utc_offset"].toULongLong() <<
                        "\n" <<
                        content.toMap()["user"].toMap()["verified"].toBool() <<
                        "\n\n";
*/
            ui->listWidget->addItem(content.toMap()["text"].toString());
        }
        return;
    }
    // for XML
    else
    {
        QDomElement root = dom.documentElement();
        QDomNode node;
        node = root.firstChild();
        while ( !node.isNull() )
        {
            if ( node.isElement() && (node.nodeName() == "status" ) )
            {
                QDomElement header = node.toElement();

                QDomNode node = header.firstChild();
                while ( !node.isNull() )
                {
                    if ( node.isElement() )
                    {
                        // いろいろなヘッダエントリの場合
                        if ( node.nodeName() == "text" )
                        {
                            QDomText textChild = node.firstChild().toText();
                            if ( !textChild.isNull() )
                            {
                                ui->listWidget->addItem(textChild.nodeValue());
                            }
                        }
                    }
                    node = node.nextSibling();
                }
            }
            node = node.nextSibling();
        }
    }
}

void Widget::onRequestReadyOwnPosts(QByteArray response) {
    qDebug() << "requestReady-OwnPosts";
    // for JSON
    QDomDocument dom;
    if(dom.setContent(response)==false)
    {
        QJson::Parser parser;
        bool ok =false;
        QVariant top = parser.parse(response, &ok);
        if(!ok)
        {
            qDebug() << response.size() << response << "json error";
            return;
        }
        qDebug() << "top.list.size:" << top.toList().size();
        qDebug() << "top.map.size:" << top.toMap().size();

        //QVariantMap result = top.toMap();
        QVariantList contentList = top.toList();

        for(int i=0;i<contentList.size();++i)
        {
            QVariant content = contentList.at(i);
            ui->tableWidget_2->setItem(ui->tableWidget_2->rowCount()-i-1, 0, new QTableWidgetItem(content.toMap()["created_at"].toString().mid(8,11)));
            if(i<10)
            {
                ui->tableWidget->setItem(9-i,0, new QTableWidgetItem(content.toMap()["text"].toString()));
            }
        }
        return;
    }
}

void Widget::sendTweet(QString tweet) {

    if( _oauthSettings->value("oauth_token").toString().isEmpty() ||
            _oauthSettings->value("oauth_token_secret").toString().isEmpty()) {
        qDebug() << "No access tokens. Aborting.";

        return;
    }

    _oauthRequest->initRequest(KQOAuthRequest::AuthorizedRequest,
                               QUrl("http://api.twitter.com/1/statuses/update.xml")
                               );
    _oauthRequest->setConsumerKey("nXrjGagSLIQxCC4AmJ1J3g");
    _oauthRequest->setConsumerSecretKey("QkTRr36zT6wAMERWqKUefyshxXIeaGzPKxB5pwMN0tg");
    _oauthRequest->setToken(_oauthSettings->value("oauth_token").toString());
    _oauthRequest->setTokenSecret(_oauthSettings->value("oauth_token_secret").toString());

    KQOAuthParameters params;
    params.insert("status", tweet);
    _oauthRequest->setAdditionalParameters(params);

    qDebug() << "Attempting send tweet such as " << tweet;
    //_oauthManager->executeRequest(_oauthRequest);

    ui->tableWidget->removeRow(0);
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    ui->tableWidget->setItem(9,0,new QTableWidgetItem(tweet));

    ui->tableWidget_2->removeRow(0);
    ui->tableWidget_2->setRowCount(ui->tableWidget_2->rowCount()+1);
    ui->tableWidget_2->setItem(129,0, new QTableWidgetItem( (QDateTime::currentDateTime().toString("dd hh:mm:ss"))));
    //    connect(_oauthManager, SIGNAL(authorizedRequestReady()),
    //         this, SLOT(onAuthorizedRequestReady()));

    // LineEdits must cleare content.
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

void Widget::on_pushButton_3_clicked()
{
    ui->lineEdit->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    _oauthSettings->setValue("number", ui->lineEdit->text());
    _oauthManager->onVerificationReceived1(ui->lineEdit->text());
}

void Widget::on_lineEdit_2_editingFinished()
{
    //lineEditの継承クラスにCtrl+Enter押し下し時の処置：(\d)との置換：を記述する
}

void Widget::on_pushButton_4_clicked()
{
    QString tweet = ui->plainTextEdit->toPlainText();
    tweet.replace(QRegExp("\\(\\d+:([^()]+)\\)"), "\\1");
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
