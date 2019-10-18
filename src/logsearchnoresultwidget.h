#ifndef LOGSEARCHNORESULTWIDGET_H
#define LOGSEARCHNORESULTWIDGET_H

#include <DLabel>
#include <DWidget>
#include <QWidget>

class LogSearchNoResultWidget : public Dtk::Widget::DWidget
{
public:
    LogSearchNoResultWidget(QWidget *parent = nullptr);
    ~LogSearchNoResultWidget();

    void setContent(QString word);

private:
    Dtk::Widget::DLabel *m_text;
};

#endif  // LOGSEARCHNORESULTWIDGET_H
