// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGPERIODBUTTON_H
#define LOGPERIODBUTTON_H
#include <DPushButton>

#include "qtcompat.h"

class LogPeriodButton : public Dtk::Widget::DPushButton
{
public:
    explicit LogPeriodButton(const QString &text, QWidget *parent = nullptr);
    void setStandardSize(int iStahndardWidth);
    Qt::FocusReason getFocusReason();
protected:
    void enterEvent(EnterEvent *e)override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void leaveEvent(EnterEvent *e)override;
#else
    void leaveEvent(QEvent *e)override;
#endif
    void paintEvent(QPaintEvent *event)override;
    void focusInEvent(QFocusEvent *event) override;
private:
    bool isEnter {false};
    int m_stahndardWidth = -1;
    Qt::FocusReason m_reson = Qt::MouseFocusReason;
};

#endif  // LOGPERIODBUTTON_H
