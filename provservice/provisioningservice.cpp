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
#include <QXmlStreamReader>
#include <QtDBus>

#include "provisioningservice.h"
#include "provisioningservicedbus.h"

#include <wbxml.h>

/*
 * Constructor
 */
ProvisioningService::ProvisioningService(QObject *parent)
    : QObject(parent)
{

#if 0 /* Reading from file, useful for testing */
    QFile file_bodyenc("/tmp/prov_body.wbxml");
    if (!file_bodyenc.open(QIODevice::ReadOnly)) {
        qDebug() << "no file";
        return;
    }
    QByteArray body = file_bodyenc.readAll();
    qDebug() << "Read " << body.length() << " encoded bytes";
    file_bodyenc.close();
    this->Notify(QByteArray(), body);
#else
    if(!QDBusConnection::sessionBus().registerService("org.nemomobile.provision"))
    {
        qDebug() << "Failed to register DBus service: " << QDBusConnection::sessionBus().lastError().message();
    }
#endif

    qDebug() << "ProvisioningService constructed.";
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
    qDebug() << "Notify(), header len " << header.length() <<
        ", body len " << body.length();

#if 0 /*  Writing to file, useful for testing */
    QFile file_headerenc("/tmp/prov_header.wbxml");
    if (!file_headerenc.open(QIODevice::ReadWrite)) {
        qDebug() << "no file";
        return;
    }
    qDebug() << "Wrote " << file_headerenc.write(header,header.length()) << " encoded bytes";
    file_headerenc.close();

    QFile file_enc("/tmp/prov_body.wbxml");
    if (!file_enc.open(QIODevice::ReadWrite)) {
        qDebug() << "no file";
        return;
    }
    qDebug() << "Wrote " << file_enc.write(body,body.length()) << " encoded bytes";
    file_enc.close();
#endif

    /* Decoding WBXML ==> Prov 1.0 XML */
    QByteArray dst;
    bool success = this->decodeWBXML(body, dst);
    qDebug() << "Decode success: " << success;
    qDebug() << "Decoded data:\n" << dst;

#if 0 /*  Writing to file, useful for testing */
    QFile file_dec("/tmp/prov_body_decoded.xml");
    if (!file_dec.open(QIODevice::ReadWrite)) {
        qDebug() << "no file";
        return;
    }
    qDebug() << "Wrote " << file_dec.write(dst,dst.length()) << " decoded bytes";
    file_dec.close();
#endif

    /* Parsing Prov 1.0 XML ==> actual internet, MMS etc. settings */
    qDebug() << "Start XML parsing";
    QStringList apns = this->Parse(dst);
    qDebug() << "Done XML parsing, found these APNs: " << apns;

    QStringListIterator it(apns);
    while (it.hasNext()) {

        const QString apn = it.next();
        qDebug() << "Creating oFono Context for APN " << apn;

        QDBusInterface ofonoConnMan("org.ofono", "/ril_0" /* TODO hard-coded! */,
            "org.ofono.ConnectionManager", QDBusConnection::systemBus());
        /* Create new context */
        QDBusReply<QDBusObjectPath> addReply = ofonoConnMan.call("AddContext",
            "internet" /* TODO context types (internet, mms, ...) */);
        if (addReply.isValid()) {

           /* Set context's AP name */
           QDBusInterface ofonoContext("org.ofono", addReply.value().path(),
               "org.ofono.ConnectionContext", QDBusConnection::systemBus());
           QDBusReply<void> propReply = ofonoContext.call("SetProperty",
               "AccessPointName", apn /* TODO wrong signature! */);
           if (!propReply.isValid())
               qDebug() << "DBus error in SetProperty: " << propReply.error();

        } else
            qDebug() << "DBus error in AddContext: " << addReply.error();

    }
}

/*
 * Decode WBXML to Prov 1.0 XML
 */
bool ProvisioningService::decodeWBXML(const QByteArray& wbxml, QByteArray& xml)
{
    WBXMLGenXMLParams params;
    params.lang = WBXML_LANG_PROV10;
    params.gen_type = WBXML_GEN_XML_INDENT;
    params.indent = 2;

    const WB_UTINY* wbxmlptr = reinterpret_cast<const WB_UTINY*>(wbxml.constData());
    WB_ULONG wbxml_len = wbxml.size();
    WB_UTINY* xmlptr;
    WB_ULONG xml_len;
    WBXMLError error = wbxml_conv_wbxml2xml_withlen(
        const_cast<WB_UTINY*>(wbxmlptr), wbxml_len, &xmlptr, &xml_len, &params);

    if( error == WBXML_OK ) {
        xml.append(reinterpret_cast<char*>( xmlptr ), xml_len);
        wbxml_free(xmlptr);
        return true;
    }

    qDebug() << "Decode failed: " << wbxml_errors_string(error);
    return false;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    ProvisioningService *service = new ProvisioningService();
    new ProvisioningServiceDBusAdaptor(service);
    QDBusConnection::sessionBus().registerObject("/org/nemomobile/provision",
        service);

    return app.exec();
}
