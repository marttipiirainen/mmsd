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
#include <QStringList>
#include <QFile>
#include <QDebug>

#include <QDBusError>
#include <QDBusConnection>

#include <QXmlStreamReader>
#include <QProcess>
#include <QThread>

#include "provisioningservice.h"
#include "provisioningservicedbus.h"

#include <wbxml.h>
#include <wbxml_config.h>

ProvisioningService::ProvisioningService(QObject *parent)
    : QObject(parent)
{
    qDebug() << ">>ProvisioningService";

    if(!QDBusConnection::sessionBus().registerService("org.nemomobile.provision"))
    {
        qDebug() << "Failed to register DBus service: " << QDBusConnection::sessionBus().lastError().message();
        return;
    }


    qDebug() << "<<ProvisioningService";
}

/*
 * Parse PROV 1.0 data, return all found Access Point Names
 */
QStringList ProvisioningService::Parse(const QByteArray &doc) {
    QXmlStreamReader xml;
    xml.addData(doc);
    QStringList apns;

    while (!xml.atEnd()) {
        xml.readNext();

        if (!xml.dtdName().isNull()){
            qDebug() << "DTD name: " << xml.dtdName();
        }
        if (!xml.dtdPublicId().isNull()){
            qDebug() << "DTD ID: " << xml.dtdPublicId();
        }

        /*
         * TODO: This is quick & lazy, of course we should check if this
         * is inside a 'CHARACTERISTIC' element
         */
		if (0 == xml.name().compare("PARM", Qt::CaseInsensitive)) {
			QXmlStreamAttributes attrs = xml.attributes();
			QVectorIterator<QXmlStreamAttribute> i(attrs);
			while (i.hasNext()) {
				QXmlStreamAttribute attr = i.next();
				if (0 == attr.value().compare("NAP-ADDRESS", Qt::CaseInsensitive)
				&& i.hasNext()) {
					attr = i.next();
					const QString apn = attr.value().toString();
					apns << apn;
				}
			}
    	}
    }
    if (xml.hasError()) {
          qDebug() << "Problem in parsing";
    }

    return apns;
}
/*
 * Implements the mmsd PushConsumer interface
 */
void ProvisioningService::Notify(const QByteArray &header, const QByteArray &body){
    qDebug() << ">>Notify";
	qDebug() << "header len " << header.length() << "body len " << body.length();

/* Writing to files, useful for testing
    QFile file_enc("/tmp/prov_body.wbxml");
    if (!file_enc.open(QIODevice::ReadWrite)) {
    	qDebug() << "no file";
    	return;
    }
    qDebug() << "Wrote " << file_enc.write(body,body.length()) << " encoded bytes";
    file_enc.close();

    QByteArray dst;
    bool success = this->decodeFromWbXML(body, dst, true);
    qDebug() << "Decode success: " << success;
    qDebug() << "Decoded data:\n" << dst;

    QFile file_dec("/tmp/prov_body_decoded.xml");
    if (!file_dec.open(QIODevice::ReadWrite)) {
    	qDebug() << "no file";
    	return;
    }
    qDebug() << "Wrote " << file_dec.write(dst,dst.length()) << " decoded bytes";
    file_dec.close();
*/

    qDebug() << "Start XML parsing";
    QStringList apns = this->Parse(dst);
    qDebug() << "Done XML parsing, found these APNs: " << apns;

    if (apns.count() > 0) {
    	qDebug() << "Creating oFono Context for APN " << apns.at(0);
    	/* TODO: Use proper D-Bus API, not oFono test python script */
    	/* TODO: Do this for all APNs and access point types */
    	QStringList arguments;
    	arguments << apns.at(0);
    	QProcess *scriptProcess = new QProcess(this);
    	scriptProcess->start("/usr/lib/ofono/test/create-internet-context", arguments);
    }

    qDebug() << "<<Notify";
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

    ProvisioningService *service = new ProvisioningService();
    new ProvisioningServiceDBusAdaptor(service);
    QDBusConnection::sessionBus().registerObject("/org/nemomobile/provision", service);

    return app.exec();
}
