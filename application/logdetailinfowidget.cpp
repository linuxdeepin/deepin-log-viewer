#include "logdetailinfowidget.h"
#include "model/log_sort_filter_proxy_model.h"
#include <sys/utsname.h>
#include <DApplication>
#include <DApplicationHelper>
#include <DGuiApplicationHelper>
#include <DFontSizeManager>
#include <DLabel>
#include <DStyle>
#include <DTextBrowser>
#include <QDebug>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QTreeView>
#include <QVBoxLayout>
#include "structdef.h"

DWIDGET_USE_NAMESPACE

#define LABEL_MIN_WIDTH 120

logDetailInfoWidget::logDetailInfoWidget(QWidget *parent)
    : DWidget(parent)
{
    initUI();
}

void logDetailInfoWidget::cleanText()
{
    m_dateTime->hide();

    m_userName->hide();
    m_userLabel->hide();

    m_pid->hide();
    m_pidLabel->hide();

    m_action->hide();
    m_actionLabel->hide();

    m_status->hide();
    m_statusLabel->hide();

    m_level->hide();

    m_daemonName->hide();

    m_textBrowser->clear();

    // add by Airy
    m_name->hide();
    m_nameLabel->hide();
    m_event->hide();
    m_eventLabel->hide();
}

void logDetailInfoWidget::hideLine(bool isHidden)
{
    m_hline->setHidden(isHidden);
}

void logDetailInfoWidget::initUI()
{
    // init pointer
    m_daemonName = new DLabel(this);
    QFont font;
    font.setBold(true);
    m_daemonName->setFont(font);
    DFontSizeManager::instance()->bind(m_daemonName, DFontSizeManager::T5);
    m_dateTime = new DLabel(this);
    DFontSizeManager::instance()->bind(m_dateTime, DFontSizeManager::T8);
    DPalette pa = DApplicationHelper::instance()->palette(m_dateTime);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_dateTime->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_dateTime, pa);

    m_userName = new DLabel(this);
    DFontSizeManager::instance()->bind(m_userName, DFontSizeManager::T8);
    m_userName->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_userName);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_userName->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_userName, pa);

    m_pid = new DLabel(this);
    DFontSizeManager::instance()->bind(m_pid, DFontSizeManager::T8);
    m_pid->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_pid);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_pid->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_pid, pa);

    m_action = new DLabel(this);
    DFontSizeManager::instance()->bind(m_action, DFontSizeManager::T8);
    m_action->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_action);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_action, pa);

    m_status = new DLabel(this);
    DFontSizeManager::instance()->bind(m_status, DFontSizeManager::T8);
    m_status->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_status);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_status, pa);

    m_level = new LogIconButton(this);
    DFontSizeManager::instance()->bind(m_level, DFontSizeManager::T8);
    pa = DApplicationHelper::instance()->palette(m_level);
    pa.setBrush(DPalette::ButtonText, pa.color(DPalette::TextTips));
    //    m_level->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_level, pa);

    // add by Airy
    m_event = new DLabel(this);
    DFontSizeManager::instance()->bind(m_event, DFontSizeManager::T8);
    m_event->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_event);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_event, pa);

    m_name = new DLabel(this);
    DFontSizeManager::instance()->bind(m_name, DFontSizeManager::T8);
    m_name->setMinimumWidth(LABEL_MIN_WIDTH);
    pa = DApplicationHelper::instance()->palette(m_name);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_name, pa);  // end

    m_userLabel = new DLabel(DApplication::translate("Label", "User:"), this);
    DFontSizeManager::instance()->bind(m_userLabel, DFontSizeManager::T7);
    m_pidLabel = new DLabel(DApplication::translate("Label", "PID:"), this);
    DFontSizeManager::instance()->bind(m_pidLabel, DFontSizeManager::T7);
    m_statusLabel = new DLabel(DApplication::translate("Label", "Status:"), this);
    DFontSizeManager::instance()->bind(m_statusLabel, DFontSizeManager::T7);
    m_actionLabel = new DLabel(DApplication::translate("Label", "Action:"), this);
    DFontSizeManager::instance()->bind(m_actionLabel, DFontSizeManager::T7);
    m_eventLabel =
        new DLabel(DApplication::translate("Label", "Event Type:"), this);          // add by Airy
    DFontSizeManager::instance()->bind(m_eventLabel, DFontSizeManager::T7);
    m_nameLabel = new DLabel(DApplication::translate("Label", "Username:"), this);  // add by Airy
    DFontSizeManager::instance()->bind(m_nameLabel, DFontSizeManager::T7);


    m_hline = new DHorizontalLine;

    m_textBrowser = new DTextBrowser(this);
    DFontSizeManager::instance()->bind(m_textBrowser, DFontSizeManager::T8);
    pa = DApplicationHelper::instance()->palette(m_textBrowser);
    pa.setBrush(DPalette::Text, pa.color(DPalette::TextTips));
    //    m_textBrowser->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_textBrowser, pa);
    m_textBrowser->setFrameShape(QFrame::NoFrame);
    m_textBrowser->viewport()->setAutoFillBackground(false);

    cleanText();

    QVBoxLayout *v = new QVBoxLayout;

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(m_daemonName);
    h1->addStretch(1);
    h1->addWidget(m_dateTime);

    QHBoxLayout *h2 = new QHBoxLayout;

    QHBoxLayout *h21 = new QHBoxLayout;
    h21->addWidget(m_userLabel);
    h21->addWidget(m_userName, 1);
    h21->setSpacing(0);
    QHBoxLayout *h22 = new QHBoxLayout;
    h22->addWidget(m_pidLabel);
    h22->addWidget(m_pid, 1);
    h22->setSpacing(0);
    QHBoxLayout *h23 = new QHBoxLayout;
    h23->addWidget(m_statusLabel);
    h23->addWidget(m_status, 1);
    h23->setSpacing(0);
    QHBoxLayout *h24 = new QHBoxLayout;
    h24->addWidget(m_actionLabel);
    h24->addWidget(m_action, 1);
    h24->setSpacing(0);

    // add by Airy
    QHBoxLayout *h25 = new QHBoxLayout;
    h25->addWidget(m_eventLabel);
    h25->addWidget(m_event, 1);
    h25->setSpacing(8);

    QHBoxLayout *h26 = new QHBoxLayout;
    h26->addWidget(m_nameLabel);
    h26->addWidget(m_name, 1);
    h26->setSpacing(8);

    // end

    h2->addLayout(h21);
    h2->addLayout(h22);
    h2->addLayout(h23);
    h2->addLayout(h24);
    h2->addLayout(h25);  // add by Airy
    h2->addLayout(h26);  // add by Airy
    h2->addStretch(1);
    h2->addWidget(m_level);
    h2->setSpacing(20);

    v->addLayout(h1);
    v->addLayout(h2);
    v->addWidget(m_hline);
    v->addWidget(m_textBrowser, 3);

    v->setContentsMargins(20, 10, 20, 0);
    v->setSpacing(4);

    this->setLayout(v);
}

