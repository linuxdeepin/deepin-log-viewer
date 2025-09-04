// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGALLEXPORTTHREAD_H
#define LOGALLEXPORTTHREAD_H

#include "structdef.h"
#include "zip.h"

#include <QObject>
#include <atomic>

class LogAllExportThread : public QObject
{
    Q_OBJECT
public:
    explicit LogAllExportThread(const QStringList &types, const QString &outfile, QObject *parent = nullptr);

public slots:
    void slot_cancelExport() {
        m_cancel.store(true);
    }
    void run();

signals:
    void updatecurrentProcess(int current);
    void updateTolProcess(int tol);
    void exportFinsh(bool success = true);

private:
    bool addFileToZip(const QString &filePath, const QString &zipEntryName);
    bool addProcessOutputToZip(const QString &command, const QStringList &args, const QString &zipEntryName);
    void ensureDirectoriesExist(const QString &filePath);
    
    // Optimized cancel check with reduced atomic load frequency
    inline bool shouldCancel() const {
        // Check every 8th call to reduce atomic operations, but always check if cancelled
        if (++m_cancelCheckCounter % 8 == 0 || m_lastCancelState) {
            m_lastCancelState = m_cancel.load();
            return m_lastCancelState;
        }
        return m_lastCancelState;
    }

    QStringList m_types;
    QString m_outfile {""};
    zipFile m_zipFile {nullptr};

    std::atomic<bool> m_cancel;
    mutable int m_cancelCheckCounter = 0;  // Reduce atomic load frequency
    mutable bool m_lastCancelState = false;  // Cache last cancel state

};

#endif // LOGALLEXPORTTHREAD_H