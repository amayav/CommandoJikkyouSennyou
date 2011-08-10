#include "extrapushbutton.h"
#include <QDebug>

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

/*
  public slots
  */
void ExtraPushButton::click()
{
    emit clicked(_string);
}
