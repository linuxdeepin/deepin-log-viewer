#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QLocale>

#include <mutex>
class Common: public QObject
{
    Q_OBJECT
public:
    static Common *instance()
    {
        Common *sin = m_instance.load();
        if (!sin) {
            //加锁 线程安全
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new Common();
                m_instance.store(sin);
            }
        }
        return sin;
    }
protected:
    Common(QObject *parent = nullptr)
        : QObject(parent) {}

public:
    static float codecConfidenceForData(const QTextCodec *codec, const QByteArray &data, const QLocale::Country &country);
    QString  trans2uft8(const char *str);
    QByteArray detectEncode(const QByteArray &data, const QString &fileName = QString());
    bool detectEncodeForCodec(const QByteArray &data, QTextCodec *oCodec);
    int ChartDet_DetectingTextCoding(const char *str, QString &encoding, float &confidence);
    QByteArray textCodecDetect(const QByteArray &data, const QString &fileName);

    QByteArray m_codecStr;


    /**
     * @brief m_instance 单例用的本类指针的原子性封装
     */
    static std::atomic<Common *> m_instance;
    /**
     * @brief m_mutex 单例用的锁
     */
    static std::mutex m_mutex;
};
//QString  trans2uft8(const char *str);
//QByteArray detectEncode(const QByteArray &data, const QString &fileName = QString());
//int ChartDet_DetectingTextCoding(const char *str, QString &encoding, float &confidence);
//QByteArray textCodecDetect(const QByteArray &data, const QString &fileName);

#endif
