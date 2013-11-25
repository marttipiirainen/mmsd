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
    /*! \brief Decode a SyncML message from WbXML document
     *
     * @param aWbXMLDocument Message to decode
     * @param aXMLDocument Output XML document
     * @param aPrettyPrint If true prefer human-readable output, otherwise prefer compact size
     * @return True on success, otherwise false
     */
    bool decodeFromWbXML( const QByteArray& aWbXMLDocument, QByteArray& aXMLDocument,
                          bool aPrettyPrint ) const;

};

#endif // PROVISIONINGSERVICE_H
