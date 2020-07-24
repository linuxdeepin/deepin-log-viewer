#ifndef EXPORTPROGRESSDLG_H
#define EXPORTPROGRESSDLG_H
#include <DDialog>
#include <DProgressBar>
#include <DWidget>
DWIDGET_USE_NAMESPACE

class ExportProgressDlg : public DDialog
{
    Q_OBJECT
public:
    explicit ExportProgressDlg(DWidget *parent = nullptr);

    void setProgressBarRange(int minValue, int maxValue);
    void updateProgressBarValue(int curValue);
signals:
    void sigCloseBtnClicked();
protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private:
    DProgressBar *m_pExportProgressBar;
};

#endif // EXPORTPROGRESSDLG_H
