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

#include "filtercontent.h"
#include <DApplication>
#include <DApplicationHelper>
#include <DComboBox>
#include <DCommandLinkButton>
#include <DFileDialog>
#include <QAbstractItemView>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QProcess>
#include <QVBoxLayout>
#include "logapplicationhelper.h"
#include "logperiodbutton.h"
#include "structdef.h"

#define BUTTON_WIDTH_MIN 68
#define BUTTON_HEIGHT_MIN 36
#define BUTTON_EXPORT_WIDTH_MIN 142

DWIDGET_USE_NAMESPACE

FilterContent::FilterContent(QWidget *parent)
    : DFrame(parent)
    , m_curBtnId(ALL)
    , m_curLvCbxId(INF)
{
    initUI();
    initConnections();
}

FilterContent::~FilterContent() {}

void FilterContent::initUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    // set period info
    hLayout_period = new QHBoxLayout;
    periodLabel = new DLabel(DApplication::translate("Label", "Period:"), this);
    periodLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    m_btnGroup = new QButtonGroup;

    LogPeriodButton *m_allBtn = new LogPeriodButton(DApplication::translate("Button", "All"), this);
    m_allBtn->setObjectName("allBtn");
    m_allBtn->setFixedSize(QSize(BUTTON_WIDTH_MIN, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_allBtn, 0);

    LogPeriodButton *m_todayBtn =
        new LogPeriodButton(DApplication::translate("Button", "Today"), this);
    m_todayBtn->setFixedSize(QSize(64, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_todayBtn, 1);

    LogPeriodButton *m_threeDayBtn =
        new LogPeriodButton(DApplication::translate("Button", "3 days"), this);
    m_threeDayBtn->setFixedSize(QSize(78, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_threeDayBtn, 2);

    LogPeriodButton *m_lastWeekBtn =
        new LogPeriodButton(DApplication::translate("Button", "1 week"), this);
    m_lastWeekBtn->setFixedSize(QSize(78, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_lastWeekBtn, 3);

    LogPeriodButton *m_lastMonthBtn =
        new LogPeriodButton(DApplication::translate("Button", "1 month"), this);
    m_lastMonthBtn->setFixedSize(QSize(92, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_lastMonthBtn, 4);

    LogPeriodButton *m_threeMonthBtn =
        new LogPeriodButton(DApplication::translate("Button", "3 months"), this);
    m_threeMonthBtn->setFixedSize(QSize(92, BUTTON_HEIGHT_MIN));
    m_btnGroup->addButton(m_threeMonthBtn, 5);

    setUeButtonSytle();

    hLayout_period->addWidget(periodLabel);
    hLayout_period->addWidget(m_allBtn);
    hLayout_period->addWidget(m_todayBtn);
    hLayout_period->addWidget(m_threeDayBtn);
    hLayout_period->addWidget(m_lastWeekBtn);
    hLayout_period->addWidget(m_lastMonthBtn);
    hLayout_period->addWidget(m_threeMonthBtn);
    hLayout_period->addStretch(1);

    DSuggestButton *cmdLinkBtn = new DSuggestButton(("Reset"), this);
    cmdLinkBtn->setFlat(true);
    cmdLinkBtn->hide();
    m_btnGroup->addButton(cmdLinkBtn, 6);
    hLayout_period->addWidget(cmdLinkBtn);
    hLayout_period->setSpacing(6);

    // set level info
    hLayout_all = new QHBoxLayout;

    QHBoxLayout *hLayout_lv = new QHBoxLayout;
    lvTxt = new DLabel(DApplication::translate("Label", "Level:  "), this);
    lvTxt->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    cbx_lv = new DComboBox(this);
    cbx_lv->setMinimumSize(QSize(208, BUTTON_HEIGHT_MIN));
    cbx_lv->addItems(QStringList() << DApplication::translate("ComboBox", "All")
                                   << DApplication::translate("ComboBox", "Emergency")
                                   << DApplication::translate("ComboBox", "Alert")
                                   << DApplication::translate("ComboBox", "Critical")
                                   << DApplication::translate("ComboBox", "Error")
                                   << DApplication::translate("ComboBox", "Warning")
                                   << DApplication::translate("ComboBox", "Notice")
                                   << DApplication::translate("ComboBox", "Info")
                                   << DApplication::translate("ComboBox", "Debug"));
    cbx_lv->setCurrentText(DApplication::translate("ComboBox", "Info"));
    hLayout_lv->addWidget(lvTxt);
    hLayout_lv->addWidget(cbx_lv, 1);
    hLayout_lv->setSpacing(6);
    hLayout_all->addLayout(hLayout_lv);

    // set all files under ~/.cache/deepin
    QHBoxLayout *hLayout_app = new QHBoxLayout;
    appTxt = new DLabel(DApplication::translate("Label", "Application list:"), this);
    cbx_app = new DComboBox(this);
    cbx_app->setMinimumSize(QSize(180, BUTTON_HEIGHT_MIN));
    hLayout_app->addWidget(appTxt);
    hLayout_app->addWidget(cbx_app, 1);
    hLayout_app->setSpacing(6);
    hLayout_all->addLayout(hLayout_app);

    // add status item
    QHBoxLayout *hLayout_status = new QHBoxLayout;
    statusTxt = new DLabel(DApplication::translate("Label", "Status:"), this);
    cbx_status = new DComboBox(this);
    cbx_status->setMinimumWidth(120);
    cbx_status->setMinimumSize(QSize(120, BUTTON_HEIGHT_MIN));
    cbx_status->addItems(QStringList() << DApplication::translate("ComboBox", "All") << "OK"
                                       << "Failed");
    hLayout_status->addWidget(statusTxt);
    hLayout_status->addWidget(cbx_status, 1);
    hLayout_status->setSpacing(6);
    hLayout_all->addLayout(hLayout_status);

    // add by Airy for adding type item
    QHBoxLayout *hLayout_type = new QHBoxLayout;
    typeTxt = new DLabel(DApplication::translate("Label", "Event Type:"), this);
    typeCbx = new DComboBox(this);
    typeCbx->setMinimumWidth(120);
    typeCbx->setMinimumSize(QSize(120, BUTTON_HEIGHT_MIN));
    typeCbx->addItems(QStringList() << DApplication::translate("ComboBox", "All")
                                    << DApplication::translate("ComboBox", "Login")
                                    << DApplication::translate("ComboBox", "Boot")
                                    << DApplication::translate("ComboBox", "Shutdown"));
    hLayout_status->addWidget(typeTxt);
    hLayout_status->addWidget(typeCbx, 1);
    hLayout_status->setSpacing(6);
    hLayout_all->addLayout(hLayout_type);  // end add

    hLayout_all->addStretch(1);

    DPushButton *exportBtn = new DPushButton(DApplication::translate("Button", "Export"), this);
    exportBtn->setFixedSize(QSize(BUTTON_EXPORT_WIDTH_MIN, BUTTON_HEIGHT_MIN));
    exportBtn->setFocusPolicy(Qt::NoFocus);
    m_btnGroup->addButton(exportBtn, 7);
    hLayout_all->addWidget(exportBtn);

    // set layout
    vLayout->addLayout(hLayout_period);
    vLayout->addLayout(hLayout_all);
    vLayout->setSpacing(16);
    this->setLayout(vLayout);

    // default application list is not visible
    setSelectorVisible(true, false, false, true, false);
}

void FilterContent::initConnections()
{
    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_buttonClicked(int)));
    connect(cbx_lv, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxLvIdxChanged(int)));
    connect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)));
    connect(cbx_status, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxStatusChanged(int)));
    connect(typeCbx, SIGNAL(currentIndexChanged(int)), this,
            SLOT(slot_cbxLogTypeChanged(int)));  // add by Airy
}

