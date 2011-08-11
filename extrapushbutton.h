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
