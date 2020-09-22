/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "exportprogressdlg.h"
#include "structdef.h"

#include <DLabel>
#include <DApplication>
#include <DFontSizeManager>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QAbstractButton>
ExportProgressDlg::ExportProgressDlg(DWidget *parent)
    : DDialog(parent)
{
    QString titleIcon = ICONPREFIX ;
    setIcon(QIcon::fromTheme("deepin-log-viewer"));
    //setWindowFlags(windowFlags());

    DWidget *pWidget = new DWidget(this);
    QVBoxLayout *pVLayout = new QVBoxLayout();

    DLabel *txtLabel = new DLabel(DApplication::translate("ExportDlg", "Exporting...")); //提示信息
    txtLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(txtLabel, DFontSizeManager::T6);
    DPalette pa = DApplicationHelper::instance()->palette(txtLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::BrightText));
    DApplicationHelper::instance()->setPalette(txtLabel, pa);
    QVBoxLayout *pVLayouttxt = new QVBoxLayout();
    pVLayouttxt->setContentsMargins(0, 0, 0, 10);
    //pVLayouttxt->addSpacing(10);
    pVLayouttxt->addWidget(txtLabel, Qt::AlignHCenter);
    pVLayout->addLayout(pVLayouttxt);

    m_pExportProgressBar = new DProgressBar(this);
    m_pExportProgressBar->setAccessibleName("export_progressbar");
    m_pExportProgressBar->setTextVisible(false);
    m_pExportProgressBar->setMaximumHeight(8);
    m_pExportProgressBar->setRange(0, 100);
    pVLayout->addWidget(m_pExportProgressBar);
    pVLayout->setContentsMargins(0, 0, 0, 5);
    pWidget->setLayout(pVLayout);

    addContent(pWidget);

    addButton(DApplication::translate("ExportDlg", "Cancel"), false, DDialog::ButtonNormal);
    getButton(0)->setAccessibleName("export_cancel_btn");
    // setOnButtonClickedClose(true);
    setModal(true);
    qDebug() << "this->height()" << this->geometry().height();
    //  setFixedHeight(120);
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