void FilterContent::shortCutExport()
{
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    if (!itemData.isEmpty())
        emit sigExportInfo();
}

void FilterContent::setAppComboBoxItem()
{
    cbx_app->clear();
    auto *appHelper = LogApplicationHelper::instance();
    QMap<QString, QString> _map = appHelper->getMap();
    QMap<QString, QString>::const_iterator iter = _map.constBegin();
    while (iter != _map.constEnd()) {
        cbx_app->addItem(iter.key());
        cbx_app->setItemData(cbx_app->count() - 1, iter.value(), Qt::UserRole + 1);
        ++iter;
    }
}

void FilterContent::setSelectorVisible(bool lvCbx, bool appListCbx, bool statusCbx, bool period,
                                       bool needMove, bool typecbx)
{
    //    va_list arg_ptr;//定义一个可变参数指针
    //    va_start(arg_ptr,needMove); //设置needMove为最后一个固定参数
    //    bool typecbx = va_arg(arg_ptr,bool); //设置第二个可变参数

    lvTxt->setVisible(lvCbx);
    cbx_lv->setVisible(lvCbx);

    appTxt->setVisible(appListCbx);
    cbx_app->setVisible(appListCbx);

    statusTxt->setVisible(statusCbx);
    cbx_status->setVisible(statusCbx);

    typeTxt->setVisible(typecbx);  // add by Airy
    typeCbx->setVisible(typecbx);  // add by Airy

    periodLabel->setVisible(period);
    for (int i = 0; i < 6; i++) {
        LogPeriodButton *pushBtn = static_cast<LogPeriodButton *>(m_btnGroup->button(i));
        pushBtn->setVisible(period);
    }

    if (needMove) {
        hLayout_period->addWidget(m_btnGroup->button(EXPORT));
        hLayout_all->removeWidget(m_btnGroup->button(EXPORT));
    } else {
        hLayout_period->removeWidget(m_btnGroup->button(EXPORT));
        hLayout_all->addWidget(m_btnGroup->button(EXPORT));
    }
    //    va_end(arg_ptr);  //清除可变参数指针
}

