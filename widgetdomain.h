#ifndef WIDGETDOMAIN_H
#define WIDGETDOMAIN_H
#include <QNetworkReply>

class WidgetDomain : public QObject
{
    Q_OBJECT
public:
    WidgetDomain();
    bool setReplyOfUserStreamsAPI(QNetworkReply *reply);
    QNetworkReply* getReplyOfUserStreamsAPI();
    QHash<QString, QString>* getKeysHash();
    QString getKeyString();
    bool setKeyString(QString key);

private:
    QHash<QString, QString> *_keysHash;
    QNetworkReply *_replyOfUserStreamsAPI;
    QString _keyString;

signals:
    void notifyObservers(int v);
};

#endif // WIDGETDOMAIN_H
