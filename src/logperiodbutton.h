#ifndef LOGPERIODBUTTON_H
#define LOGPERIODBUTTON_H
#include <DPushButton>
#include <QEvent>

class LogPeriodButton : public Dtk::Widget::DPushButton
{
public:
    LogPeriodButton(const QString text, QWidget *parent = nullptr);

    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

    void paintEvent(QPaintEvent *event);

private:
    bool isEnter {false};
};

#endif  // LOGPERIODBUTTON_H
