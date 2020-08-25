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
#include "logcombox.h"
#include "lognormalbutton.h"
#include "logapplicationhelper.h"
#include "logperiodbutton.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DComboBox>
#include <DCommandLinkButton>
#include <DFileDialog>

#include <QAbstractItemView>
#include <QDebug>
#include <QDir>
#include <QSizePolicy>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QProcess>
#include <QVBoxLayout>
#include <QResizeEvent>

#include "structdef.h"

#define BUTTON_WIDTH_MIN 68
#define BUTTON_HEIGHT_MIN 36
#define BUTTON_EXPORT_WIDTH_MIN 142
#define FONT_20_MIN_WIDTH 821
#define FONT_18_MIN_WIDTH 100
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
    m_btnGroup = new QButtonGroup(this);

    m_allBtn = new LogPeriodButton(DApplication::translate("Button", "All"), this);
    m_allBtn->setToolTip(DApplication::translate("Button", "All"));  // add by Airy for bug 16245
    m_allBtn->setObjectName("allBtn");
    // m_allBtn->setFixedHeight(BUTTON_HEIGHT_MIN);
    //m_allBtn->setMaximumWidth(20);
//    m_allBtn->setW
//    QFont standardFont = m_allBtn->font();
//    standardFont.setPixelSize(16);
    //  standardFont.set
    // QFontMetrics(standardFont).width("All") + m_allBtn->contentsMargins().left() + m_allBtn->contentsMargins().right()+m_allBtn->b;
    // m_allBtn->setMaximumWidth();
//    m_allBtn->setFixedSize(
//        QSize(BUTTON_WIDTH_MIN - 4, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    m_btnGroup->addButton(m_allBtn, 0);

    m_todayBtn =
        new LogPeriodButton(DApplication::translate("Button", "Today"), this);
    m_todayBtn->setToolTip(DApplication::translate("Button", "Today"));  // add by Airy for bug
    // 16245
    // m_todayBtn->setFixedSize(QSize(64 + 10, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    //  m_todayBtn->setFixedHeight(BUTTON_HEIGHT_MIN);
    m_btnGroup->addButton(m_todayBtn, 1);

    m_threeDayBtn =
        new LogPeriodButton(DApplication::translate("Button", "3 days"), this);
    m_threeDayBtn->setToolTip(
        DApplication::translate("Button", "3 days"));  // add by Airy for bug 16245
//    m_threeDayBtn->setFixedSize(
//        QSize(78 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    //m_threeDayBtn->setFixedHeight(BUTTON_HEIGHT_MIN);
    m_btnGroup->addButton(m_threeDayBtn, 2);

    m_lastWeekBtn =
        new LogPeriodButton(DApplication::translate("Button", "1 week"), this);
    m_lastWeekBtn->setToolTip(
        DApplication::translate("Button", "1 week"));  // add by Airy for bug 16245
//    m_lastWeekBtn->setFixedSize(
//        QSize(78 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    //  m_lastWeekBtn->setFixedHeight(BUTTON_HEIGHT_MIN);
    m_btnGroup->addButton(m_lastWeekBtn, 3);

    m_lastMonthBtn =
        new LogPeriodButton(DApplication::translate("Button", "1 month"), this);
    m_lastMonthBtn->setToolTip(
        DApplication::translate("Button", "1 month"));  // add by Airy for bug 16245
//    m_lastMonthBtn->setFixedSize(
//        QSize(92 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    //  m_lastMonthBtn->setFixedHeight(BUTTON_HEIGHT_MIN);
    m_btnGroup->addButton(m_lastMonthBtn, 4);
    m_threeMonthBtn =
        new LogPeriodButton(DApplication::translate("Button", "3 months"), this);
    m_threeMonthBtn->setToolTip(
        DApplication::translate("Button", "3 months"));  // add by Airy for bug 16245
