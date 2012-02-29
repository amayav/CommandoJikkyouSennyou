#include "widgetdomain.h"
#include <QTextCodec>
WidgetDomain::WidgetDomain()
    : _keysHash(new QHash<QString, QString>),
      _replyOfUserStreamsAPI()
{
}

QString WidgetDomain::getKeyString()
{
    return _keyString;
}

bool WidgetDomain::setKeyString(QString key)
{
    _keyString = key;
    return true;
}

QHash<QString, QString>* WidgetDomain::getKeysHash()
{
    return _keysHash;
}

bool WidgetDomain::setReplyOfUserStreamsAPI(QNetworkReply *reply)
{
    _replyOfUserStreamsAPI = reply;
    emit notifyObservers(0);
    return true;
}

QNetworkReply* WidgetDomain::getReplyOfUserStreamsAPI()
{
    return _replyOfUserStreamsAPI;
}
