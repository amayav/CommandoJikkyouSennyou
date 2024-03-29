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
#ifndef EXTRAPUSHBUTTON_H
#define EXTRAPUSHBUTTON_H
#include <QPushButton>

class ExtraPushButton : public QPushButton
{
    Q_OBJECT
public:
    ExtraPushButton(QString shortCutKey, QString string, QWidget *parent=0);
    QString getShortCutKey();
    QString getString();

public slots:
    void click();

protected:
    virtual void enterEvent(QEvent *e);

private:
    QString _shortCutKey, _string;

signals:
    void clicked(QString string);
    void showingToolTip(QString toolTipString);
};

#endif // EXTRAPUSHBUTTON_H
