#include "logsearchnoresultwidget.h"
#include <DApplication>
#include <QFont>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

LogSearchNoResultWidget::LogSearchNoResultWidget(QWidget *parent)
    : DWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    m_text = new DLabel(this);
    layout->addWidget(m_text);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
}

LogSearchNoResultWidget::~LogSearchNoResultWidget()
{
    delete m_text;
    m_text = nullptr;
}

void LogSearchNoResultWidget::setContent(QString word)
{
    QString result;

    QString str1 = DApplication::translate("NoSearch", "Don't find");
    result = str1 + "“" + word + "”";
    QString str2 = DApplication::translate("NoSearch", ", please retry another keyword.");
    result += str2;

    QFont labelFont;
    labelFont.setPixelSize(20);
    m_text->setFont(labelFont);
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setText(result);
}
