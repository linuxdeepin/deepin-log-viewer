#ifndef MYICONBUTTON_H
#define MYICONBUTTON_H

#include <QPushButton>
class LogIconButton : public QPushButton
{
public:
    explicit LogIconButton(QWidget *parent = nullptr);
    explicit LogIconButton(QString text, QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *e) override;

private:
    bool m_status;
};

#endif  // MYICONBUTTON_H
