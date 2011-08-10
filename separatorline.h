#ifndef SEPARATORLINE_H
#define SEPARATORLINE_H

#include <QFrame>

class separatorLine : public QFrame
{
    Q_OBJECT
public:
    explicit separatorLine(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:

signals:
    void mousePressed(QPoint pos);
    void mouseReleased(QPoint pos);

public slots:

};

#endif // SEPARATORLINE_H
