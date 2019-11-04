#ifndef LOGSPINNERWIDGET_H
#define LOGSPINNERWIDGET_H

#include <DSpinner>
#include <DWidget>

class LogSpinnerWidget : public Dtk::Widget::DWidget
{
public:
    LogSpinnerWidget();
    void initUI();

    void spinnerStart();
    void spinnerStop();

private:
    Dtk::Widget::DSpinner *m_spinner;
};

#endif  // LOGSPINNERWIDGET_H
