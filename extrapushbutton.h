#ifndef EXTRAPUSHBUTTON_H
#define EXTRAPUSHBUTTON_H
#include <QPushButton>

class ExtraPushButton : public QPushButton
{
    Q_OBJECT
public:
    ExtraPushButton(QString shortCutKey, QString string, QWidget *parent=0);

public slots:
    void click();

private:
    QString _shortCutKey, _string;

signals:
    void clicked(QString string);
};

#endif // EXTRAPUSHBUTTON_H
