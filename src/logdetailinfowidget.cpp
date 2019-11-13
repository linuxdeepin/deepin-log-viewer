#include "logdetailinfowidget.h"
#include <sys/utsname.h>
#include <DApplication>
#include <DApplicationHelper>
#include <DGuiApplicationHelper>
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

    m_dateTime = new DLabel(this);
    DPalette pa = DApplicationHelper::instance()->palette(m_dateTime);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_dateTime->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_dateTime, pa);

    m_userName = new DLabel(this);
    pa = DApplicationHelper::instance()->palette(m_userName);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_userName->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_userName, pa);

    m_pid = new DLabel(this);
    pa = DApplicationHelper::instance()->palette(m_pid);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_pid->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_pid, pa);

    m_action = new DLabel(this);
    pa = DApplicationHelper::instance()->palette(m_action);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_action, pa);

    m_status = new DLabel(this);
    pa = DApplicationHelper::instance()->palette(m_status);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    //    m_status->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_status, pa);

    m_level = new LogIconButton(this);
    pa = DApplicationHelper::instance()->palette(m_level);
    pa.setBrush(DPalette::ButtonText, pa.color(DPalette::TextTips));
    //    m_level->setPalette(pa);
    DApplicationHelper::instance()->setPalette(m_level, pa);

    m_userLabel = new DLabel(DApplication::translate("Label", "User:"), this);
    m_pidLabel = new DLabel(DApplication::translate("Label", "PID:"), this);
    m_statusLabel = new DLabel(DApplication::translate("Label", "Status:"), this);
    m_actionLabel = new DLabel(DApplication::translate("Label", "Action:"), this);

    m_hline = new DHorizontalLine;

    m_textBrowser = new DTextBrowser(this);
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
    QHBoxLayout *h22 = new QHBoxLayout;
    h22->addWidget(m_pidLabel);
    h22->addWidget(m_pid, 1);
    QHBoxLayout *h23 = new QHBoxLayout;
    h23->addWidget(m_statusLabel);
    h23->addWidget(m_status, 1);
    QHBoxLayout *h24 = new QHBoxLayout;
    h24->addWidget(m_actionLabel);
    h24->addWidget(m_action, 1);

    h2->addLayout(h21, 1);
    h2->addLayout(h22, 1);
    h2->addLayout(h23, 1);
    h2->addLayout(h24, 1);
    h2->addStretch(1);
    h2->addWidget(m_level);

    v->addLayout(h1);
    v->addLayout(h2);
    v->addWidget(m_hline);
    v->addWidget(m_textBrowser, 3);

    v->setContentsMargins(20, 10, 20, 0);
    v->setSpacing(4);

    this->setLayout(v);
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

void logDetailInfoWidget::fillDetailInfo(QString deamonName, QString usrName, QString pid,
                                         QString dateTime, QModelIndex level, QString msg,
                                         QString status, QString action)
{
    m_dateTime->show();
    m_daemonName->show();
    m_level->show();
    m_userName->show();
    m_userLabel->show();
    m_pid->show();
    m_pidLabel->show();
    m_status->show();
    m_statusLabel->show();
    m_action->show();
    m_actionLabel->show();

    deamonName.isEmpty() ? m_daemonName->hide() : m_daemonName->setText(deamonName);

    if (!level.isValid()) {
        m_level->hide();
    } else {
        QIcon icon = m_pModel->item(level.row())->icon();
        m_level->setIcon(icon);
        m_level->setText(m_pModel->item(level.row())->data(Qt::UserRole + 6).toString());
    }

    if (dateTime.isEmpty()) {
        m_dateTime->hide();
    } else {
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
        m_userName->setText(usrName);
    }

    if (pid.isEmpty()) {
        m_pid->hide();
        m_pidLabel->hide();
    } else {
        m_pid->setText(pid);
    }

    if (status.isEmpty()) {
        m_status->hide();
        m_statusLabel->hide();
    } else {
        m_status->setText(status);
    }

    if (action.isEmpty()) {
        m_action->hide();
        m_actionLabel->hide();
    } else {
        m_action->setText(action);
    }

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

    if (dataStr.contains(DPKG_TABLE_DATA)) {
        fillDetailInfo("Dpkg", hostname, "", m_pModel->item(index.row(), 0)->text(), QModelIndex(),
                       m_pModel->item(index.row(), 1)->text(), "",
                       m_pModel->item(index.row(), 2)->text());
    } else if (dataStr.contains(XORG_TABLE_DATA)) {
        fillDetailInfo("Xorg", hostname, "", "", QModelIndex(),
                       m_pModel->item(index.row(), 0)->text());
    } else if (dataStr.contains(BOOT_TABLE_DATA)) {
        fillDetailInfo("Boot", hostname, "", "", QModelIndex(),
                       m_pModel->item(index.row(), 1)->text(),
                       m_pModel->item(index.row(), 0)->text());
    } else if (dataStr.contains(KERN_TABLE_DATA)) {
        fillDetailInfo(m_pModel->item(index.row(), 2)->text(),
                       /*m_pModel->item(index.row(), 1)->text()*/ hostname, "",
                       m_pModel->item(index.row(), 0)->text(), QModelIndex(),
                       m_pModel->item(index.row(), 3)->text());
    } else if (dataStr.contains(JOUR_TABLE_DATA)) {
        fillDetailInfo(m_pModel->item(index.row(), 1)->text(),
                       /*m_pModel->item(index.row(), 4)->text()*/ hostname,
                       m_pModel->item(index.row(), 5)->text(),
                       m_pModel->item(index.row(), 2)->text(), index,
                       m_pModel->item(index.row(), 3)->text());
    } else if (dataStr.contains(APP_TABLE_DATA)) {
        fillDetailInfo(name, hostname, "", m_pModel->item(index.row(), 1)->text(), index,
                       m_pModel->item(index.row(), 3)->text());
    }
}
