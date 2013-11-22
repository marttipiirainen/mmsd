/*
 * Copyright (C) 2013 Jolla Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

/*
 * A simple consumer for OMA Client Provisioning push messages.
 */

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QDBusError>
#include <QDBusConnection>

#include "provisioningservice.h"

#include <wbxml.h>
#include <wbxml_config.h>

ProvisioningService::ProvisioningService(QObject *parent)
    : QObject(parent)
{
    qDebug() << ">>ProvisioningService";

#ifdef WBXML_LANG_PROV10
    qDebug() << "prov10 apparently ok";
#else
    qDebug() << "prov10 apparently not ok (not sure what this means)";
#endif

    if(!QDBusConnection::sessionBus().registerService("org.nemomobile.provision"))
    {

        qDebug() << "Failed to register DBus service: " << QDBusConnection::sessionBus().lastError().message();
        return;
    }

    if(!QDBusConnection::sessionBus().registerObject("/org/nemomobile/provision", this))
    {
    	qDebug() << "Failed to register DBus object: " << QDBusConnection::sessionBus().lastError().message();
        return;
    }


    // TODO implement interface "org.ofono.mms.PushConsumer"
    //
    QFile file("./prov.wbxml");
    if (!file.open(QIODevice::ReadOnly)) {
    	qDebug() << "no file";
    	return;
    }
    QByteArray fileBlob = file.readAll();
    QByteArray dst;
    bool success = this->decodeFromWbXML(fileBlob, dst, true);
    qDebug() << "success: " << success;
    qDebug() << "dst: " << dst;

    qDebug() << "<<ProvisioningService";
}

/*
 * Almost verbatim copy of buteo's method LibWbXML2Encoder::decodeFromWbXML()
 * (from buteo-syncml/src/transport/LibWbXML2Encoder.cpp)
 */
bool ProvisioningService::decodeFromWbXML( const QByteArray& aWbXMLDocument, QByteArray& aXMLDocument,
                                        bool aPrettyPrint ) const
{

	qDebug() << "Converting WbXML to XML";

    WBXMLGenXMLParams params;
    params.lang = WBXML_LANG_UNKNOWN;

    if( aPrettyPrint ) {
        params.gen_type = WBXML_GEN_XML_INDENT;
        params.indent = 3;
    }
    else {
        params.gen_type = WBXML_GEN_XML_COMPACT;
        params.indent = 0;
    }

    params.keep_ignorable_ws = TRUE;

    const WB_UTINY* wbxml = reinterpret_cast<const WB_UTINY*>( aWbXMLDocument.constData() );
    WB_ULONG wbxml_len = aWbXMLDocument.size();

    WB_UTINY* xml;
    WB_ULONG xml_len;

    qDebug() << "WbXML buffer size: " << wbxml_len;

    WBXMLError error = wbxml_conv_wbxml2xml_withlen( const_cast<WB_UTINY*>( wbxml ), wbxml_len,
                                                     &xml, &xml_len,
                                                     &params );

    QByteArray data;

    if( error == WBXML_OK ) {
        qDebug() << "WbXML to XML conversion succeeded";
        qDebug() << "XML buffer size: " << xml_len;
        aXMLDocument.append( reinterpret_cast<char*>( xml ), xml_len );
        wbxml_free( xml );
        return true;
    }
    else {
        qDebug() << "WbXML to XML conversion failed: " << (const char* )wbxml_errors_string( error );
        return false;
    }

}


int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setOrganizationName("nemomobile");
    QCoreApplication::setApplicationName("provisioningservice");

    ProvisioningService service;

    return app.exec();
}
