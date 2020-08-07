#ifndef LOGCOMBOX_H
#define LOGCOMBOX_H
#include <DComboBox>
DWIDGET_USE_NAMESPACE
class LogCombox: public DComboBox
{
public:
    explicit LogCombox(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event)override;
    void focusOutEvent(QFocusEvent *event)override;
private:
    Qt::FocusReason m_reson;
};

#endif // LOGCOMBOX_H
