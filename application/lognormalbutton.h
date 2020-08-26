#ifndef LOGNORMALBUTTON_H
#define LOGNORMALBUTTON_H
#include <DPushButton>

class QKeyEvent;
DWIDGET_USE_NAMESPACE
class LogNormalButton: public DPushButton
{
public:
    explicit LogNormalButton(QWidget *parent = nullptr);
    explicit LogNormalButton(const QString &text, QWidget *parent = nullptr);
    LogNormalButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void  keyReleaseEvent(QKeyEvent *event)override;
    void paintEvent(QPaintEvent *e) override;
    void focusInEvent(QFocusEvent *event)override;
private:
    Qt::FocusReason m_reson;


};

#endif // LOGNORMALBUTTON_H
