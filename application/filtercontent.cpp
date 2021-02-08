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
#include "loglistview.h"

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
#include <QPainterPath>

#include "structdef.h"

#define BUTTON_WIDTH_MIN 68
#define BUTTON_HEIGHT_MIN 36
#define BUTTON_EXPORT_WIDTH_MIN 142
#define FONT_20_MIN_WIDTH 821
#define FONT_18_MIN_WIDTH 100
DWIDGET_USE_NAMESPACE

/**
 * @brief FilterContent::FilterContent 构造函数初始化ui和信号槽连接
 * @param parent
 */
FilterContent::FilterContent(QWidget *parent)
    : DFrame(parent)
    , m_curBtnId(ALL)
    , m_curLvCbxId(INF)
{
    initUI();
    initConnections();
}

FilterContent::~FilterContent() {}

/**
 * @brief FilterContent::initUI 初始化界面
 */
void FilterContent::initUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    // set period info
    hLayout_period = new QHBoxLayout;
    periodLabel = new DLabel(DApplication::translate("Label", "Period:"), this);
    periodLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    m_btnGroup = new QButtonGroup(this);
    //初始化时间筛选按钮部分布局
    m_allBtn = new LogPeriodButton(DApplication::translate("Button", "All"), this);
    m_allBtn->setToolTip(DApplication::translate("Button", "All"));  // add by Airy for bug 16245
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
    //设置初始时间筛选为全部
    setUeButtonSytle();
    //所有时间筛选按钮放在上排layout
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
    m_currentType = JOUR_TREE_DATA;
    //设置初始筛选选项

    updateDataState();
    //为时间筛选按钮设置事件过滤器,为处理左右按键键盘事件触发切换选择时间筛选器选项
    m_allBtn->installEventFilter(this);
    m_todayBtn->installEventFilter(this);
    m_threeDayBtn->installEventFilter(this);
    m_lastWeekBtn->installEventFilter(this);
    m_lastMonthBtn->installEventFilter(this);
    m_threeMonthBtn->installEventFilter(this);


}

/**
 * @brief FilterContent::initConnections 连接信号槽
 */
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

/**
 * @brief FilterContent::shortCutExport 主窗口触发导出快捷键时,在这里判断是否能够导出再发出导出信号
 */
void FilterContent::shortCutExport()
{
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    bool canExport = true;
    if (exportBtn) {
        canExport = exportBtn->isEnabled();
    }
    //判断现在导出按钮是否能够点击,不能点击时不可以导出
    if (!itemData.isEmpty() && canExport)
        emit sigExportInfo();
}

//void FilterContent::resizeEvent(QResizeEvent *event)
//{
//    updateWordWrap();
//}


/**
 * @brief FilterContent::setAppComboBoxItem 刷新应用种类下拉列表
 */
void FilterContent::setAppComboBoxItem()
{
    //必须先disconnect变动值的信号槽,因为改变下拉选项会几次触发currentIndexChanged信号,这不是我们想要的
    disconnect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)));
    cbx_app->clear();
    //获取应用列表
    auto *appHelper = LogApplicationHelper::instance();
    QMap<QString, QString> _map = appHelper->getMap();
    QMap<QString, QString>::const_iterator iter = _map.constBegin();
    //添加数据进combox
    while (iter != _map.constEnd()) {
        cbx_app->addItem(iter.key());
        //应用日志的路径为Qt::UserRole + 1
        cbx_app->setItemData(cbx_app->count() - 1, iter.value(), Qt::UserRole + 1);
        ++iter;
    }
    connect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)), Qt::UniqueConnection);

}

/**
 * @brief FilterContent::setSelectorVisible 设置筛选控件显示或不显示以适应各种日志类型的筛选情况
 * @param lvCbx 等级筛选下拉框是否显示
 * @param appListCbx 应用筛选下拉框是否显示
 * @param statusCbx 启动日志状态筛选下拉框是否显示
 * @param period 时间筛选按钮是否显示
 * @param needMove 如果筛选器只有单排布局,则需要移动导出按钮到上排布局,这个参数表示是否把导出按钮移动到上排的布局
 * @param typecbx 开关机日志日志种类筛选下拉框是否显示
 */