//    m_threeMonthBtn->setFixedSize(
//        QSize(92 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
    //m_threeMonthBtn->setFixdHeight(BUTTON_HEIGHT_MIN);
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
    hLayout_period->setSpacing(10);

    // set level info
    hLayout_all = new QHBoxLayout;
    QHBoxLayout *hLayout_lvl = new QHBoxLayout;
    lvTxt = new DLabel(DApplication::translate("Label", "Level:  "), this);
    lvTxt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    cbx_lv = new LogCombox(this);
    cbx_lv->setMinimumSize(QSize(198, BUTTON_HEIGHT_MIN));
    // cbx_lv->setMaximumWidth(208);
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

    hLayout_lvl->addWidget(lvTxt);
    hLayout_lvl->addWidget(cbx_lv, 1);
    hLayout_lvl->setSpacing(6);
    hLayout_all->addLayout(hLayout_lvl);
    // set all files under ~/.cache/deepin
    QHBoxLayout *hLayout_app = new QHBoxLayout;
    appTxt = new DLabel(DApplication::translate("Label", "Application list:"), this);
    cbx_app = new LogCombox(this);
    cbx_app->setMinimumSize(QSize(180, BUTTON_HEIGHT_MIN));
    hLayout_app->addWidget(appTxt);
    hLayout_app->addWidget(cbx_app, 1);
    hLayout_app->setSpacing(6);
    hLayout_all->addLayout(hLayout_app);

    // add status item
    QHBoxLayout *hLayout_status = new QHBoxLayout;
    statusTxt = new DLabel(DApplication::translate("Label", "Status:"), this);
    cbx_status = new LogCombox(this);
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
    typeCbx = new LogCombox(this);
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
    exportBtn = new LogNormalButton(DApplication::translate("Button", "Export"), this);
    //  exportBtn->setContentsMargins(102, 100, 200, 200);
    //exportBtn->setFixedSize(QSize(BUTTON_EXPORT_WIDTH_MIN, BUTTON_HEIGHT_MIN));
    exportBtn->setContentsMargins(0, 0, 18, 18);
    exportBtn->setFixedWidth(BUTTON_EXPORT_WIDTH_MIN);
//    exportBtn->setFocusPolicy(Qt::TabFocus);
    hLayout_all->addWidget(exportBtn);
    // set layout
    vLayout->addLayout(hLayout_period);
    vLayout->addLayout(hLayout_all);
    vLayout->setSpacing(16);

    this->setLayout(vLayout);
//    cbx_lv->setFocusPolicy(Qt::TabFocus);
//    cbx_app->setFocusPolicy(Qt::TabFocus);
//    cbx_status->setFocusPolicy(Qt::TabFocus);
//    typeCbx->setFocusPolicy(Qt::TabFocus);
    // default application list is not visible
    setSelectorVisible(true, false, false, true, false);
    m_allBtn->installEventFilter(this);
    m_todayBtn->installEventFilter(this);
    m_threeDayBtn->installEventFilter(this);
    m_lastWeekBtn->installEventFilter(this);
    m_lastMonthBtn->installEventFilter(this);
    m_threeMonthBtn->installEventFilter(this);
}

void FilterContent::initConnections()
{
    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_buttonClicked(int)));
    connect(exportBtn, &DPushButton::clicked, this, &FilterContent::slot_exportButtonClicked);

    connect(cbx_lv, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxLvIdxChanged(int)));
    connect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)), Qt::UniqueConnection);
    connect(cbx_status, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxStatusChanged(int)));
    connect(typeCbx, SIGNAL(currentIndexChanged(int)), this,
            SLOT(slot_cbxLogTypeChanged(int)));  // add by Airy
}

void FilterContent::shortCutExport()
{
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    bool canExport = true;
    if (exportBtn) {
        canExport = exportBtn->isEnabled();
    }
    if (!itemData.isEmpty() && canExport)
        emit sigExportInfo();
}

//void FilterContent::resizeEvent(QResizeEvent *event)
//{
//    updateWordWrap();
//}



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
    setUpdatesEnabled(false);
    lvTxt->setVisible(lvCbx);
    cbx_lv->setVisible(lvCbx);
    if (cbx_lv->isVisible()) {
        cbx_lv->setCurrentIndex(INF + 1);

    }

    appTxt->setVisible(appListCbx);
    cbx_app->setVisible(appListCbx);
    if (cbx_app->isVisible())
        cbx_app->setCurrentIndex(0);
    statusTxt->setVisible(statusCbx);
    cbx_status->setVisible(statusCbx);
    if (cbx_status->isVisible())
        cbx_status->setCurrentIndex(0);
    typeTxt->setVisible(typecbx);  // add by Airy
    typeCbx->setVisible(typecbx);  // add by Airy
    if (typeCbx->isVisible())
        typeCbx->setCurrentIndex(0);
    periodLabel->setVisible(period);
    for (int i = 0; i < 6; i++) {
        LogPeriodButton *pushBtn = static_cast<LogPeriodButton *>(m_btnGroup->button(i));
        pushBtn->setVisible(period);
    }

    if (needMove) {
        hLayout_period->addWidget(exportBtn);
        hLayout_all->removeWidget(exportBtn);
    } else {
        hLayout_period->removeWidget(exportBtn);
        hLayout_all->addWidget(exportBtn);

    }
    resizeWidth();
    setUpdatesEnabled(true);
    //    va_end(arg_ptr);  //清除可变参数指针

}

