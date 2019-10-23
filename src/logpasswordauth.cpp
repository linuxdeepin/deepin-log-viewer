#include "logpasswordauth.h"
#include <DApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

LogPasswordAuth::LogPasswordAuth(DWidget *parent)
    : DDialog(parent)
{
    //    this->setFixedSize(350, 120);

    initUI();
    initConnections();
}

void LogPasswordAuth::initUI()
{
    this->setTitle(
        DApplication::translate("password", "please input sudo password to execute operate"));
    m_edt = new DLineEdit(this);
    m_edt->setEchoMode(QLineEdit::Password);
    m_edt->setMaximumHeight(30);
    QList<QWidget *> list;
    DPushButton *hs = new DPushButton("s");
    hs->setCheckable(true);
    list.append(hs);
    m_edt->setRightWidgets(list);
    this->addSpacing(10);
    this->addContent(m_edt);
    this->setIcon(QIcon("://images/logo.svg"), QSize(50, 50));
    this->addButton(DApplication::translate("Button", "Cancel"));
    this->addButton(DApplication::translate("Button", "Comfirm"), true);

    connect(hs, &DPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            m_edt->setEchoMode(QLineEdit::Normal);
            hs->setText("h");
        } else {
            m_edt->setEchoMode(QLineEdit::Password);
            hs->setText("s");
        }
    });
}

void LogPasswordAuth::initConnections()
{
    connect(this, &DDialog::buttonClicked, this, &LogPasswordAuth::slotButtonClicked);
}

QString LogPasswordAuth::getPasswd()
{
    return m_passwd;
}

void LogPasswordAuth::slotButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text)

    switch (index) {
        case 0:
            this->setPasswd("");
            this->setOnButtonClickedClose(true);
            break;
        case 1:
            if (m_edt->text().isEmpty())
                this->setOnButtonClickedClose(false);
            else {
                this->setOnButtonClickedClose(true);
                this->setPasswd(m_edt->text());
            }
            break;
        default:
            break;
    }
    m_edt->clear();
}

void LogPasswordAuth::setPasswd(QString str)
{
    m_passwd = str;
}
