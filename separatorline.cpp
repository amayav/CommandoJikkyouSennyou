#include "separatorline.h"
#include <QMouseEvent>
separatorLine::separatorLine(QWidget *parent) :
    QFrame(parent)
{
    setFrameStyle(QFrame::HLine);
    setCursor(Qt::SizeVerCursor);
}

void separatorLine::mousePressEvent(QMouseEvent *e)
{
    emit mousePressed(e->pos());
}

void separatorLine::mouseReleaseEvent(QMouseEvent *e)
{
    emit mouseReleased(e->pos());
}
