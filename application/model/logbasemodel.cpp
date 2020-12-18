#include "logbasemodel.h"
#include "structdef.h"
#include <DApplication>
#include <QTextDecoder>
#include <QDateTime>
#include <QDebug>
#include <QtMath>
#include <QPair>
DWIDGET_USE_NAMESPACE

LogBaseModel::LogBaseModel(LOG_DATA_BASE_INFO *info, QObject *parent)
    : QAbstractTableModel(parent)
    , m_curentDataLineRage(0, 0)
{
    setBaseInfo(info);
}

LogBaseModel::~LogBaseModel()
{
//    if (m_baseInfo->mLineCnt > 0) {
//        qDeleteAll(m_baseInfo->mEntersFilter);
//        m_baseInfo->mEntersFilter.clear();
//    }
    if (m_baseInfo->mFile) {
        qDebug() << "fileClose";
        // m_baseInfo->mEnters.clear();
        m_baseInfo-> mFile->unmap((uchar *)m_baseInfo->mMem);
        m_baseInfo->mFile->close();
        delete m_baseInfo->mFile;
        m_baseInfo->mFile = nullptr;
    }
}

int LogBaseModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
//    if (m_currentPage >= m_allPage - 1) {
//        return m_lastPageCount;
//    }
//    return m_pageSize * 3;
    return  m_currentData.size();
}

int LogBaseModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 4;
}

QVariant LogBaseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

//    if (index.row() < 0 || index.row() >= m_baseInfo->mLineCnt)
//        return {};
//    int actualRow = 0;
//    if (m_currentPage < 1) {
//        // actualRow = (m_currentPage - 1) * m_pageSize + index.row();
//        actualRow = index.row();
////   } else if () {

//    } else if (m_currentPage >= m_allPage) {
//        int p_temp = m_allPage - 1 > 0 ? m_allPage - 1 : 0;
//        actualRow = p_temp * m_pageSize + index.row();
//    } else {
//        actualRow = (m_currentPage - 1) * m_pageSize + index.row();
//    }

//    // actualRow = (m_currentPage - 1) * m_pageSize + index.row();
//    qDebug() << m_baseInfo->mLineCnt << m_currentPage << m_allPage << "actualRow" << actualRow;
//    QString str =   getLine(m_baseInfo->mLineCnt - actualRow, m_baseInfo->mLineCnt - actualRow);
//    str.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
//    str.replace('\u0000', "");
//    return filterData(str, index, role);
    ;
    LOG_MSG_KERN itemData = m_currentData[index.row()];
    if (static_cast<Qt::ItemDataRole>(role) == Qt::DisplayRole) {
        switch (index.column()) {

        case KERN_SPACE::kernDateTimeColumn: {
            return itemData.dateTime;
        }
        case KERN_SPACE::kernHostNameColumn: {
            return itemData.hostName;
        }
        case KERN_SPACE::kernDaemonNameColumn:
            return  itemData.daemonName;
        case KERN_SPACE::kernMsgColumn:
            //  qDebug() << "msgInfo" << msgInfo;
            return itemData.msg;

        default:
            break;

        }
    } else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::UserRole + 1) {
        return KERN_TABLE_DATA;
    }
    return {};
}

QVariant LogBaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (section) {
        case KERN_SPACE::kernDateTimeColumn: {
            return DApplication::translate("Table", "Date and Time");
        }
        case KERN_SPACE::kernHostNameColumn: {
            return DApplication::translate("Table", "User");
        }
        case KERN_SPACE::kernDaemonNameColumn:
            return  DApplication::translate("Table", "Process");
        case KERN_SPACE::kernMsgColumn:
            return DApplication::translate("Table", "Info");

        default:
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::InitialSortOrderRole) {
        return QVariant::fromValue(Qt::DescendingOrder);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags LogBaseModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant LogBaseModel::filterData(QString &iLiineStr, const QModelIndex &index, int role) const
{
    QStringList list = iLiineStr.split(" ", QString::SkipEmptyParts);
    if (list.size() < 5)
        return {};
    QStringList timeList;
    timeList.append(list[0]);
    timeList.append(list[1]);
    timeList.append(list[2]);
    //qint64 iTime = formatDateTime(list[0], list[1], list[2]);

    QStringList tmpList = list[4].split("[");
    QString daemonName = "";
    QString daemonId = "";
    if (tmpList.size() != 2) {
        daemonName = list[4].split(":")[0];
    } else {
        daemonName = list[4].split("[")[0];
        QString id = list[4].split("[")[1];
        id.chop(2);
        daemonId = id;
    }
    QString msgInfo;
    for (auto i = 5; i < list.size(); i++) {
        msgInfo.append(list[i] + " ");
    }
    LogBaseModel *th = const_cast<LogBaseModel *>(this);
    th-> m_skipLine.insert(1, 2);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {

        case KERN_SPACE::kernDateTimeColumn: {
            return timeList.join(" ");;
        }
        case KERN_SPACE::kernHostNameColumn: {
            return list[3];
        }
        case KERN_SPACE::kernDaemonNameColumn:
            return  daemonName;
        case KERN_SPACE::kernMsgColumn:
            //  qDebug() << "msgInfo" << msgInfo;
            return msgInfo;

        default:
            break;

        }
    } else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::UserRole + 1) {
        return KERN_TABLE_DATA;
    }
    return {};
}



