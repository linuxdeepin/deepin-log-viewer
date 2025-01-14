// SPDX-FileCopyrightText: 2024 - 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ORG_DEEPIN_LOG_VIEWER_H
#define ORG_DEEPIN_LOG_VIEWER_H

#include <QThread>
#include <QVariant>
#include <QDebug>
#include <QAtomicPointer>
#include <QAtomicInteger>
#include <DConfig>

class org_deepin_log_viewer : public QObject {
    Q_OBJECT

    Q_PROPERTY(double coredumpReportMax READ coredumpReportMax WRITE setCoredumpReportMax NOTIFY coredumpReportMaxChanged)
    Q_PROPERTY(QString coredumpReportTime READ coredumpReportTime WRITE setCoredumpReportTime NOTIFY coredumpReportTimeChanged)
    Q_PROPERTY(QList<QVariant> customLogFiles READ customLogFiles WRITE setCustomLogFiles NOTIFY customLogFilesChanged)
    Q_PROPERTY(QString log_rules READ log_rules WRITE setLog_rules NOTIFY log_rulesChanged)
    Q_PROPERTY(double specialComType READ specialComType WRITE setSpecialComType NOTIFY specialComTypeChanged)
public:
    explicit org_deepin_log_viewer(QThread *thread, const QString &appId, const QString &name, const QString &subpath, QObject *parent = nullptr)
        : QObject(parent) {

        if (!thread->isRunning()) {
            qWarning() << QStringLiteral("Warning: The provided thread is not running.");
        }
        Q_ASSERT(QThread::currentThread() != thread);
        auto worker = new QObject();
        worker->moveToThread(thread);
        QMetaObject::invokeMethod(worker, [=]() {
            auto config = DTK_CORE_NAMESPACE::DConfig::create(appId, name, subpath, nullptr);
            if (!config) {
                qWarning() << QStringLiteral("Failed to create DConfig instance.");
                worker->deleteLater();
                return;
            }
            config->moveToThread(QThread::currentThread());
            initialize(config);
            worker->deleteLater();
        });
    }
    explicit org_deepin_log_viewer(QThread *thread, DTK_CORE_NAMESPACE::DConfigBackend *backend, const QString &appId, const QString &name, const QString &subpath, QObject *parent = nullptr)
        : QObject(parent) {

        if (!thread->isRunning()) {
            qWarning() << QStringLiteral("Warning: The provided thread is not running.");
        }
        Q_ASSERT(QThread::currentThread() != thread);
        auto worker = new QObject();
        worker->moveToThread(thread);
        QMetaObject::invokeMethod(worker, [=]() {
            auto config = DTK_CORE_NAMESPACE::DConfig::create(backend, appId, name, subpath, nullptr);
            if (!config) {
                qWarning() << QStringLiteral("Failed to create DConfig instance.");
                worker->deleteLater();
                return;
            }
            config->moveToThread(QThread::currentThread());
            initialize(config);
            worker->deleteLater();
        });
    }
    explicit org_deepin_log_viewer(QThread *thread, const QString &name, const QString &subpath, QObject *parent = nullptr)
        : QObject(parent) {

        if (!thread->isRunning()) {
            qWarning() << QStringLiteral("Warning: The provided thread is not running.");
        }
        Q_ASSERT(QThread::currentThread() != thread);
        auto worker = new QObject();
        worker->moveToThread(thread);
        QMetaObject::invokeMethod(worker, [=]() {
            auto config = DTK_CORE_NAMESPACE::DConfig::create(name, subpath, nullptr);
            if (!config) {
                qWarning() << QStringLiteral("Failed to create DConfig instance.");
                worker->deleteLater();
                return;
            }
            config->moveToThread(QThread::currentThread());
            initialize(config);
            worker->deleteLater();
        });
    }
    explicit org_deepin_log_viewer(QThread *thread, DTK_CORE_NAMESPACE::DConfigBackend *backend, const QString &name, const QString &subpath, QObject *parent = nullptr)
        : QObject(parent) {

        if (!thread->isRunning()) {
            qWarning() << QStringLiteral("Warning: The provided thread is not running.");
        }
        Q_ASSERT(QThread::currentThread() != thread);
        auto worker = new QObject();
        worker->moveToThread(thread);
        QMetaObject::invokeMethod(worker, [=]() {
            auto config = DTK_CORE_NAMESPACE::DConfig::create(backend, name, subpath, nullptr);
            if (!config) {
                qWarning() << QStringLiteral("Failed to create DConfig instance.");
                worker->deleteLater();
                return;
            }
            config->moveToThread(QThread::currentThread());
            initialize(config);
            worker->deleteLater();
        });
    }
    ~org_deepin_log_viewer() {
        if (m_config.loadRelaxed()) {
            m_config.loadRelaxed()->deleteLater();
        }
    }

