/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:    LZ <zhou.lu@archermind.com>
*
* Maintainer:  LZ <zhou.lu@archermind.com>
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

#include "loglistview.h"
#include "logapplicationhelper.h"
#include "dbusmanager.h"
#include "DebugTimeManager.h"

#include <DDesktopServices>
#include <DDialog>
#include <DDialogButtonBox>
#include <DInputDialog>
#include <DApplication>
#include <DStyle>
#include <DApplication>
#include <DSysInfo>

#include <QItemSelectionModel>
#include <QMargins>
#include <QPaintEvent>
#include <QStandardItemModel>
#include <QPainter>
#include <QProcess> //add by Airy
#include <QDebug>
#include <QHeaderView>
#include <QToolTip>
#include <QDir>
#include <QDir>
#include <QMenu>
#include <QShortcut>
#include <QAbstractButton>
#define ITEM_HEIGHT 40
#define ITEM_WIDTH 108
#define ICON_SIZE 16

#define ICON_DATA (Qt::UserRole + 99)

Q_DECLARE_METATYPE(QMargins)

DWIDGET_USE_NAMESPACE

LogListDelegate::LogListDelegate(LogListView *parent) : DStyledItemDelegate(parent)
{

}

void LogListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    DStyledItemDelegate::paint(painter, option, index);
    // if (option.state & QStyle::State_HasFocus) {
    LogListView *parentView = qobject_cast<LogListView *>(this->parent());
    if ((option.state & QStyle::State_HasFocus) && parentView && (parentView->focusReson() == Qt::TabFocusReason || parentView->focusReson() == Qt::BacktabFocusReason) && (parentView->hasFocus())) {
        // draw focus
        auto *style = dynamic_cast<DStyle *>(DApplication::style());
        QRect rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());

        //    QRect backgroundRect = QRect(rect.left() + 10, rect.top(), rect.width() - 20, rect.height());
        if (index.isValid()) {
            QStyleOptionFocusRect o;
            o.QStyleOption::operator=(option);
            o.state |= QStyle::State_KeyboardFocusChange | QStyle::State_HasFocus;
            o.rect = style->visualRect(Qt::LeftToRight, option.rect, option.rect);
            style->drawPrimitive(DStyle::PE_FrameFocusRect, &o, painter);
        }
    }



}

/**
 * @brief LogListDelegate::helpEvent 重写helpEvent以让tooltip能在鼠标移出item项目后正确消失
 * @param event
 * @param view
 * @param option
 * @param index
 * @return
 */
bool LogListDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QToolTip::hideText();
    if (event->type() == QEvent::ToolTip) {
        const QString tooltip = index.data(Qt::DisplayRole).toString();
        //qDebug() << __FUNCTION__ << "__now Hover is :__" << tooltip;
        //如果tooltip不为空且合法则显示，否则直接关闭所有tooltip
        if (tooltip.isEmpty() || tooltip == "_split_") {
            hideTooltipImmediately();
        } else {
            int tooltipsize = tooltip.size();
            const int nlong = 32;
            int lines = tooltipsize / nlong + 1;
            QString strtooltip;
            for (int i = 0; i < lines; ++i) {
                strtooltip.append(tooltip.mid(i * nlong, nlong));
                strtooltip.append("\n");
            }
            strtooltip.chop(1);
            QToolTip::showText(event->globalPos(), strtooltip, view);
        }
        return false;
    }
    return DStyledItemDelegate::helpEvent(event, view, option, index);
}


/**
 * @brief LogListDelegate::hideTooltipImmediately 隐藏所有tooltip
 */
void LogListDelegate::hideTooltipImmediately()
{
    QWidgetList qwl = QApplication::topLevelWidgets();
    for (QWidget *qw : qwl) {
        if (QStringLiteral("QTipLabel") == qw->metaObject()->className()) {
            qw->close();
        }
    }
}