void FilterContent::setUeButtonSytle()
{
    for (QAbstractButton *abtn : m_btnGroup->buttons()) {
        LogPeriodButton *btn = static_cast<LogPeriodButton *>(abtn);

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

//    m_allBtn->setFixedSize(
//        QSize(BUTTON_WIDTH_MIN - 4, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    m_todayBtn->setFixedSize(QSize(64 + 10, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    m_threeDayBtn->setFixedSize(
//        QSize(78 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    m_lastWeekBtn->setFixedSize(
//        QSize(78 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    m_lastMonthBtn->setFixedSize(
//        QSize(92 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    m_threeMonthBtn->setFixedSize(
//        QSize(92 + 12, BUTTON_HEIGHT_MIN));  // modified by Airy for bug 16245
//    exportBtn->setFixedSize(QSize(BUTTON_EXPORT_WIDTH_MIN, BUTTON_HEIGHT_MIN));

    DFrame::paintEvent(event);
}

bool FilterContent::eventFilter(QObject *obj, QEvent *event)
{

//    LogPeriodButton *m_allBtn = nullptr;
//    LogPeriodButton *m_todayBtn = nullptr;
//    LogPeriodButton *m_threeDayBtn = nullptr;
//    LogPeriodButton *m_lastWeekBtn = nullptr;
//    LogPeriodButton *m_lastMonthBtn = nullptr;
//    LogPeriodButton *m_threeMonthBtn = nullptr;
    if (event->type() == QEvent::KeyPress) {
        if (obj == m_allBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_todayBtn->click();
                m_todayBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_threeMonthBtn->click();
                m_threeMonthBtn->setFocus(Qt::TabFocusReason);
            }
        } else if (obj == m_todayBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_threeDayBtn->click();
                m_threeDayBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_allBtn->click();
                m_allBtn->setFocus(Qt::TabFocusReason);
            }
        } else if (obj == m_threeDayBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_lastWeekBtn->click();
                m_lastWeekBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_todayBtn->click();
                m_todayBtn->setFocus(Qt::TabFocusReason);
            }
        } else if (obj == m_lastWeekBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_lastMonthBtn->click();
                m_lastMonthBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_threeDayBtn->click();
                m_threeDayBtn->setFocus(Qt::TabFocusReason);
            }
        } else if (obj == m_lastMonthBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_threeMonthBtn->click();
                m_threeMonthBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_lastWeekBtn->click();
                m_lastWeekBtn->setFocus(Qt::TabFocusReason);
            }
        }  else if (obj == m_threeMonthBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            if (kev->key() == Qt::Key_Right) {
                m_allBtn->click();
                m_allBtn->setFocus(Qt::TabFocusReason);
            } else if (kev->key() == Qt::Key_Left) {
                m_lastMonthBtn->click();
                m_lastMonthBtn->setFocus(Qt::TabFocusReason);
            }
        }
    }
    return DFrame::eventFilter(obj, event);
}


