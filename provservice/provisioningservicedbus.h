/*
 * Copyright (C) 2013 Jolla Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef PROVISIONINGSERVICEDBUS_H
#define PROVISIONINGSERVICEDBUS_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDebug>

#include "provisioningservice.h"

class ProvisioningServiceDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ofono.mms.PushConsumer")

public:
    ProvisioningServiceDBusAdaptor(ProvisioningService *aService)
        : QDBusAbstractAdaptor(aService), service(aService) { }

public slots:
    /*
     * Implements the mmsd PushConsumer interface
     */
    Q_NOREPLY void Notify(const QByteArray &header, const QByteArray &body) {
        service->Notify(header, body);
	}

    /*
     * Shut service down with
     * dbus-send --print-reply --session --dest="org.nemomobile.provision" /org/nemomobile/provision org.ofono.mms.PushConsumer.Release
     */
    Q_NOREPLY void Release() {
        qDebug() << "Release, exiting";
        exit(0);
    }
private:
    ProvisioningService *service;
};

#endif // PROVISIONINGSERVICEDBUS_H