LogListView::LogListView(QWidget *parent)
    : DListView(parent)
{
    initUI();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &LogListView::customContextMenuRequested, this, &LogListView::requestshowRightMenu);
    //    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
    //            &LogListView::onChangedTheme);
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    //    onChangedTheme(ct);
    //    setFocusPolicy(Qt::TabFocus);

    m_rightClickTriggerShortCut = new QShortcut(this);
    m_rightClickTriggerShortCut->setKey(Qt::ALT + Qt::Key_M);
    m_rightClickTriggerShortCut->setContext(Qt::WidgetShortcut);
    m_rightClickTriggerShortCut->setAutoRepeat(false);
    connect(m_rightClickTriggerShortCut, &QShortcut::activated, this, [this] {
        QRect r = rectForIndex(this->currentIndex());
        showRightMenu(QPoint(r.x() + r.width() / 2, r.y() + r.height() / 2), true);
    });
}


/**
 * @brief LogListView::initUI 设置基本属性，且本listview为固定的种类，所以在此初始化函数中根据日志文件是否存在动态显示日志种类
 */
void LogListView::initUI()
{
    this->setMinimumWidth(150);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setItemDelegate(new LogListDelegate(this));
    this->setItemSpacing(0);
    this->setViewportMargins(10, 10, 10, 0);
    const QMargins ListViweItemMargin(15, 0, 5, 0);
    const QVariant VListViewItemMargin = QVariant::fromValue(ListViweItemMargin);
    Dtk::Core::DSysInfo::UosEdition edition = Dtk::Core::DSysInfo::uosEditionType();
    //等于服务器行业版或欧拉版(centos)
    bool isCentos = Dtk::Core::DSysInfo::UosEuler == edition || Dtk::Core::DSysInfo::UosEnterpriseC == edition;
    m_pModel = new QStandardItemModel(this);
    QStandardItem *item = nullptr;
    QString  systemName =   DBusManager::getSystemInfo();
    qDebug() << "systemName" << systemName;
    if (isFileExist("/var/log/journal")) {
        item = new QStandardItem(QIcon::fromTheme("dp_system"), DApplication::translate("Tree", "System Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "System Log"));  // add by Airy for bug 16245
        item->setData(JOUR_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("System Log");
        m_pModel->appendRow(item);
    }

    if (isCentos) {
        item = new QStandardItem(QIcon::fromTheme("dp_core"), DApplication::translate("Tree", "Kernel Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "Kernel Log"));  // add by Airy for bug 16245
        item->setData(DMESG_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("dmesg Log");
        m_pModel->appendRow(item);
    } else {
        if (isFileExist("/var/log/kern.log")) {
            item = new QStandardItem(QIcon::fromTheme("dp_core"), DApplication::translate("Tree", "Kernel Log"));
            setIconSize(QSize(ICON_SIZE, ICON_SIZE));
            item->setToolTip(DApplication::translate("Tree", "Kernel Log")); // add by Airy for bug 16245
            item->setData(KERN_TREE_DATA, ITEM_DATE_ROLE);
            item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
            item->setData(VListViewItemMargin, Dtk::MarginsRole);
            item->setAccessibleText("Kernel Log");
            m_pModel->appendRow(item);
        }
    }
    if (systemName == "klu" || systemName == "panguV" || systemName == "W515 PGUV-WBY0" || systemName.toUpper().contains("PGUV") || systemName.toUpper().contains("PANGUV") || systemName.toUpper().contains("KLU")) {
        item = new QStandardItem(QIcon::fromTheme("dp_start"), DApplication::translate("Tree", "Boot Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "Boot Log"));  // add by Airy for bug 16245
        item->setData(BOOT_KLU_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("Boot Klu Log");
        m_pModel->appendRow(item);
    } else {
        item = new QStandardItem(QIcon::fromTheme("dp_start"), DApplication::translate("Tree", "Boot Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "Boot Log"));  // add by Airy for bug 16245
        item->setData(BOOT_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("Boot Log");
        m_pModel->appendRow(item);
    }
    if (isCentos) {
        item = new QStandardItem(QIcon::fromTheme("dp_d"), DApplication::translate("Tree", "dpkg Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "dnf Log"));
        item->setData(DNF_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("dnf Log");
        m_pModel->appendRow(item);
    } else {
        if (isFileExist("/var/log/dpkg.log")) {
            item = new QStandardItem(QIcon::fromTheme("dp_d"), DApplication::translate("Tree", "dpkg Log"));
            setIconSize(QSize(ICON_SIZE, ICON_SIZE));
            item->setToolTip(DApplication::translate("Tree", "dpkg Log")); // add by Airy for bug 16245
            item->setData(DPKG_TREE_DATA, ITEM_DATE_ROLE);
            item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
            item->setData(VListViewItemMargin, Dtk::MarginsRole);
            item->setAccessibleText("dpkg Log");
            m_pModel->appendRow(item);
        }
    }
    // if (isFileExist(QDir::homePath() + "/.kwin.log")) {
    //w515是新版本内核的panguv返回值  panguV是老版本
    if (systemName == "klu" || systemName == "panguV" || systemName == "W515 PGUV-WBY0" || systemName == "pangu" || systemName.toUpper().contains("PGUV") || systemName.toUpper().contains("PANGUV") || systemName.toUpper().contains("KLU") || systemName.toUpper().contains("PANGU")) {
        item = new QStandardItem(QIcon::fromTheme("dp_kwin"), DApplication::translate("Tree", "Kwin Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "Kwin Log"));
        item->setData(KWIN_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("dpkg Log");
        m_pModel->appendRow(item);
    } else {
        item = new QStandardItem(QIcon::fromTheme("dp_x"), DApplication::translate("Tree", "Xorg Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(DApplication::translate("Tree", "Xorg Log"));  // add by Airy for bug 16245
        item->setData(XORG_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("Xorg Log");
        m_pModel->appendRow(item);
    }
    auto *appHelper = LogApplicationHelper::instance();
    QMap<QString, QString> appMap = appHelper->getMap();
    if (!appMap.isEmpty()) {
        item = new QStandardItem(QIcon::fromTheme("dp_application"), DApplication::translate("Tree", "Application Log"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setToolTip(
            DApplication::translate("Tree", "Application Log"));  // add by Airy for bug 16245
        item->setData(APP_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("Application Log");
        m_pModel->appendRow(item);
        this->setModel(m_pModel);
    }
// add by Airy
    if (isFileExist("/var/log/wtmp")) {
        item = new QStandardItem(QIcon::fromTheme("dp_onoff"), DApplication::translate("Tree", "Boot-Shutdown Event"));
        setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        item->setData(LAST_TREE_DATA, ITEM_DATE_ROLE);
        item->setToolTip(
            DApplication::translate("Tree", "Boot-Shutdown Event"));  // add by Airy for bug 16245
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setAccessibleText("Boot-Shutdown Event");
        m_pModel->appendRow(item);
        this->setModel(m_pModel);
    }
// set first item is select when app start
    if (m_pModel->rowCount() > 0) {
        this->setCurrentIndex(m_pModel->index(0, 0));
    }
}

void LogListView::setDefaultSelect()
{
    setCurrentIndex(currentIndex());
    itemChanged(currentIndex());
}

/**
 * @brief LogListView::isFileExist 判断文件路径是否存在
 * @param iFile 要判断的文件路径字符串
 * @return
 */
bool LogListView::isFileExist(const QString &iFile)
{
    QFile file(iFile);
    return file.exists();
}

/**
 * @brief LogListView::paintEvent 绘制背景颜色为全是base角色
 * @param event
 */
void LogListView::paintEvent(QPaintEvent *event)
{
    DPalette pa = DApplicationHelper::instance()->palette(this);
    pa.setBrush(DPalette::ItemBackground, pa.color(DPalette::Base));
    pa.setBrush(DPalette::Background, pa.color(DPalette::Base));
    this->setPalette(pa);
    DApplicationHelper::instance()->setPalette(this, pa);

    this->setAutoFillBackground(true);

    DListView::paintEvent(event);
}

/**
 * @brief LogListView::currentChanged 虚函数中变换图标的选中状态
 * @param current
 * @param previous
 */
void LogListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    PERF_PRINT_BEGIN("POINT-03", "");
    if (current.row() < 0) {
        return;
    }

    emit itemChanged(current);
    DListView::currentChanged(current, previous);
}

/**
 * @author Airy
 * @brief LogListView::truncateFile 清空日志文件内容
 * @param path_
 */
void LogListView::truncateFile(QString path_)
{
    QProcess prc;
    if (path_ == KERN_TREE_DATA || path_ == BOOT_TREE_DATA || path_ == DPKG_TREE_DATA || path_ == XORG_TREE_DATA || path_ == KWIN_TREE_DATA || path_ == DNF_TREE_DATA || path_ == DMESG_TREE_DATA) {
        prc.start("pkexec", QStringList() << "logViewerTruncate" << path_.append("*"));
    } else {
        QStringList arg;
        arg << "-c" << QString("truncate -s 0 %1").arg(path_.append("*"));
        prc.start("/bin/bash", arg);
    }

    prc.waitForFinished();
}


/**
 * @author Airy
 * @brief LogListView::slot_getAppPath  清空应用日志时，当前显示应用日志的目录由外部提供并赋予成员变量
 * @param path 要设置的当前应用路径
 */
void LogListView::slot_getAppPath(int id, QString path)
{
    Q_UNUSED(id);
    g_path = path;
}

Qt::FocusReason LogListView::focusReson()
{
    return  m_reson;
}

void LogListView::showRightMenu(const QPoint &pos, bool isUsePoint)
{
    qDebug() << __FUNCTION__;
    QModelIndex idx = this->currentIndex();
    QString pathData = idx.data(ITEM_DATE_ROLE).toString();
    if (!this->selectionModel()->selectedIndexes().empty()) {
        g_context = new QMenu(this);
        g_openForder = new QAction(/*tr("在文件管理器中显示")*/DApplication::translate("Action", "Display in file manager"), this);
        g_clear = new QAction(/*tr("清除日志内容")*/DApplication::translate("Action", "Clear log"), this);
        g_refresh = new QAction(/*tr("刷新")*/DApplication::translate("Action", "Refresh"), this);

        g_context->addAction(g_openForder);
        g_context->addAction(g_clear);
        g_context->addAction(g_refresh);

        if (pathData == JOUR_TREE_DATA || pathData == LAST_TREE_DATA || pathData == BOOT_KLU_TREE_DATA) {
            g_clear->setEnabled(false);
            g_openForder->setEnabled(false);
        }
        if (pathData == DMESG_TREE_DATA) {
            g_openForder->setEnabled(false);
        }
        QString dirPath = QDir::homePath();
        QString _path_ = g_path;      //get app path
        QString path = "";

        if (pathData == KERN_TREE_DATA || pathData == BOOT_TREE_DATA || pathData == DPKG_TREE_DATA || pathData == XORG_TREE_DATA || pathData == KWIN_TREE_DATA || pathData == DNF_TREE_DATA || pathData == DMESG_TREE_DATA) {
            path = pathData;
        } else if (pathData == APP_TREE_DATA) {
            path = _path_;
        }
        //显示当前日志目录
        connect(g_openForder, &QAction::triggered, this, [ = ] {
            DDesktopServices::showFileItem(path);
        });

        QModelIndex index = idx;
        //刷新逻辑
        connect(g_refresh, &QAction::triggered, this, [ = ]() {
            emit sigRefresh(index);
        });

        //清除日志逻辑
        connect(g_clear, &QAction::triggered, this, [ = ]() {

            DDialog *dialog = new DDialog(this);
            dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
            dialog->setIcon(QIcon::fromTheme("dialog-warning"));
            dialog->setMessage(/*"清除日志内容"*/DApplication::translate("Action", "Are you sure you want to clear the log?"));
            dialog->addButton(QString(/*tr("取消")*/DApplication::translate("Action", "Cancel")), false, DDialog::ButtonNormal);
            dialog->addButton(QString(/*tr("确定")*/DApplication::translate("Action", "Confirm")), true, DDialog::ButtonRecommend);
            dialog->getButton(0)->setAccessibleName("clear_confirm_btn");
            dialog->getButton(1)->setAccessibleName("clear_cancel_btn");
            dialog->setAccessibleName("clear_log_dialog");
            int Ok = dialog->exec();
            if (Ok == DDialog::Accepted) {
                truncateFile(path);
                emit sigRefresh(index);
            }
        });

        this->setContextMenuPolicy(Qt::CustomContextMenu);
        QPoint p;
        if (isUsePoint) {
            p = mapToGlobal(pos);
        } else {
            p = QCursor::pos();
        }
        g_context->exec(p);

    }
}

void LogListView::requestshowRightMenu(const QPoint &pos)
{
    if (this->indexAt(pos).isValid()) {
        showRightMenu(pos, false);
    }
}


/**
 * @author Airy
 * @brief LogListView::contextMenuEvent 显示右键菜单
 * @param event
 */
//void LogListView::contextMenuEvent(QContextMenuEvent *event)
//{
//    Q_UNUSED(event);
//    QModelIndex idx = this->currentIndex();
//    QString pathData = idx.data(ITEM_DATE_ROLE).toString();
//    if (!this->selectionModel()->selectedIndexes().empty()) {

//        g_context = new QMenu(this);
//        g_openForder = new QAction(/*tr("在文件管理器中显示")*/DApplication::translate("Action", "Display in file manager"), this);
//        g_clear = new QAction(/*tr("清除日志内容")*/DApplication::translate("Action", "Clear log"), this);
//        g_refresh = new QAction(/*tr("刷新")*/DApplication::translate("Action", "Refresh"), this);

//        g_context->addAction(g_openForder);
//        g_context->addAction(g_clear);
//        g_context->addAction(g_refresh);

//        if (pathData == JOUR_TREE_DATA || pathData == LAST_TREE_DATA || pathData == BOOT_KLU_TREE_DATA) {
//            g_clear->setEnabled(false);
//            g_openForder->setEnabled(false);
//        }

//        QString dirPath = QDir::homePath();
//        QString _path_ = g_path;      //get app path
//        QString path = "";


//        if (pathData == KERN_TREE_DATA || pathData == BOOT_TREE_DATA || pathData == DPKG_TREE_DATA || pathData == XORG_TREE_DATA || pathData == KWIN_TREE_DATA) {
//            path = pathData;
//        } else if (pathData == APP_TREE_DATA) {
//            path = _path_;
//        }
//        //显示当前日志目录
//        connect(g_openForder, &QAction::triggered, this, [ = ] {
//            DDesktopServices::showFileItem(path);
//        });

//        QModelIndex index = idx;
//        //刷新逻辑
//        connect(g_refresh, &QAction::triggered, this, [ = ]() {
//            emit sigRefresh(index);
//        });

//        //清除日志逻辑
//        connect(g_clear, &QAction::triggered, this, [ = ]() {

//            DDialog *dialog = new DDialog(this);
//            dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
//            dialog->setIcon(QIcon::fromTheme("dialog-warning"));
//            dialog->setMessage(/*"清除日志内容"*/DApplication::translate("Action", "Are you sure you want to clear the log?"));
//            dialog->addButton(QString(/*tr("取消")*/DApplication::translate("Action", "Cancel")), false, DDialog::ButtonNormal);
//            dialog->addButton(QString(/*tr("确定")*/DApplication::translate("Action", "Confirm")), true, DDialog::ButtonRecommend);

//            int Ok = dialog->exec();
//            if (Ok == DDialog::Accepted) {
//                truncateFile(path);
//                emit sigRefresh(index);
//            }
//        });

//        this->setContextMenuPolicy(Qt::DefaultContextMenu);
//        g_context->exec(mapToGlobal(event->pos()));

//    }
//}

void LogListView::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (QToolTip::isVisible()) {
        QToolTip::hideText();
    }
    return;
}

void LogListView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up) {
        if (currentIndex().row() == 0) {
            QModelIndex modelIndex = model()->index(model()->rowCount() - 1, 0);
            setCurrentIndex(modelIndex);
        } else {
            DListView::keyPressEvent(event);
        }
    } else if (event->key() == Qt::Key_Down) {
        if (currentIndex().row() == model()->rowCount() - 1) {
            QModelIndex modelIndex =  model()->index(0, 0);
            setCurrentIndex(modelIndex);
        } else {
            DListView::keyPressEvent(event);
        }
    } else {
        DListView::keyPressEvent(event);
    }

}

void LogListView::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::RightButton) {
        emit clicked(indexAt(event->pos()));
    }
    DListView::mousePressEvent(event);
}

void LogListView::focusInEvent(QFocusEvent *event)
{

    if ((event->reason() != Qt::PopupFocusReason) && (event->reason() != Qt::ActiveWindowFocusReason)) {
        m_reson = event->reason();
    }
    DListView::focusInEvent(event);
}

void LogListView::focusOutEvent(QFocusEvent *event)
{
    DListView::focusOutEvent(event);
}



