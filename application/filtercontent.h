/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILTERCONTENT_H
#define FILTERCONTENT_H

#include <DComboBox>
#include <DFrame>
#include <DLabel>
#include <DSuggestButton>
#include <DWidget>
#include <DPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QWidget>
#include "structdef.h"
class LogCombox;
class LogPeriodButton;
class LogNormalButton;
class FilterContent : public Dtk::Widget::DFrame
{
    Q_OBJECT
public:
    explicit FilterContent(QWidget *parent = nullptr);
    ~FilterContent();

    void initUI();
    void initConnections();

    void shortCutExport();
protected:
    //  void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void setAppComboBoxItem();

    void setSelectorVisible(bool lvCbx, bool appListCbx, bool statusCbx, bool period, bool needMove,
                            bool typecbx = false);  // modified by Airy
    void setSelection(FILTER_CONFIG iConifg);

    void setUeButtonSytle();

    void resizeWidth();
    void updateWordWrap();
    void updateDataState();
    void setCurrentConfig(FILTER_CONFIG iConifg);


signals:
    void sigButtonClicked(int tId, int lId, QModelIndex idx);
    //    void sigCbxLvIdxChanged(int idx);
    void sigCbxAppIdxChanged(QString path);
    void sigExportInfo();

    void sigStatusChanged(QString str);

    void sigLogtypeChanged(int tId);  // add by Airy
    void sigResizeWidth(int iWidth);

public slots:
    void slot_logCatelogueClicked(const QModelIndex &index);
    void slot_logCatelogueRefresh(const QModelIndex &index);
    void slot_buttonClicked(int idx);
    void slot_exportButtonClicked();
    void slot_cbxLvIdxChanged(int idx);
    void slot_cbxAppIdxChanged(int idx);
    void slot_cbxStatusChanged(int idx);
    void slot_cbxLogTypeChanged(int idx);  // add  by Airy
    void setExportButtonEnable(bool iEnable);

private:
    QButtonGroup *m_btnGroup;
    Dtk::Widget::DLabel *lvTxt;
    Dtk::Widget::DLabel *periodLabel;
    LogCombox *cbx_lv;
    Dtk::Widget::DLabel *appTxt;
    LogCombox *cbx_app;
    Dtk::Widget::DLabel *statusTxt;
    LogCombox *cbx_status;
    Dtk::Widget::DLabel *typeTxt;     // add by Airy
    LogCombox *typeCbx;  // add by Airy
    QModelIndex m_curTreeIndex;
    LogPeriodButton *m_allBtn = nullptr;
    LogPeriodButton *m_todayBtn = nullptr;
    LogPeriodButton *m_threeDayBtn = nullptr;
    LogPeriodButton *m_lastWeekBtn = nullptr;
    LogPeriodButton *m_lastMonthBtn = nullptr;
    LogPeriodButton *m_threeMonthBtn = nullptr;
    LogNormalButton *exportBtn = nullptr;

    int m_curBtnId, m_curLvCbxId;

    QHBoxLayout *hLayout_period;
    QHBoxLayout *hLayout_all;
    //是否为按钮省略状态
    bool m_isIndentation = false;
    QMap<QString, FILTER_CONFIG>m_config;
    QString m_currentType = "";
};

#endif  // FILTERCONTENT_H
