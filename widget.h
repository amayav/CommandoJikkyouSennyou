/******************************************************************************

  widget.h

******************************************************************************/

#ifndef WIDGET_H
#define WIDGET_H

#include <QSettings>
#include <QWidget>
#include <QtKOAuth>
#include <QSslError>
#include <QAbstractSocket>
#include <QSslSocket>
#include <QNetworkReply>
#include <QShortcut>
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
    bool initializeOwnPostsWidgets();
    bool replacePlainTextEditContent(int i, QString str);
    bool setShortCutKeyAndContext(QShortcut *sc, QKeySequence s, Qt::ShortcutContext c);

public slots:

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
    void sendTweet(QString tweet);
    void receive(QList<QSslError> e);
    void cError(QAbstractSocket::SocketError);
    void readyRead();
    void cState(QAbstractSocket::SocketState);
    void slotError(QNetworkReply::NetworkError);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_lineEdit_2_editingFinished();

    void on_pushButton_4_clicked();

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_4_textChanged(const QString &arg1);

    void on_lineEdit_5_textChanged(const QString &arg1);

    void on_lineEdit_6_textChanged(const QString &arg1);

private:
    Ui::Widget *ui;

    KQOAuthManager *_oauthManager;
    KQOAuthRequest *_oauthRequest;
    //  KQOAuthRequest_XAuth *oauthRequestX;
    QSettings *_oauthSettings, *_shortCutKeysSettings, *_sizeSettings;
    QString _keyString;
    QHash<QString, QString> *_keysHash;

    void showFatalError(const QString strMsg, const int nNum = 0,
                        const QString strTitle = tr("Fatal Error!"));
    void showOAuthError(const int nErrNum);

    QSslSocket *sock;

signals:
    void verificationReceived1(QString oauth_verifier);
};

#endif // WIDGET_H