void FilterContent::setSelectorVisible(bool lvCbx, bool appListCbx, bool statusCbx, bool period,
                                       bool needMove, bool typecbx)
{
    //    va_list arg_ptr;//定义一个可变参数指针
    //    va_start(arg_ptr,needMove); //设置needMove为最后一个固定参数
    //    bool typecbx = va_arg(arg_ptr,bool); //设置第二个可变参数
//    bool statushasFoucs = false;
//    Qt::FocusReason statusComboxFocusReason;
//    if (cbx_status->hasFocus()) {
//        statusComboxFocusReason =   cbx_status->getFocusReason();
//        statushasFoucs = true;
//    }
    //先不立马更新界面,等全部更新好控件状态后再更新界面,否则会导致界面跳动
    setUpdatesEnabled(false);
    lvTxt->setVisible(lvCbx);
    cbx_lv->setVisible(lvCbx);


    appTxt->setVisible(appListCbx);
    cbx_app->setVisible(appListCbx);



    typeTxt->setVisible(typecbx);  // add by Airy
    typeCbx->setVisible(typecbx);  // add by Airy

    periodLabel->setVisible(period);
    //button的setVisible false会触发taborder到下一个可视控件,比如cbx_status,所以先设置button,再设置cbx_status可防止点击后时间筛选button后再切启动日志导致cbx_status自动进入tabfocus状态,但是这样会引起本窗口焦点重置,所以设置完后需要对loglist setfoucs
    for (int i = 0; i < 6; i++) {
        LogPeriodButton *pushBtn = static_cast<LogPeriodButton *>(m_btnGroup->button(i));
        pushBtn->setVisible(period);
    }
    statusTxt->setVisible(statusCbx);
    cbx_status->setVisible(statusCbx);
    //根据是否需要移动导出按钮移动到对应布局
    if (needMove) {
        hLayout_period->addWidget(exportBtn);
        hLayout_all->removeWidget(exportBtn);
    } else {
        hLayout_period->removeWidget(exportBtn);
        hLayout_all->addWidget(exportBtn);

    }
    resizeWidth();
    //先不立马更新界面,等全部更新好控件状态后再更新界面,否则会导致界面跳动
    setUpdatesEnabled(true);

    //    va_end(arg_ptr);  //清除可变参数指针
    cbx_lv->setObjectName("level_combox");
    cbx_lv->setAccessibleName("level_combox");
    cbx_app->setObjectName("app_combox");
    cbx_app->setAccessibleName("app_combox");
    cbx_status->setObjectName("status_combox");
    cbx_status->setAccessibleName("status_combox");
    typeCbx->setObjectName("event_type_combox");
    typeCbx->setAccessibleName("event_type_combox");
    // exportBtn->setFocus(Qt::MouseFocusReason);



}

/**
 * @brief FilterContent::setSelection 设置当前筛选器选择的状态
 * @param iConifg 控件的选择值的结构体
 */
