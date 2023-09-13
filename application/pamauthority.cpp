// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pamauthority.h"

#include <libgen.h>
#include <malloc.h>
#include <termio.h>

#include <DApplication>
DWIDGET_USE_NAMESPACE

#define PAM_SERVICE_NAME "deepin-log-viewer"

PamAuthority::PamAuthority(QObject *parent)
    : QObject(parent)
{
}

PamAuthority::~PamAuthority()
{
}

void PamAuthority::startPam()
{
    struct pam_conv conv = { pam_tty_conv, nullptr };

    pam_handle_t *pamh;

    struct passwd *pw;

    int err;

    if ((pw = getpwuid(getuid())) == nullptr) {

        (void) fprintf(stderr, "deepin-log-viewer: Can't get username: %s\n",

                       strerror(errno));

        exit(1);

    }

    /* Initialize PAM framework */

    err = pam_start(PAM_SERVICE_NAME, pw->pw_name, &conv, &pamh);

    if (err != PAM_SUCCESS) {

        (void) fprintf(stderr, "deepin-log-viewer: pam_start failed: %s\n",

                       pam_strerror(pamh, err));

        exit(1);

    }

    /* Authenticate user in order to unlock screen */

    do {

        QString str = DApplication::translate("Warning", "Please input password for %1. ").arg(pw->pw_name);
        (void) fprintf(stderr, str.toStdString().c_str());

        err = pam_authenticate(pamh, 0);

        if (err == PAM_USER_UNKNOWN)

            logout();

        else if (err != PAM_SUCCESS) {

            (void) fprintf(stderr, "Invalid password.\n");

        }

    } while (err != PAM_SUCCESS);


    /* Make sure account and password are still valid */

    switch (err = pam_acct_mgmt(pamh, 0)) {

    case PAM_USER_UNKNOWN:

    case PAM_ACCT_EXPIRED:

        /* User not allowed in anymore */

        logout();

    case PAM_NEW_AUTHTOK_REQD:

        /* The user's password has expired. Get a new one */

        do {

            err = pam_chauthtok(pamh, 0);

        } while (err == PAM_AUTHTOK_ERR);

        if (err != PAM_SUCCESS)

            logout();

        break;

    }

    if (pam_setcred(pamh, PAM_REFRESH_CRED) != PAM_SUCCESS){

        logout();

    }

    (void) pam_end(pamh, 0);

}

int PamAuthority::pam_tty_conv(int num_msg, const pam_message **mess, pam_response **resp, void *my_data)
{
    Q_UNUSED(my_data);

    const struct pam_message *m = *mess;
    struct pam_response *r;
    int i;

    if (num_msg <= 0 || num_msg >= PAM_MAX_NUM_MSG) {
        (void) fprintf(stderr, "bad number of messages %d "
                               "<= 0 || >= %d\n",
                       num_msg, PAM_MAX_NUM_MSG);
        *resp = nullptr;
        return (PAM_CONV_ERR);
    }
    if ((*resp = r = static_cast<struct pam_response*>(calloc(static_cast<size_t>(num_msg),
                            sizeof (struct pam_response)))) == nullptr)
        return (PAM_BUF_ERR);

    /* Loop through messages */
    for (i = 0; i < num_msg; i++) {
        int echo_off;

        /* bad message from service module */
        if (m->msg == nullptr) {
            (void) fprintf(stderr, "message[%d]: %d/nullptr\n",
                           i, m->msg_style);
            goto err;
        }

        /*
             * fix up final newline:
             * 	removed for prompts
             * 	added back for messages
             */
        if (m->msg[strlen(m->msg)] == '\n') {
            char* str = const_cast<char *>(m->msg);
            str[strlen(m->msg)] = '\0';
        }

        r->resp = nullptr;
        r->resp_retcode = 0;
        echo_off = 0;
        switch (m->msg_style) {

        case PAM_PROMPT_ECHO_OFF:
        case PAM_PROMPT_ECHO_ON:
            echo_off = 1;
            (void) fputs(m->msg, stdout);

            r->resp = getinput(echo_off);
            break;

        case PAM_ERROR_MSG:
            (void) fputs(m->msg, stderr);
            (void) fputc('\n', stderr);
            break;

        case PAM_TEXT_INFO:
            (void) fputs(m->msg, stdout);
            (void) fputc('\n', stdout);
            break;

        default:
            (void) fprintf(stderr, "message[%d]: unknown type "
                                   "%d/val=\"%s\"\n",
                           i, m->msg_style, m->msg);
            /* error, service module won't clean up */
            goto err;
        }
        if (errno == EINTR)
            goto err;

        /* next message/response */
        m++;
        r++;
    }
    return (PAM_SUCCESS);

err:
    free_resp(i, r);
    *resp = nullptr;
    return (PAM_CONV_ERR);
}

char *PamAuthority::getinput(int noecho)
{
    struct termio tty;
        unsigned short tty_flags = 0;
        char input[PAM_MAX_RESP_SIZE];
        int c;
        int i = 0;

        if (noecho) {
            (void) ioctl(fileno(stdin), TCGETA, &tty);
            tty_flags = tty.c_lflag;
            tty.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
            (void) ioctl(fileno(stdin), TCSETAF, &tty);
        }

        /* go to end, but don't overflow PAM_MAX_RESP_SIZE */
        flockfile(stdin);
        while ((c = getchar_unlocked()) != '\n' &&
            c != '\r' &&
            c != EOF) {
            if (i < PAM_MAX_RESP_SIZE) {
                input[i++] = static_cast<char>(c);
            }
        }
        funlockfile(stdin);
        input[i] = '\0';
        if (noecho) {
            tty.c_lflag = tty_flags;
            (void) ioctl(fileno(stdin), TCSETAW, &tty);
            (void) fputc('\n', stdout);
        }

        return (strdup(input));
}

void PamAuthority::free_resp(int num_msg, pam_response *pr)
{
    int i;
        struct pam_response *r = pr;

        if (pr == nullptr)
            return;

        for (i = 0; i < num_msg; i++, r++) {

            if (r->resp) {
                /* clear before freeing -- may be a password */
                bzero(r->resp, strlen(r->resp));
                free(r->resp);
                r->resp = nullptr;
            }
        }
        free(pr);
}

void PamAuthority::logout()
{
    pid_t pgroup = getpgrp();

    (void) signal(SIGTERM, SIG_IGN);

    (void) getchar();

    (void) kill(-pgroup, SIGTERM);

    (void) sleep(2);

    (void) kill(-pgroup, SIGKILL);

    exit(-1);

}
