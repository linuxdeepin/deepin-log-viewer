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
#ifndef EXPORTPROGRESSDLG_H
#define EXPORTPROGRESSDLG_H
#include <DDialog>
#include <DProgressBar>
#include <DWidget>
DWIDGET_USE_NAMESPACE
/**
 * @brief The ExportProgressDlg class 导出进度框
 */
class ExportProgressDlg : public DDialog
{
    Q_OBJECT
public:
    explicit ExportProgressDlg(DWidget *parent = nullptr);

    void setProgressBarRange(int minValue, int maxValue);
    void updateProgressBarValue(int curValue);
signals:
    /**
     * @brief sigCloseBtnClicked 关闭或取消按钮触发时发出,用来取消与此相关的导出出逻辑
     */
    void sigCloseBtnClicked();
protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private:
    /**
     * @brief m_pExportProgressBar 进度条
     */
    DProgressBar *m_pExportProgressBar;
};

#endif // EXPORTPROGRESSDLG_H