void FilterContent::setSelection(FILTER_CONFIG iConifg)
{
    //控件不显示说明此情况下不需要给它设置值
    if (cbx_lv->isVisible())
        cbx_lv->setCurrentIndex(iConifg.levelCbx);
    if (cbx_app->isVisible()) {
        QString path = iConifg.appListCbx;
        int appCount =  cbx_app->count();
        int rsIndex = 0;

        if (!path.isEmpty()) {
            //找原来选的选项的index,如果这个应用日志没了,就选第一个
            for (int i = 0; i < appCount; ++i) {
                if (cbx_app->itemData(i, Qt::UserRole + 1).toString() == path) {
                    rsIndex = i;
                    break;
                }
            }
            //不能直接connect再setCurrentIndex,而是要手动发出改变app的信号让其刷新,让combox自己发的话,如果原来的index是0他不发currentindexChanged信号
        }
        if (rsIndex == 0) {
            Q_EMIT cbx_app->currentIndexChanged(0);
        } else {
            cbx_app->setCurrentIndex(rsIndex);
        }
    }

    if (cbx_status->isVisible())
        cbx_status->setCurrentIndex(iConifg.statusCbx);
    if (typeCbx->isVisible())
        typeCbx->setCurrentIndex(iConifg.typeCbx);
    m_btnGroup->button(iConifg.dateBtn)->setChecked(true); //add by Airy for bug 19660:period button default setting
    Q_EMIT m_btnGroup->button(iConifg.dateBtn)->click();
    //    Q_EMIT sigButtonClicked(iConifg.dateBtn,iConifg.levelCbx,QModelIndex());
}

/**
 * @brief FilterContent::setUeButtonSytle 时间筛选器重置为默认选择全部
 */
void FilterContent::setUeButtonSytle()
{
    for (QAbstractButton *abtn : m_btnGroup->buttons()) {
        LogPeriodButton *btn = static_cast<LogPeriodButton *>(abtn);

        if (btn->objectName() == "allBtn")
            btn->setChecked(true);
    }

}

/**
 * @brief FilterContent::paintEvent 绘制事件徐函数,主要用来绘制出筛选控件背景的圆角矩形效果
 * @param event 绘制事件
 */
void FilterContent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Save pen
    QPen oldPen = painter.pen();
    //设置画笔颜色角色,根据主题变色
    painter.setRenderHint(QPainter::Antialiasing);
    DPalette pa = DApplicationHelper::instance()->palette(this);
    painter.setBrush(QBrush(pa.color(DPalette::Base)));
    QColor penColor = pa.color(DPalette::FrameBorder);
    //设置透明度
    penColor.setAlphaF(0.05);
    painter.setPen(QPen(penColor));
    //算出背景圆角矩形的大小位置
    QRectF rect = this->rect();
    rect.setX(0.5);
    rect.setY(0.5);
    rect.setWidth(rect.width() - 0.5);
    rect.setHeight(rect.height() - 0.5);

    QPainterPath painterPath;
    //绘制圆角矩形
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

/**
 * @brief FilterContent::eventFilter 事件过滤器
 * @param obj 发生事件的对象
 * @param event 事件指针
 * @return true为截获事件,处理后不继续传递,否则继续传递下去
 */
