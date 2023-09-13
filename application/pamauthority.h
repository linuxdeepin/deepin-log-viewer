// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PAMAUTHORITY_H
#define PAMAUTHORITY_H

#include <QObject>

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <security/pam_appl.h>

class PamAuthority : public QObject
{
    Q_OBJECT

public:
    explicit PamAuthority(QObject *parent = nullptr);
    ~PamAuthority();

    void startPam();

    static int pam_tty_conv(int num_msg, const struct pam_message **mess, struct pam_response **resp, void *my_data);
    static char *getinput(int noecho);
    static void free_resp(int num_msg, struct pam_response *pr);
    static void logout() __attribute__((noreturn));
};

#endif // PAMAUTHORITY_H
