/*
 *  Copyright (c) 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_delayed_save_dialog.h"
#include "ui_kis_delayed_save_dialog.h"

#include <QTimer>
#include <QElapsedTimer>
#include <QThread>

#include "kis_debug.h"
#include "kis_image.h"
#include "kis_composite_progress_proxy.h"


struct KisDelayedSaveDialog::Private
{
    Private(KisImageSP _image, int _busyWait, Type _type) : image(_image), busyWait(_busyWait), type(_type) {}

    KisImageSP image;
    QTimer updateTimer;
    int busyWait;


    bool checkImageIdle() {
        const bool allowLocked = type != SaveDialog;
        return image->isIdle(allowLocked);
    }

private:
    Type type;
};

KisDelayedSaveDialog::KisDelayedSaveDialog(KisImageSP image, Type type, int busyWait, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::KisDelayedSaveDialog),
      m_d(new Private(image, busyWait, type))
{
    KIS_ASSERT_RECOVER_NOOP(image);

    ui->setupUi(this);

    if (type == SaveDialog) {
        connect(ui->bnDontWait, SIGNAL(clicked()), SLOT(slotIgnoreRequested()));
        connect(ui->bnCancel, SIGNAL(clicked()), SLOT(slotCancelRequested()));
    } else {
        ui->bnDontSave->setText(i18n("Cancel"));
        ui->bnDontWait->setVisible(false);
        ui->bnCancel->setVisible(false);

        if (type == ForcedDialog) {
            ui->bnDontSave->setVisible(false);
        }
    }

    connect(ui->bnDontSave, SIGNAL(clicked()), SLOT(reject()));

    connect(&m_d->updateTimer, SIGNAL(timeout()), SLOT(slotTimerTimeout()));

    m_d->image->compositeProgressProxy()->addProxy(ui->progressBar);
}

KisDelayedSaveDialog::~KisDelayedSaveDialog()
{
    m_d->image->compositeProgressProxy()->removeProxy(ui->progressBar);
    delete ui;
}

void KisDelayedSaveDialog::blockIfImageIsBusy()
{
    if (m_d->checkImageIdle()) {
        setResult(Accepted);
        return;
    }

    m_d->image->requestStrokeEnd();

    QElapsedTimer t;
    t.start();

    while (t.elapsed() < m_d->busyWait) {
        QApplication::processEvents();

        if (m_d->checkImageIdle()) {
            setResult(Accepted);
            return;
        }

        QThread::yieldCurrentThread();
    }

    m_d->updateTimer.start(200);
    exec();
    m_d->updateTimer.stop();
}

void KisDelayedSaveDialog::slotTimerTimeout()
{
    if (m_d->checkImageIdle()) {
        accept();
    }
}

void KisDelayedSaveDialog::slotCancelRequested()
{
    m_d->image->requestStrokeCancellation();
}

void KisDelayedSaveDialog::slotIgnoreRequested()
{
    done(Ignored);
}