bool FilterContent::eventFilter(QObject *obj, QEvent *event)
{
    //判断是否为键盘按下事件
    if (event->type() == QEvent::KeyPress) {
        if (obj == m_allBtn) {
            auto *kev = dynamic_cast<QKeyEvent *>(event);
            //左右按钮使焦点和选项变为两边的按钮,第一个按钮往左是最后一个按钮
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
            //最后一个按钮往右为第一个按钮,如此循环
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


/**
 * @brief FilterContent::resizeWidth 自适应宽度
 */
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

/**
 * @brief FilterContent::updateWordWrap 根据当前宽度省略日期筛选按钮文字以适应宽度，让控件能塞下
 */
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

/**
 * @brief FilterContent::updateDataState  每次日志类型变动或需要刷新时,按记录的数据刷新筛选器的选项
 */
void FilterContent::updateDataState()
{
    //如果没有记录当前日志的选项,则给一个默认的
    if (!m_config.contains(m_currentType)) {
        FILTER_CONFIG newConfig;
        m_config.insert(m_currentType, newConfig);
    }
    //按记录的筛选器选项还原控件选项
    setSelection(m_config.value(m_currentType));
}

/**
 * @brief FilterContent::setCurrentConfig 控件发生值改变时,改变当前类型的筛选器选项
 * @param iConifg 筛选器选项
 */
void FilterContent::setCurrentConfig(FILTER_CONFIG iConifg)
{
    m_config.insert(m_currentType, iConifg);
}

/**
 * @brief FilterContent::slot_logCatelogueClicked 连接日志种类listview  itemchanged信号,根据日志种类改变筛选控件布局
 * @param index  listview 当前选中index,用来确定日志种类
 */
void FilterContent::slot_logCatelogueClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    //获取日志种类
    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty()) {
        return;
    }

    m_curTreeIndex = index;
    //根据日志种类改变布局
    if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        m_currentType = APP_TREE_DATA;
        this->setAppComboBoxItem();
        this->setSelectorVisible(true, true, false, true, false);
        //   emit sigCbxAppIdxChanged(cbx_app->itemData(0, Qt::UserRole + 1).toString());
    } else if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
        m_currentType = JOUR_TREE_DATA;
        this->setSelectorVisible(true, false, false, true, false);
    } else if (itemData.contains(BOOT_TREE_DATA)) {
        m_currentType = BOOT_TREE_DATA;
        this->setSelectorVisible(false, false, true, false, false);
    } else if (itemData.contains(KERN_TREE_DATA)) {
        m_currentType = KERN_TREE_DATA;
        this->setSelectorVisible(false, false, false, true, true);
    } else if (itemData.contains(DPKG_TREE_DATA)) {
        m_currentType = DPKG_TREE_DATA;
        this->setSelectorVisible(false, false, false, true, true);
    } else if (itemData.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
        m_currentType = XORG_TREE_DATA;
        this->setSelectorVisible(false, false, false, true,
                                 true);  // modified by Airy for showing  peroid
    } else if (itemData.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {  // add by Airy
        m_currentType = LAST_TREE_DATA;
        this->setSelectorVisible(false, false, false, true, false,
                                 true);  // modifed by Airy for showing peroid
    } else if (itemData.contains(KWIN_TREE_DATA)) {
        m_currentType = KWIN_TREE_DATA;
        this->setSelectorVisible(false, false, false, false, false);
    } else if (itemData.contains(BOOT_KLU_TREE_DATA)) {
        m_currentType = BOOT_KLU_TREE_DATA;
        this->setSelectorVisible(true, false, false, false, false);
    }
//    cbx_lv->setFocusReason(Qt::NoFocusReason);
//    cbx_app->setFocusReason(Qt::NoFocusReason);
//    cbx_status->setFocusReason(Qt::NoFocusReason);
//    typeCbx->setFocusReason(Qt::NoFocusReason);
    //  cbx_lv->setFocus(Qt::MouseFocusReason);
    updateDataState();
    //必须需要,因为会丢失当前焦点顺序
    LogListView *logList =  qobject_cast<LogListView *>(sender());
    if (logList) {
        logList->setFocus();
    }

}

/**
 * @brief FilterContent::slot_logCatelogueRefresh 日志种类选择listview的右键菜单刷新时处理的槽函数
 * @param index 刷新对应的日志种类
 */
void FilterContent::slot_logCatelogueRefresh(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty()) {
        return;
    }
    //现在只需处理应用日志刷新时需要刷新应用选择下拉列表的数据
    if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        //记录当前选择项目以便改变combox内容后可以选择原来的选项刷新
        //  QString cuurentText = cbx_app->currentText();
        //先disconnect防止改变combox内容时发出currentIndexChanged让主表获取
        disconnect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)));
        this->setAppComboBoxItem();

        updateDataState();
        //第一次刷新手动发出选项变化信号
        emit sigCbxAppIdxChanged(m_config.value(m_currentType).dateBtn, cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString());
        connect(cbx_app, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_cbxAppIdxChanged(int)), Qt::UniqueConnection);
        // cbx_app->setCurrentText(cuurentText);
        //  qDebug() << "cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString()" << cbx_app->itemData(cbx_app->currentIndex(), Qt::UserRole + 1).toString();
    }
}