void logDetailInfoWidget::setTextCustomSize(QWidget *w)
{
    DFontSizeManager::instance()->bind(w, DFontSizeManager::T8);
//    QFont font = w->font();
//    int size = font.pointSize();
//    font.setPointSize(size);
//    w->setFont(font);
}

void logDetailInfoWidget::paintEvent(QPaintEvent *event)
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
    QRectF clipRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRoundedRect(clipRect, 8, 8);
    subPath.addRect(subRect);
    clipPath = clipPath.subtracted(subPath);

    painter.fillPath(clipPath, QBrush(pa.color(DPalette::Base)));

    // Restore the pen
    painter.setPen(oldPen);

    DWidget::paintEvent(event);
}

//bool logDetailInfoWidget::event(QEvent *ev)
//{
//    if (ev->type() == QEvent::FontChange) {
//        setTextCustomSize(m_dateTime);
//    }
//    return  false;
//}



void logDetailInfoWidget::fillDetailInfo(QString deamonName, QString usrName, QString pid,
                                         QString dateTime, QModelIndex level, QString msg,
                                         QString status, QString action, QString uname,
                                         QString event)
{
    if (deamonName.isEmpty()) {
        m_daemonName->hide();
    } else {
        m_daemonName->show();
        m_daemonName->setText(deamonName);
    }
    if (!level.isValid()) {
        m_level->hide();
    } else {
        QIcon icon =  level.siblingAtColumn(0).data(Qt::DecorationRole).value<QIcon>();
        m_level->setIcon(icon);
        m_level->setText(level.siblingAtColumn(0).data(Qt::UserRole + 6).toString());
        m_level->show();
    }

    if (dateTime.isEmpty()) {
        m_dateTime->hide();
    } else {
        m_dateTime->show();
        QStringList dtlist = dateTime.split(".");
        if (dtlist.count() == 2)
            m_dateTime->setText(dtlist[0]);
        else
            m_dateTime->setText(dateTime);
    }

    if (usrName.isEmpty()) {
        m_userName->hide();
        m_userLabel->hide();
    } else {
        m_userName->show();
        m_userLabel->show();
        m_userName->setText(usrName);
    }

    if (pid.isEmpty()) {
        m_pid->hide();
        m_pidLabel->hide();
    } else {
        m_pid->show();
        m_pidLabel->show();
        m_pid->setText(pid);
    }

    if (status.isEmpty()) {
        m_status->hide();
        m_statusLabel->hide();
    } else {
        m_status->show();
        m_statusLabel->show();
        m_status->setText(status);
    }

    if (action.isEmpty()) {
        m_action->hide();
        m_actionLabel->hide();
    } else {
        m_action->show();
        m_actionLabel->show();
        m_action->setText(action);
    }

    // add by Airy
    if (uname.isEmpty()) {
        m_name->hide();
        m_nameLabel->hide();
    } else {
        m_name->show();        // add by Airy
        m_nameLabel->show();   // add by Airy
        m_name->setText(uname);
    }
    if (event.isEmpty()) {
        m_event->hide();
        m_eventLabel->hide();
    } else {
        m_event->show();       // add by Airy
        m_eventLabel->show();  // add by Airy
        m_event->setText(event);
    }
    // end

    m_textBrowser->setText(msg);
}

