/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