/**
 * @brief FilterContent::slot_buttonClicked 时间筛选buttongroup值变化处理槽函数
 * @param idx 当前选择的按钮
 */
void FilterContent::slot_buttonClicked(int idx)
{
    /** note: In order to adapt to the new scene, select time-period first,
     *        then select any log item, should display current log info.
     *        so comment this judge.
     */
    //    if (!m_curTreeIndex.isValid())
    //        return;
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    FILTER_CONFIG curConfig = m_config.value(m_currentType);
    curConfig.dateBtn = idx;
    setCurrentConfig(curConfig);
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
        //根据选择时间发出当前选项变化信号
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

/**
 * @brief FilterContent::slot_exportButtonClicked 导出按钮触发处理槽函数
 */
void FilterContent::slot_exportButtonClicked()
{
    QString itemData = m_curTreeIndex.data(ITEM_DATE_ROLE).toString();
    //当前日志种类不为空则发出导出数据信号给其他类处理
    if (!itemData.isEmpty())
        emit sigExportInfo();
}

/**
 * @brief FilterContent::slot_cbxLvIdxChanged 日志等级下拉框选择变化处理槽函数
 * @param idx 当前选择的选项下标
 */
void FilterContent::slot_cbxLvIdxChanged(int idx)
{
    m_curLvCbxId = idx - 1;
    FILTER_CONFIG curConfig = m_config.value(m_currentType);
    curConfig.levelCbx = idx;
    //变化时改变记录选择选项的数据结构,以便下次还原
    setCurrentConfig(curConfig);
    //发出信号以供数据显示控件刷新数据
    emit sigButtonClicked(m_curBtnId, m_curLvCbxId, m_curTreeIndex);
}

/**
 * @brief FilterContent::slot_cbxAppIdxChanged 应用选择下拉框选择变化处理槽函数
 * @param idx 当前选择的选项下标
 */
void FilterContent::slot_cbxAppIdxChanged(int idx)
{
    QString path = cbx_app->itemData(idx, Qt::UserRole + 1).toString();
    FILTER_CONFIG curConfig = m_config.value(m_currentType);
    qDebug() << "apppath" << path;
    curConfig.appListCbx = path;
    //变化时改变记录选择选项的数据结构,以便下次还原
    setCurrentConfig(curConfig);
    //发出信号以供数据显示控件刷新数据
    emit sigCbxAppIdxChanged(curConfig.dateBtn, path);
}

/**
 * @brief FilterContent::slot_cbxStatusChanged 启动日志状态选择下拉框选择变化处理槽函数
 * @param idx 当前选择的选项下标
 */
void FilterContent::slot_cbxStatusChanged(int idx)
{
    FILTER_CONFIG curConfig = m_config.value(m_currentType);
    curConfig.statusCbx = idx;
    //变化时改变记录选择选项的数据结构,以便下次还原
    setCurrentConfig(curConfig);
    QString str;
    if (idx == 1)
        str = "OK";
    else if (idx == 2)
        str = "Failed";
    //发出信号以供数据显示控件刷新数据
    emit sigStatusChanged(str);
}

/**
 * @brief FilterContent::slot_cbxLogTypeChanged 开关机日志日志种类选择下拉框选择变化处理槽函数
 * @param idx 当前选择的选项下标
 */
void FilterContent::slot_cbxLogTypeChanged(int idx)
{
    FILTER_CONFIG curConfig = m_config.value(m_currentType);
    curConfig.typeCbx = idx;
    setCurrentConfig(curConfig);
    emit sigLogtypeChanged(idx);
    qDebug() << "emit signal " + QString::number(idx);
}

/**
 * @brief FilterContent::setExportButtonEnable 导出按钮是否置灰
 * @param iEnable true 不置灰 false 置灰
 */
void FilterContent::setExportButtonEnable(bool iEnable)
{
    if (exportBtn) {
        exportBtn->setEnabled(iEnable);
    }
}
