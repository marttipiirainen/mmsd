/*
 * Copyright (C) 2013 Jolla Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef PROVISIONINGSERVICE_H
#define PROVISIONINGSERVICE_H

#include <QObject>

class ProvisioningService : public QObject
{
    Q_OBJECT

public:
    explicit ProvisioningService(QObject *parent = 0);
    void Notify(const QByteArray &header, const QByteArray &body);

public slots:


private:
    /*
     * Decode WBXML to Prov 1.0 XML
     */
    bool decodeWBXML(const QByteArray &wbxml, QByteArray &xml);

    /*
     * Parse PROV 1.0 data, return all found Access Point Names
     */
    QStringList Parse(const QByteArray &doc);

};

#endif // PROVISIONINGSERVICE_H
