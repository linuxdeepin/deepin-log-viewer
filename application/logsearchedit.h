#ifndef LOGSEARCHEDIT_H
#define LOGSEARCHEDIT_H
#include <DSearchEdit>
DWIDGET_USE_NAMESPACE
class LogSearchEdit : public DSearchEdit
{
public:
    explicit LogSearchEdit(QWidget *parent = nullptr);
protected:
    void focusInEvent(QFocusEvent *event)override;
};

#endif // LOGSEARCHEDIT_H
