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
#ifndef LATESTTWEETSFORTIME_H
#define LATESTTWEETSFORTIME_H
#include <QDateTime>
class LatestTweetsForTime
{
public:
    LatestTweetsForTime();
    const QList<QDateTime> getPostedDateTimeList() const;
    bool appendPostedDateTimeList(QDateTime &dateTime);
    bool replacePostedDateTimeList(QDateTime &dateTime);
    bool clear();

private:
    QList<QDateTime> _postedDateTimeList;
};

#endif // LATESTTWEETSFORTIME_H
