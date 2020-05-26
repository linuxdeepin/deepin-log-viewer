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

#include <DApplication>
#include <QDebug>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMargins>
#include <QPaintEvent>
#include <QPainter>
#include <QProcess> //add by Airy
#include <QDir>
#include "loglistview.h"
#include "logapplicationhelper.h"
#define ITEM_HEIGHT 40
#define ITEM_WIDTH 108

#define ICON_DATA (Qt::UserRole + 99)

Q_DECLARE_METATYPE(QMargins)

DWIDGET_USE_NAMESPACE

LogListView::LogListView(QWidget *parent)
    : DListView(parent)
{
    initUI();

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &LogListView::onChangedTheme);
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void LogListView::initUI()
{
    this->setMinimumWidth(150);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setItemSpacing(0);
    this->setViewportMargins(10, 10, 10, 0);

    const QMargins ListViweItemMargin(5, 0, 0, 0);
    const QVariant VListViewItemMargin = QVariant::fromValue(ListViweItemMargin);

    m_pModel = new QStandardItemModel(this);
    QStandardItem *item = nullptr;
    if (isFileExist("/var/log/journal")) {
        item = new QStandardItem(DApplication::translate("Tree", "System Log"));
        item->setToolTip(DApplication::translate("Tree", "System Log"));  // add by Airy for bug 16245
        item->setData(JOUR_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }

    if (isFileExist("/var/log/kern.log")) {
        item = new QStandardItem(DApplication::translate("Tree", "Kernel Log"));
        item->setToolTip(DApplication::translate("Tree", "Kernel Log"));  // add by Airy for bug 16245
        item->setData(KERN_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }

    if (isFileExist("/var/log/boot.log")) {
        item = new QStandardItem(DApplication::translate("Tree", "Boot Log"));
        item->setToolTip(DApplication::translate("Tree", "Boot Log"));  // add by Airy for bug 16245
        item->setData(BOOT_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }
    if (isFileExist("/var/log/dpkg.log")) {
        item = new QStandardItem(DApplication::translate("Tree", "dpkg Log"));
        item->setToolTip(DApplication::translate("Tree", "dpkg Log"));  // add by Airy for bug 16245
        item->setData(DPKG_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }
    if (isFileExist("/var/log/Xorg.0.log")) {
        item = new QStandardItem(DApplication::translate("Tree", "Xorg Log"));
        item->setToolTip(DApplication::translate("Tree", "Xorg Log"));  // add by Airy for bug 16245
        item->setData(XORG_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }
    if (isFileExist(QDir::homePath() + "/.kwin.log")) {
        item = new QStandardItem(DApplication::translate("Tree", "Kwin Log"));
        item->setToolTip(DApplication::translate("Tree", "Kwin Log"));
        item->setData(KWIN_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
    }
    auto *appHelper = LogApplicationHelper::instance();
    QMap<QString, QString> appMap = appHelper->getMap();
    if (!appMap.isEmpty()) {
        item = new QStandardItem(DApplication::translate("Tree", "Application Log"));
        item->setToolTip(
            DApplication::translate("Tree", "Application Log"));  // add by Airy for bug 16245
        item->setData(APP_TREE_DATA, ITEM_DATE_ROLE);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_pModel->appendRow(item);
        this->setModel(m_pModel);
    }


// add by Airy

    if (isFileExist("/var/log/wtmp")) {
        item = new QStandardItem(DApplication::translate("Tree", "Boot-Shutdown Event"));
        item->setData(LAST_TREE_DATA, ITEM_DATE_ROLE);
        item->setToolTip(
            DApplication::translate("Tree", "Boot-Shutdown Event"));  // add by Airy for bug 16245
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
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
    emit clicked(currentIndex());
}

void LogListView::setCustomFont(QStandardItem *item)
{
    //    QFont font = item->font();
    //    font.setPointSize(11);
    //    item->setFont(font);
    this->setIconSize(QSize(16, 16));

    //    item->setTextAlignment(Qt::AlignCenter);
}

bool LogListView::isFileExist(const QString &iFile)
{
    QFile file(iFile);
    return file.exists();
}

void LogListView::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{

    icon = ((themeType == DGuiApplicationHelper::LightType) ? ICONLIGHTPREFIX : ICONDARKPREFIX);

    QString _itemIcon;

    QModelIndex idx = this->currentIndex();

    QStandardItem *currentItem = m_pModel->itemFromIndex(idx);

    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);

        if (item) {
            setCustomFont(item);
            if (item->data(ITEM_DATE_ROLE).toString() == JOUR_TREE_DATA) {
                _itemIcon = icon + "system.svg";
            } else if (item->data(ITEM_DATE_ROLE).toString() == KERN_TREE_DATA) {
                _itemIcon = icon + "core.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == BOOT_TREE_DATA) {
                _itemIcon = icon + "start.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == DPKG_TREE_DATA) {
                _itemIcon = icon + "d.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == XORG_TREE_DATA) {
                _itemIcon = icon + "x.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == APP_TREE_DATA) {
                _itemIcon = icon + "application.svg";
            } else if (item->data(ITEM_DATE_ROLE).toString() == LAST_TREE_DATA) {
                _itemIcon = icon + "start.svg";
            } else if (item->data(ITEM_DATE_ROLE).toString() == KWIN_TREE_DATA) {
                _itemIcon = icon + "kwin.svg";
            }
            if (currentItem != nullptr && item == currentItem) {
                _itemIcon.replace(".svg", "_checked.svg");
            }
            item->setIcon(QIcon(_itemIcon));
            item->setData(_itemIcon, ICON_DATA);
        }
    }

    // set itembackground color
    //    DPalette pa = DApplicationHelper::instance()->palette(this);
    //    pa.setBrush(DPalette::ItemBackground, pa.color(DPalette::Base));
    //    pa.setBrush(DPalette::Background, pa.color(DPalette::Base));

    //    this->setPalette(pa);
    //    DApplicationHelper::instance()->setPalette(this, pa);

    //    this->setAutoFillBackground(true);
}

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

void LogListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QStandardItem *currentItem = m_pModel->itemFromIndex(current);
    if (currentItem) {
        QString icon = currentItem->data(ICON_DATA).toString();
        if (icon.isEmpty() == false) {
            icon.replace(".svg", "_checked.svg");
            currentItem->setIcon(QIcon(icon));
            currentItem->setData(icon, ICON_DATA);
        }
    }

    QStandardItem *previousItem = m_pModel->itemFromIndex(previous);
    if (previousItem) {
        QString icon = previousItem->data(ICON_DATA).toString();
        if (icon.isEmpty() == false) {
            icon.replace("_checked", "");
            previousItem->setIcon(QIcon(icon));
            previousItem->setData(icon, ICON_DATA);
        }
    }

    emit itemChanged();
    DListView::currentChanged(current, previous);
}

/**
 * @author Airy
 * @brief LogListView::truncateFile for truncating file
 * @param path_
 */
void LogListView::truncateFile(QString path_)
{
    QProcess prc;
    if (path_ == KERN_TREE_DATA || path_ == BOOT_TREE_DATA || path_ == DPKG_TREE_DATA || path_ == XORG_TREE_DATA || path_ == KWIN_TREE_DATA) {
        prc.start("pkexec", QStringList() << "truncate"
                  << "-s"
                  << "0"
                  << path_);
    } else {
        prc.start("truncate", QStringList() << "-s"
                  << "0"
                  << path_);
    }

    prc.waitForFinished();
}

/**
 * @author Airy
 * @brief LogListView::slot_getAppPath for get app path
 * @param path
 */
void LogListView::slot_getAppPath(QString path)
{
    g_path = path;
}

#include <DDesktopServices>
#include <DDialog>
#include <DDialogButtonBox>
#include <DInputDialog>
#include <QDir>
#include <QMenu>
/**
 * @author Airy
 * @brief LogListView::contextMenuEvent for adding context Menu
 * @param event
 */
void LogListView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
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

        if (pathData == JOUR_TREE_DATA || pathData == LAST_TREE_DATA) {
            g_clear->setEnabled(false);
            g_openForder->setEnabled(false);
        }

        QString dirPath = QDir::homePath();
        QString _path_ = g_path;      //get app path
        QString path = "";


        if (pathData == KERN_TREE_DATA || pathData == BOOT_TREE_DATA || pathData == DPKG_TREE_DATA || pathData == XORG_TREE_DATA || pathData == KWIN_TREE_DATA) {
            path = pathData;
        } else if (pathData == APP_TREE_DATA) {
            //                    path = dirPath + QString("/.cache/deepin/.");
            path = _path_;
        }

        connect(g_openForder, &QAction::triggered, this, [ = ] {
            DDesktopServices::showFileItem(path);
        });

        QModelIndex index = idx;
        connect(g_refresh, &QAction::triggered, this, [ = ]() {
            emit sigRefresh(index);
        });

        connect(g_clear, &QAction::triggered, this, [ = ]() {

            DDialog *dialog = new DDialog(this);
            dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
            dialog->setIcon(QIcon::fromTheme("dialog-warning"));
            dialog->setMessage(/*"清除日志内容"*/DApplication::translate("Action", "Are you sure you want to clear the log?"));
            dialog->addButton(QString(/*tr("取消")*/DApplication::translate("Action", "Cancel")), false, DDialog::ButtonNormal);
            dialog->addButton(QString(/*tr("确定")*/DApplication::translate("Action", "Confirm")), true, DDialog::ButtonRecommend);
//            dialog->setModal(true);
//            dialog->show(); //modal

            int Ok = dialog->exec();
            if (Ok == DDialog::Accepted) {
                truncateFile(path);
                emit sigRefresh(index);
            }
        });

        this->setContextMenuPolicy(Qt::DefaultContextMenu);
        g_context->exec(QCursor::pos());
    }
//    this->selectionModel()->clear();  // if Cursor is empty,clear the context Menu
}