void LogBaseModel::setBaseInfo(LOG_DATA_BASE_INFO *iInfo)
{
    if (!iInfo) {
        return;
    }
    m_baseInfo = iInfo;
    double tempPage = m_baseInfo->mLineCnt / m_pageSize;
    m_allPage = qCeil(tempPage);
    int lastPageCount = m_baseInfo->mLineCnt % m_pageSize;

    if (m_baseInfo->mLineCnt <= 0) {
        m_lastPageCount = 0;
    } else {
        if (m_allPage == 1) {
            m_lastPageCount = lastPageCount ;
        } else if (m_allPage == 2) {
            //m_lastPageCount = lastPageCount > 0 ? lastPageCount : m_pageSize;
            m_lastPageCount = lastPageCount + m_pageSize;
        } else if (m_allPage >= 3) {
            m_lastPageCount = lastPageCount + m_pageSize * 2;
        }

    }
    refreshPage(true, true);
}

void LogBaseModel::setKernFilter(KERN_FILTERS1 iFilter)
{
    m_kernFilter = iFilter;
    //  refreshPage(true, true);
}

void LogBaseModel::setCurrentPage(int iPage)
{

    beginResetModel();
    if (iPage < 0) {
        m_currentPage = 0;
    } else if (iPage >= m_allPage - 1) {
        m_currentPage = m_allPage - 1;
    } else {
        m_currentPage = iPage;
    }


    endResetModel();
}

int LogBaseModel::getCurrentPage()
{
    return  m_currentPage;
}

void LogBaseModel::addPage()
{
    // qDebug() << __FUNCTION__;
    //  setCurrentPage(m_currentPage + 1);
    refreshPage(true);
    //  emit updateView(true, m_currentPage, m_allPage);
}

void LogBaseModel::reducePage()
{
    // qDebug() << __FUNCTION__;
    // setCurrentPage(m_currentPage - 1);
    refreshPage(false);
    //  emit updateView(false, m_currentPage, m_allPage);
}

void LogBaseModel::refreshPage(bool isAdd, bool iReset)
{
    if (!m_baseInfo->mMem || m_baseInfo->mLineCnt <= 0) {
        return;
    }
    //qDebug() << "m_baseInfo->mEnters.size()" << m_baseInfo->mEnters.size();
    beginResetModel();

    QString str ;
    LOG_MSG_KERN itemData;
    bool isContainsStr = false;
    if (m_kernFilter.searchstr.isEmpty()) {
        isContainsStr = true;
    }

    int count = 0;
    if (iReset) {
        m_curentDataLineRage.first = 0;
        m_curentDataLineRage.second = 0;
    }
    if (isAdd) {
        if (m_curentDataLineRage.second >= (m_baseInfo->mEntersFilterStart.size() - 1)) {
            endResetModel();
            return;
        }
    } else {
        if (m_curentDataLineRage.first <= 0) {
            endResetModel();
            return;
        }
    }
    if (isAdd) {
        for (int i = m_curentDataLineRage.second ; i < m_baseInfo->mEntersFilterStart.size(); ++i) {
            //  for (int i = m_curentDataLineRage.second ; i < m_baseInfo->mEnters.size(); ++i) {
            m_curentDataLineRage.second = i;
            if (!getKernDataFromlineCnt(i, itemData)) {
                continue;
            }
            m_currentData .push_back(itemData);
            if (count < m_pageSize) {
                count++;
            } else {

                break;
            }
        }
    } else {
        for (int i = m_curentDataLineRage.first ; i >= 0; --i) {
            m_curentDataLineRage.first = i;
            if (!getKernDataFromlineCnt(i, itemData)) {
                continue;
            }
            m_currentData .push_front(itemData);
            if (count < m_pageSize) {
                count++;
            } else {
                break;
            }
        }
    }
    if (count > 0 && m_currentData.size() >= m_pageSize * 2) {
        QVector<LOG_MSG_KERN>::iterator iterEnd ;
        QVector<LOG_MSG_KERN>::iterator iterBegin;
        if (isAdd) {
            iterBegin = m_currentData.begin();
            iterEnd = m_currentData.begin();
            iterEnd += m_pageSize;
            m_curentDataLineRage.first += m_pageSize;
        } else {
            iterBegin = m_currentData.end();
            iterEnd = m_currentData.end();
            iterBegin -= m_pageSize;
            m_curentDataLineRage.second -= m_pageSize;
        }
        m_currentData.erase(iterBegin, iterEnd);
    }
    endResetModel();
}

