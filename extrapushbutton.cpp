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
#include "extrapushbutton.h"
#include <QDebug>
#include <QEvent>

/*
  public functions
  */
ExtraPushButton::ExtraPushButton(QString shortCutKey, QString string, QWidget *parent)
    :QPushButton(shortCutKey + ":" + string, parent),
      _shortCutKey(shortCutKey),
      _string(string)
{
    if(string.size()>7)
    {
        setText(shortCutKey + ":" + string.left(7) + "...");
    }
    setMaximumSize(130,23);
    setMinimumSize(130,23);
}

QString ExtraPushButton::getShortCutKey()
{
    return _shortCutKey;
}

QString ExtraPushButton::getString()
{
    return _string;
}

/*
  public slots
  */
void ExtraPushButton::click()
{
    emit clicked(_string);
}

/*
  private functions
  */
void ExtraPushButton::enterEvent(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::Enter:
    {
        if(this->toolTip().size()==0)
        {
            qDebug("Tool Tip of Button Error");
            return;
        }
        if(this->isEnabled()==false)
        {
            return;
        }
        emit showingToolTip(toolTip());
        break;
    }
    default:
        break;
    }
}
