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
/******************************************************************************

  widget.h

******************************************************************************/

#ifndef WIDGET_H

#define WIDGET_H

#include <QSettings>
#include <QWidget>
#include <QtKOAuth>
#include <QNetworkReply>
#include <QShortcut>
#include "widgetdomain.h"
#include <QNetworkProxy>
#include <QTimer>
#include "latesttweetsfortime.h"

class KQOAuthManager;
class KQOAuthRequest;
//class KQOAuthRequest_XAuth;


namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private slots:
    void onTemporaryTokenReceived(QString token, QString tokenSecret);
    void onAuthorizationReceived(QString token, QString verifier);
    void onAccessTokenReceived(QString token, QString tokenSecret);
    void onAuthorizedRequestDone();
    void onRequestReady(QByteArray response);
    void onRequestReadyTimeline(QByteArray response);
    void onRequestReadyOwnPosts(QByteArray);
    void onReadyRead();
    void sendTweet(QString tweet);
    void receiveSSLError(QNetworkReply * reply, const QList<QSslError> & errors);
    void readOwnPosts(QNetworkReply * r);

    void countDownClock();

    void updateData();

    /**
      Open authorizing web page in twitter.com.
      */
    void on_authorizationButton_clicked();

    /**
      Start Timeline flowing.
      */
    void on_showTimeLineButton_clicked();

    /**
      Set authorizing this program with twitter.com.
      */
    void on_verificationButton_clicked();

    /**
      Post some text to twitter.com.
      */
    void on_postButton_clicked();

    void on_lineEdit_2_textChanged(const QString &arg1);
    void on_lineEdit_3_textChanged(const QString &arg1);
    void on_lineEdit_4_textChanged(const QString &arg1);
    void on_lineEdit_5_textChanged(const QString &arg1);
    void on_lineEdit_6_textChanged(const QString &arg1);

    void on_proxyButton_clicked();

private:
    Ui::Widget *ui;

    KQOAuthManager *_oauthManager;
    KQOAuthRequest *_oauthRequest;
    //  KQOAuthRequest_XAuth *oauthRequestX;
    QSettings *_oauthSettings, *_shortCutKeysSettings, *_sizeSettings;
    QNetworkProxy *_proxy;
    QString _consumerKey;
    QString _consumerSecretKey;
    QTimer *_clockCountTimer;
    LatestTweetsForTime _latestTweetsForTime;

    WidgetDomain *_subject;

    bool setPostShortcutKeys();
    bool setJikkyouShortcutKeys();
    bool setWindowSize();
    bool setProxyLineEdits();
    bool setConsumerKeyAndConsumerSecretLineEdits();
    bool setConsumerKeyAndConsumerSecret();
    bool initializeKQOAuth();

    bool setConsumerKeyAndConsumerSecretSettings();

    void showFatalError(const QString strMsg, const int nNum = 0,
                        const QString strTitle = tr("Fatal Error!"));
    void showOAuthError(const int nErrNum);

    /**
      Initialize widget which has user own post data.
      Latest 130 and Latest 10.
      */
    bool initializeOwnPostsWidgets();

    /**
      Replace text from PlaneTextEdit to twitter.
      */
    bool replacePlainTextEditContent(int i, QString str);

    /**
      Set shortcut key config.
      */
    bool setShortCutKeyAndContext(QShortcut *sc, QKeySequence s, Qt::ShortcutContext c);

    /**
      Connect with twitter.com by SSL.
      This is copied and changed from KQOauthManager::executeRequest(KQOAuthRequest *request)
      */
    bool SSLconnect(KQOAuthRequest *request);

    /**
      set consumerKey and consumerSecretKey to oauthRequest.
      */
    bool initializeRequest(QUrl url,
                           KQOAuthRequest::RequestHttpMethod requestMethod = KQOAuthRequest::POST,
                           KQOAuthRequest::RequestType requestType = KQOAuthRequest::AuthorizedRequest);

    bool setTokenAndSecretTokenToRequest();

    bool SetButtonsEnable(bool on = true);
    QByteArray makeAuthHeaderFrom(QList<QByteArray> requestParameters);
    bool isOAuthed();
    QString getValueOfMatchedKey();
    WidgetDomain* getWidgetDomain();

signals:
    void verificationReceived1(QString oauth_verifier);
};

#endif // WIDGET_H
