#ifndef LOGBASEMODEL_H
#define LOGBASEMODEL_H

#include <QAbstractTableModel>
#include "structdef.h"
class LogBaseModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LogBaseModel(LOG_DATA_BASE_INFO *info = nullptr, QObject *parent = nullptr);
    int lineCount()  {return m_baseInfo->mLineCnt;}
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant filterData(QString &iLiineStr, const QModelIndex &index, int role) const ;

    void setBaseInfo(LOG_DATA_BASE_INFO *iInfo);
    void setKernFilter(KERN_FILTERS1 iFilter);
    void setCurrentPage(int iPage);
    int getCurrentPage();
    void addPage();
    void reducePage();
    void refreshPage(bool isAdd, bool iReset = false);
    bool getKernDataFromlineCnt(int iCount, LOG_MSG_KERN &oItemData);
    QString getLine(int from, int to) const;
    qint64 getLineStart(int num) const;
    qint64 formatDateTime(QString m, QString d, QString t) const;

    LOG_DATA_BASE_INFO *m_baseInfo;
    int m_pageSize = 50;
    QHash<int, int> m_pageHash; // key是页码值, value是此页的第一项数据在dataList中的下标值
    int m_currentPage = 1;
    int m_allPage = 0;
    int m_lastPageCount = 0;
    KERN_FILTERS1 m_kernFilter;
    QHash<int, int> m_skipLine;
    QVector<LOG_MSG_KERN> m_currentData;
    QPair<int, int> m_curentDataLineRage;

signals:
    void updateView(bool iDirection, int iCurrentPage, int iTotal);


};

#endif // LOGBASEMODEL_H
