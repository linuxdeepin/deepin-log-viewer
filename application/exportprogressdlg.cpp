#include "exportprogressdlg.h"
#include "structdef.h"

#include <DLabel>
#include <DApplication>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
ExportProgressDlg::ExportProgressDlg(DWidget *parent)
    : DDialog(parent)
{
    QString titleIcon = ICONPREFIX ;
    setIcon(QIcon(titleIcon + "uospm_warning_32px.svg"));
    //setWindowFlags(windowFlags());

    DWidget *pWidget = new DWidget(this);
    QVBoxLayout *pVLayout = new QVBoxLayout();

    DLabel *txtLabel = new DLabel(DApplication::translate("ExportDlg", "Exporting")); //提示信息
    txtLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *pVLayouttxt = new QVBoxLayout();
    pVLayouttxt->addSpacing(10);
    pVLayouttxt->addWidget(txtLabel, Qt::AlignHCenter);
    pVLayout->addLayout(pVLayouttxt);

    m_pExportProgressBar = new DProgressBar(this);
    m_pExportProgressBar->setTextVisible(false);
    m_pExportProgressBar->setMaximumHeight(8);
    m_pExportProgressBar->setRange(0, 100);
    pVLayout->addWidget(m_pExportProgressBar);

    pWidget->setLayout(pVLayout);
    addContent(pWidget);

    addButton(DApplication::translate("ExportDlg", "Cancel"), false, DDialog::ButtonNormal);
    // setOnButtonClickedClose(true);
    setModal(true);
}

void ExportProgressDlg::setProgressBarRange(int minValue, int maxValue)
{
    if (m_pExportProgressBar != nullptr) {
        if (maxValue > minValue)
            m_pExportProgressBar->setRange(minValue, maxValue);
    }
}

void ExportProgressDlg::updateProgressBarValue(int curValue)
{
    if (m_pExportProgressBar != nullptr) {
        m_pExportProgressBar->setValue(curValue);
        update();
    }
}

void ExportProgressDlg::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    m_pExportProgressBar->setValue(m_pExportProgressBar->minimum());
    emit sigCloseBtnClicked();
}