void logDetailInfoWidget::slot_DetailInfo(const QModelIndex &index, QStandardItemModel *pModel,
                                          QString name)
{
    cleanText();

    if (!index.isValid())
        return;

    if (nullptr == pModel)
        return;

    m_pModel = pModel;

    // get hostname.
    utsname _utsname;
    uname(&_utsname);
    QString hostname = QString(_utsname.nodename);

    QString dataStr = index.data(Qt::UserRole + 1).toString();
    index.row();
    if (dataStr.contains(DPKG_TABLE_DATA)) {

        fillDetailInfo("dnf", hostname, "", index.siblingAtColumn(0).data().toString(), QModelIndex(),
                       index.siblingAtColumn(1).data().toString(), "",
                       index.siblingAtColumn(2).data().toString());
    } else if (dataStr.contains(XORG_TABLE_DATA)) {
        fillDetailInfo("Xorg", hostname, "", index.siblingAtColumn(0).data().toString(), QModelIndex(),
                       index.siblingAtColumn(1).data().toString());
    } else if (dataStr.contains(BOOT_TABLE_DATA)) {
        fillDetailInfo("Boot", hostname, "", "", QModelIndex(),
                       index.siblingAtColumn(1).data().toString(),
                       index.siblingAtColumn(0).data().toString());
    } else if (dataStr.contains(KERN_TABLE_DATA)) {
        fillDetailInfo(index.siblingAtColumn(2).data().toString(),
                       /*m_pModel->item(index.row(), 1)->text()*/ hostname, "",
                       index.siblingAtColumn(0).data().toString(), QModelIndex(),
                       index.siblingAtColumn(3).data().toString());
    } else if (dataStr.contains(JOUR_TABLE_DATA)) {
        fillDetailInfo(index.siblingAtColumn(1).data().toString(),
                       /*m_pModel->item(index.row(), 4)->text()*/ hostname,
                       index.siblingAtColumn(5).data().toString(),
                       index.siblingAtColumn(2).data().toString(), index,
                       index.siblingAtColumn(3).data().toString());
    } else if (dataStr.contains(APP_TABLE_DATA)) {
        fillDetailInfo(name, hostname, "", index.siblingAtColumn(1).data().toString(), index,
                       index.siblingAtColumn(3).data().toString());
    } else if (dataStr.contains(LAST_TABLE_DATA)) {
        //        fillDetailInfo("Event", m_pModel->item(index.row(), 0)->text(), "",
        //                       m_pModel->item(index.row(), 2)->text(), QModelIndex(),
        //                       m_pModel->item(index.row(), 4)->text());

        QString str = "";
        QString typeStr = index.siblingAtColumn(0).data().toString();
        if (typeStr.compare("Boot") == 0) {
            str = DApplication::translate("Label", "Boot record");
        } else if (typeStr.compare("shutdown") == 0) {
            str = DApplication::translate("Label", "Shutdown record");
        } else if (typeStr.compare("shutdown") != 0 &&
                   typeStr.compare("Boot") != 0) {
            str = DApplication::translate("Label", "Login record");
        }
        fillDetailInfo(str, hostname, "", index.siblingAtColumn(2).data().toString(), QModelIndex(),
                       index.siblingAtColumn(3).data().toString(), "", "",
                       index.siblingAtColumn(1).data().toString(),
                       index.siblingAtColumn(0).data().toString());
        // modified by Airy
    } else if (dataStr.contains(KWIN_TABLE_DATA)) {
        fillDetailInfo("Kwin", hostname, "", "", QModelIndex(),
                       index.siblingAtColumn(0).data().toString());
    } else if (dataStr.contains(DNF_TABLE_DATA)) {
        fillDetailInfo("dnf", hostname, "", "", index,
                       index.siblingAtColumn(2).data().toString());
    }
}