void FilterContent::setUeButtonSytle()
{
    for (QAbstractButton *abtn : m_btnGroup->buttons()) {
        LogPeriodButton *btn = static_cast<LogPeriodButton *>(abtn);
        btn->setFlat(true);
        btn->setCheckable(true);
        if (btn->objectName() == "allBtn")
            btn->setChecked(true);
    }
}

void FilterContent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Save pen
    QPen oldPen = painter.pen();

    painter.setRenderHint(QPainter::Antialiasing);
    DPalette pa = DApplicationHelper::instance()->palette(this);
    painter.setBrush(QBrush(pa.color(DPalette::Base)));
    QColor penColor = pa.color(DPalette::FrameBorder);
    penColor.setAlphaF(0.05);
    painter.setPen(QPen(penColor));

    QRectF rect = this->rect();
    rect.setX(0.5);
    rect.setY(0.5);
    rect.setWidth(rect.width() - 0.5);
    rect.setHeight(rect.height() - 0.5);

    QPainterPath painterPath;
    painterPath.addRoundedRect(rect, 8, 8);
    painter.drawPath(painterPath);

    // Restore the pen
    painter.setPen(oldPen);

    DFrame::paintEvent(event);
}

void FilterContent::slot_logCatelogueClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty()) {
        return;
    }

    m_curTreeIndex = index;

    if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        this->setAppComboBoxItem();
        this->setSelectorVisible(true, true, false, true, false);
        cbx_app->setCurrentIndex(0);
        emit sigCbxAppIdxChanged(cbx_app->itemData(0, Qt::UserRole + 1).toString());
    } else if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
        this->setSelectorVisible(true, false, false, true, false);
        cbx_lv->setCurrentIndex(INF + 1);  // index+1
    } else if (itemData.contains(BOOT_TREE_DATA)) {
        this->setSelectorVisible(false, false, true, false, false);
    } else if (itemData.contains(KERN_TREE_DATA) || itemData.contains(DPKG_TREE_DATA)) {
        this->setSelectorVisible(false, false, false, true, true);
    } else if (itemData.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
        this->setSelectorVisible(false, false, false, true,
                                 true);  // modified by Airy for showing  peroid
    } else if (itemData.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {  // add by Airy
        this->setSelectorVisible(false, false, false, true, false,
                                 true);  // modifed by Airy for showing peroid
    }
}

void FilterContent::slot_buttonClicked(int idx)
{
    /** note: In order to adapt to the new scene, select time-period first,
     *        then select any log item, should display current log info.
     *        so comment this judge.
     */
    //    if (!m_curTreeIndex.isValid())
    //        return;

    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    // because button has no focus,so focus on label;
    lvTxt->setFocus();
    switch (idx) {
        case ALL:
        case ONE_DAY:
        case THREE_DAYS:
        case ONE_WEEK:
        case ONE_MONTH:
        case THREE_MONTHS: {
            m_curBtnId = idx;
            emit sigButtonClicked(idx, m_curLvCbxId, m_curTreeIndex);
        } break;
        case RESET: {
            m_curBtnId = ALL;
            //            if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive) ||
            //                itemData.contains(".cache"))
            if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive) ||
                itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
                cbx_lv->setCurrentIndex(INF + 1);
            } else {
                emit sigButtonClicked(m_curBtnId, INVALID, m_curTreeIndex);
            }
        } break;
        case EXPORT:
            if (!itemData.isEmpty())
                emit sigExportInfo();
            break;
        default:
            break;
    }
}

void FilterContent::slot_cbxLvIdxChanged(int idx)
{
    m_curLvCbxId = idx - 1;
    emit sigButtonClicked(m_curBtnId, m_curLvCbxId, m_curTreeIndex);
}

void FilterContent::slot_cbxAppIdxChanged(int idx)
{
    QString path = cbx_app->itemData(idx, Qt::UserRole + 1).toString();

    emit sigCbxAppIdxChanged(path);
}

void FilterContent::slot_cbxStatusChanged(int idx)
{
    QString str;
    if (idx == 1)
        str = "OK";
    else if (idx == 2)
        str = "Failed";
    emit sigStatusChanged(str);
}

// add by Airy
void FilterContent::slot_cbxLogTypeChanged(int idx)
{
    emit sigLogtypeChanged(idx);
    qDebug() << "emit signal " + QString::number(idx);
}
