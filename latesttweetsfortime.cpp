#include "latesttweetsfortime.h"

LatestTweetsForTime::LatestTweetsForTime()
    : _postedDateTimeList()
{
}

const QList<QDateTime> LatestTweetsForTime::getPostedDateTimeList() const {
    return _postedDateTimeList;
}

bool LatestTweetsForTime::appendPostedDateTimeList(QDateTime &dataTime) {
    _postedDateTimeList.append(dataTime);
    return true;
}

bool LatestTweetsForTime::replacePostedDateTimeList(QDateTime &dataTime) {
    _postedDateTimeList.removeFirst();
    _postedDateTimeList.append(dataTime);
    return true;
}

bool LatestTweetsForTime::clear(){
    _postedDateTimeList.clear();
    return true;
}