bool LogBaseModel::getKernDataFromlineCnt(int iCount, LOG_MSG_KERN &oItemData)
{
    bool isContainsStr = false;
    if (m_kernFilter.searchstr.isEmpty()) {
        isContainsStr = true;
    } else {
        isContainsStr = false;
    }
    // QString  str = getLine(iCount, iCount);
    QString  str = getLineFilter(iCount);
    //  qDebug() << "str" << str;
    str.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
    str.replace('\u0000', "");
    QStringList list = str.split(" ", QString::SkipEmptyParts);
    if (list.size() < 5) {
        return  false;
    }
    QStringList timeList;
    timeList.append(list[0]);
    timeList.append(list[1]);
    timeList.append(list[2]);
    QStringList tmpList = list[4].split("[");


    if (tmpList.size() != 2) {
        oItemData.daemonName = list[4].split(":")[0];
    } else {
        oItemData.daemonName = list[4].split("[")[0];
        QString id = list[4].split("[")[1];
        id.chop(2);
        oItemData.daemonId = id;
    }
    QString msgInfo;
    for (auto i = 5; i < list.size(); i++) {
        msgInfo.append(list[i] + " ");
    }
//    qint64 iTime = formatDateTime(list[0], list[1], list[2]);
//    //对时间筛选
//    if (m_kernFilter.timeFilterBegin > 0 && m_kernFilter.timeFilterEnd > 0) {
//        if (iTime < m_kernFilter.timeFilterBegin || iTime > m_kernFilter.timeFilterEnd)
//            return  false;
//    }
    oItemData.dateTime = timeList.join(" ");
    oItemData.hostName = list[3];
    oItemData.msg = msgInfo;
//    if (!isContainsStr) {
//        if (oItemData.msg.contains(m_kernFilter.searchstr) || oItemData.dateTime.contains(m_kernFilter.searchstr) || oItemData.hostName.contains(m_kernFilter.searchstr) || oItemData.daemonName.contains(m_kernFilter.searchstr) || oItemData.daemonId.contains(m_kernFilter.searchstr)) {
//            isContainsStr = true;
//        }
//    }
    return true;
}




QString LogBaseModel::getLine(int from, int to) const
{
    if (from <= 0 || from > m_baseInfo->mLineCnt) {
        return "";
    }

    if (to > m_baseInfo->mLineCnt) {
        to = m_baseInfo->mLineCnt;
    }

    auto start = getLineStart(from);
    auto ba = QByteArray::fromRawData(m_baseInfo->mMem + start, (int)(m_baseInfo->mEnters[to] - start));

    return m_baseInfo->mCodec->toUnicode(ba);
}

QString LogBaseModel::getLineFilter(int iLine) const
{
    if (m_baseInfo->mEntersFilterStart.count() < iLine || iLine < 0) {
        return "";
    }
    qint64 start = m_baseInfo->mEntersFilterStart[iLine];
    qint64 end = m_baseInfo->mEntersFilterEnd[iLine];
    return m_baseInfo->mCodec->toUnicode(QByteArray::fromRawData(m_baseInfo->mMem + start, (int)(end - start)));
}



qint64 LogBaseModel::getLineStart(int num) const
{
    if (num == 1) {
        return 0;
    }

    return m_baseInfo->mEnters[num - 1] + 1 + m_baseInfo->mEnterCharOffset;
}

qint64 LogBaseModel::formatDateTime(QString m, QString d, QString t) const
{
    QLocale local(QLocale::English, QLocale::UnitedStates);

    QDate curdt = QDate::currentDate();

    QString tStr = QString("%1 %2 %3 %4").arg(m).arg(d).arg(curdt.year()).arg(t);
    QDateTime dt = local.toDateTime(tStr, "MMM d yyyy hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}