//自适应宽度
void FilterContent::resizeWidth()
{
    int periodWidth = 0;
    int periodCount = hLayout_period->count();

    for (int i = 0; i < periodCount ; ++i) {
        QLayoutItem *item = hLayout_period->itemAt(i);
        QWidget *item_w = item->widget();
        if (item_w && (!item_w->isHidden())) {
            int  itemWidth = item_w->geometry().width();
            periodWidth += itemWidth;
        }
    }
    emit   sigResizeWidth(periodWidth);
}
//根据当前宽度省略日期筛选按钮文字以适应宽度，让控件能塞下
void FilterContent::updateWordWrap()
{
    int currentWidth = this->rect().width();
//    int leftMargin = this->contentsMargins().left();
//    int rightMargin = this->contentsMargins().right();
//    int layoutWidth = hLayout_period->sizeHint().width();
    qDebug() << "currentWidth" << currentWidth;
//    qDebug() << "layoutWidth" << layoutWidth + leftMargin + rightMargin;
//    qDebug() << "leftMargin" << leftMargin;
//    qDebug() << "rightMargin" << rightMargin << hLayout_all->count() ;
    setUpdatesEnabled(false);
    //12对应系统16字号，13.5对应系统18字号,15对应系统20字号
    qreal fontSize = m_allBtn->font().pointSizeF();
    int minWidth = -1;
    qDebug() << fontSize;
    if (fontSize <= 13.5) {
        minWidth = FONT_20_MIN_WIDTH;
    } else {
        minWidth = FONT_20_MIN_WIDTH;
    }
    //判断各字体下能塞下控件的最小宽度
    if ((currentWidth <= minWidth) && (hLayout_period->count() > 9)) {
        // if (m_allBtn->font().pointSizeF() > 12) {
        //qDebug() << font().pointSizeF();
        QFont standFont = m_allBtn->font();
        QFont standFontBig = standFont;
        standFont.setPointSizeF(13.5);
        //standFontBig.setPointSize(14);
        periodLabel->setText(QFontMetrics(periodLabel->font()).elidedText(DApplication::translate("Label", "Period:"), Qt::ElideRight, 1 + QFontMetrics(periodLabel->font()).width(DApplication::translate("Label", "Period:"))));
        // m_allBtn->setText(QFontMetrics(m_allBtn->font()).elidedText(DApplication::translate("Button", "All"), Qt::ElideRight, 1 +  QFontMetrics(standFontBig).width(DApplication::translate("Button", "All"))));
        m_todayBtn->setText(QFontMetrics(m_todayBtn->font()).elidedText(DApplication::translate("Button", "Today"), Qt::ElideRight, 1 + QFontMetrics(standFont).width(DApplication::translate("Button", "Today"))));
        m_threeDayBtn->setText(QFontMetrics(m_threeDayBtn->font()).elidedText(DApplication::translate("Button", "3 days"), Qt::ElideRight, 1 + QFontMetrics(standFont).width(DApplication::translate("Button", "3 days"))));
        m_lastWeekBtn->setText(QFontMetrics(m_lastWeekBtn->font()).elidedText(DApplication::translate("Button", "1 week"), Qt::ElideRight, 1 + QFontMetrics(standFont).width(DApplication::translate("Button", "1 week"))));
        m_lastMonthBtn->setText(QFontMetrics(m_lastMonthBtn->font()).elidedText(DApplication::translate("Button", "1 month"), Qt::ElideRight, 1 + QFontMetrics(standFont).width(DApplication::translate("Button", "1 month"))));
        m_threeMonthBtn->setText(QFontMetrics(m_threeMonthBtn->font()).elidedText(DApplication::translate("Button", "3 months"), Qt::ElideRight, 1 + QFontMetrics(standFont).width(DApplication::translate("Button", "3 months"))));
    }  else  {
        // qDebug() << font().pointSizeF();
        periodLabel->setText(DApplication::translate("Label", "Period:"));
        m_allBtn->setText(DApplication::translate("Button", "All"));
        m_todayBtn->setText(DApplication::translate("Button", "Today"));
        m_threeDayBtn->setText(DApplication::translate("Button", "3 days"));
        m_lastWeekBtn->setText(DApplication::translate("Button", "1 week"));
        m_lastMonthBtn->setText(DApplication::translate("Button", "1 month"));
        m_threeMonthBtn->setText(DApplication::translate("Button", "3 months"));
        // cbx_lv->setMinimumWidth(208);
    }
    setUpdatesEnabled(true);
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
    m_btnGroup->button(0)->setChecked(true); //add by Airy for bug 19660:period button default setting

    if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        this->setAppComboBoxItem();
        this->setSelectorVisible(true, true, false, true, false);
        cbx_app->setCurrentIndex(0);
        cbx_lv->setCurrentIndex(INF + 1); //add by Airy for bug 19660:period button default setting
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
    } else if (itemData.contains(KWIN_TREE_DATA)) {
        this->setSelectorVisible(false, false, false, false, false);
    }
}

void FilterContent::slot_logCatelogueRefresh(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty()) {
        return;
    }

    if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        //记录当前选择项目以便改变combox内容后可以选择原来的选项刷新
        QString cuurentText = cbx_app->currentText();
        //先disconnect防止改变combox内容时发出currentIndexChanged让主表获取
        disconnect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)));
        this->setAppComboBoxItem();
        int appCount =  cbx_app->count();
        int rsIndex = 0;
        //找原来选的选项的index,如果这个应用日志没了,就选第一个
        for (int i = 0; i < appCount; ++i) {
            if (cbx_app->itemText(i) == cuurentText) {
                rsIndex = i;
                break;
            }
        }
        //不能直接connect再setCurrentIndex,而是要手动发出改变app的信号让其刷新,让combox自己发的话,如果原来的index是0他不发currentindexChanged信号
        cbx_app->setCurrentIndex(rsIndex);
        emit sigCbxAppIdxChanged(cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString());
        connect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)), Qt::UniqueConnection);
        // cbx_app->setCurrentText(cuurentText);
        //  qDebug() << "cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString()" << cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString();
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
    //有这行,按tab时切不了焦点
    // lvTxt->setFocus();
    switch (idx) {
    case ALL:
    case ONE_DAY:
    case THREE_DAYS:
    case ONE_WEEK:
    case ONE_MONTH:
    case THREE_MONTHS: {
//        foreach (QAbstractButton *, m_btnGroup->buttons()) {

//        }
//        qDebug()<<
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
    default:
        break;
    }
}

void FilterContent::slot_exportButtonClicked()
{
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    if (!itemData.isEmpty())
        emit sigExportInfo();
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

void FilterContent::setExportButtonEnable(bool iEnable)
{
    if (exportBtn) {
        exportBtn->setEnabled(iEnable);
    }
}
