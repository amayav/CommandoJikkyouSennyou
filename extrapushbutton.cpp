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