    double coredumpReportMax() const {
        return p_coredumpReportMax;
    }
    void setCoredumpReportMax(const double &value) {
        auto oldValue = p_coredumpReportMax;
        p_coredumpReportMax = value;
        markPropertySet(0);
        if (auto config = m_config.loadRelaxed()) {
            QMetaObject::invokeMethod(config, [this, value]() {
                m_config.loadRelaxed()->setValue(QStringLiteral("coredumpReportMax"), value);
            });
        }
        if (p_coredumpReportMax != oldValue) {
            Q_EMIT coredumpReportMaxChanged();
        }
    }
    QString coredumpReportTime() const {
        return p_coredumpReportTime;
    }
    void setCoredumpReportTime(const QString &value) {
        auto oldValue = p_coredumpReportTime;
        p_coredumpReportTime = value;
        markPropertySet(1);
        if (auto config = m_config.loadRelaxed()) {
            QMetaObject::invokeMethod(config, [this, value]() {
                m_config.loadRelaxed()->setValue(QStringLiteral("coredumpReportTime"), value);
            });
        }
        if (p_coredumpReportTime != oldValue) {
            Q_EMIT coredumpReportTimeChanged();
        }
    }
    QList<QVariant> customLogFiles() const {
        return p_customLogFiles;
    }
    void setCustomLogFiles(const QList<QVariant> &value) {
        auto oldValue = p_customLogFiles;
        p_customLogFiles = value;
        markPropertySet(2);
        if (auto config = m_config.loadRelaxed()) {
            QMetaObject::invokeMethod(config, [this, value]() {
                m_config.loadRelaxed()->setValue(QStringLiteral("customLogFiles"), value);
            });
        }
        if (p_customLogFiles != oldValue) {
            Q_EMIT customLogFilesChanged();
        }
    }
    QString log_rules() const {
        return p_log_rules;
    }
    void setLog_rules(const QString &value) {
        auto oldValue = p_log_rules;
        p_log_rules = value;
        markPropertySet(3);
        if (auto config = m_config.loadRelaxed()) {
            QMetaObject::invokeMethod(config, [this, value]() {
                m_config.loadRelaxed()->setValue(QStringLiteral("log_rules"), value);
            });
        }
        if (p_log_rules != oldValue) {
            Q_EMIT log_rulesChanged();
        }
    }
    double specialComType() const {
        return p_specialComType;
    }
    void setSpecialComType(const double &value) {
        auto oldValue = p_specialComType;
        p_specialComType = value;
        markPropertySet(4);
        if (auto config = m_config.loadRelaxed()) {
            QMetaObject::invokeMethod(config, [this, value]() {
                m_config.loadRelaxed()->setValue(QStringLiteral("specialComType"), value);
            });
        }
        if (p_specialComType != oldValue) {
            Q_EMIT specialComTypeChanged();
        }
    }
Q_SIGNALS:
    void coredumpReportMaxChanged();
    void coredumpReportTimeChanged();
    void customLogFilesChanged();
    void log_rulesChanged();
    void specialComTypeChanged();
private:
    void initialize(DTK_CORE_NAMESPACE::DConfig *config) {
        Q_ASSERT(!m_config.loadRelaxed());
        m_config.storeRelaxed(config);
        if (testPropertySet(0)) {
            config->setValue(QStringLiteral("coredumpReportMax"), QVariant::fromValue(p_coredumpReportMax));
        } else {
            updateValue(QStringLiteral("coredumpReportMax"), QVariant::fromValue(p_coredumpReportMax));
        }
        if (testPropertySet(1)) {
            config->setValue(QStringLiteral("coredumpReportTime"), QVariant::fromValue(p_coredumpReportTime));
        } else {
            updateValue(QStringLiteral("coredumpReportTime"), QVariant::fromValue(p_coredumpReportTime));
        }
        if (testPropertySet(2)) {
            config->setValue(QStringLiteral("customLogFiles"), QVariant::fromValue(p_customLogFiles));
        } else {
            updateValue(QStringLiteral("customLogFiles"), QVariant::fromValue(p_customLogFiles));
        }
        if (testPropertySet(3)) {
            config->setValue(QStringLiteral("log_rules"), QVariant::fromValue(p_log_rules));
        } else {
            updateValue(QStringLiteral("log_rules"), QVariant::fromValue(p_log_rules));
        }
        if (testPropertySet(4)) {
            config->setValue(QStringLiteral("specialComType"), QVariant::fromValue(p_specialComType));
        } else {
            updateValue(QStringLiteral("specialComType"), QVariant::fromValue(p_specialComType));
        }

        connect(config, &DTK_CORE_NAMESPACE::DConfig::valueChanged, this, [this](const QString &key) {
            updateValue(key);
        }, Qt::DirectConnection);
    }
    void updateValue(const QString &key, const QVariant &fallback = QVariant()) {
        Q_ASSERT(QThread::currentThread() == m_config.loadRelaxed()->thread());
        const QVariant &value = m_config.loadRelaxed()->value(key, fallback);
        if (key == QStringLiteral("coredumpReportMax")) {
            auto newValue = qvariant_cast<double>(value);
            QMetaObject::invokeMethod(this, [this, newValue]() {
                if (p_coredumpReportMax != newValue) {
                    p_coredumpReportMax = newValue;
                    Q_EMIT coredumpReportMaxChanged();
                }
            });
            return;
        }
        if (key == QStringLiteral("coredumpReportTime")) {
            auto newValue = qvariant_cast<QString>(value);
            QMetaObject::invokeMethod(this, [this, newValue]() {
                if (p_coredumpReportTime != newValue) {
                    p_coredumpReportTime = newValue;
                    Q_EMIT coredumpReportTimeChanged();
                }
            });
            return;
        }
        if (key == QStringLiteral("customLogFiles")) {
            auto newValue = qvariant_cast<QList<QVariant>>(value);
            QMetaObject::invokeMethod(this, [this, newValue]() {
                if (p_customLogFiles != newValue) {
                    p_customLogFiles = newValue;
                    Q_EMIT customLogFilesChanged();
                }
            });
            return;
        }
        if (key == QStringLiteral("log_rules")) {
            auto newValue = qvariant_cast<QString>(value);
            QMetaObject::invokeMethod(this, [this, newValue]() {
                if (p_log_rules != newValue) {
                    p_log_rules = newValue;
                    Q_EMIT log_rulesChanged();
                }
            });
            return;
        }
        if (key == QStringLiteral("specialComType")) {
            auto newValue = qvariant_cast<double>(value);
            QMetaObject::invokeMethod(this, [this, newValue]() {
                if (p_specialComType != newValue) {
                    p_specialComType = newValue;
                    Q_EMIT specialComTypeChanged();
                }
            });
            return;
        }
    }
    inline void markPropertySet(const int index) {
        if (index < 32) {
            m_propertySetStatus0.fetchAndOrOrdered(1 << (index - 0));
            return;
        }
        Q_UNREACHABLE();
    }
    inline bool testPropertySet(const int index) const {
        if (index < 32) {
            return (m_propertySetStatus0.loadRelaxed() & (1 << (index - 0)));
        }
        Q_UNREACHABLE();
    }
    QAtomicPointer<DTK_CORE_NAMESPACE::DConfig> m_config = nullptr;
    double p_coredumpReportMax { 50 };
    QString p_coredumpReportTime { QStringLiteral("") };
    QList<QVariant> p_customLogFiles { QList<QVariant>{} };
    QString p_log_rules { QStringLiteral("*.debug=false;*.info=false;*log.viewer*.warning=true") };
    double p_specialComType { -1 };
    QAtomicInteger<quint32> m_propertySetStatus0 = 0;
};

#endif // ORG_DEEPIN_LOG_VIEWER_H
