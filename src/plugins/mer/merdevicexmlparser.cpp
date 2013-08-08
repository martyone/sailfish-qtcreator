/****************************************************************************
**
** Copyright (C) 2012 - 2013 Jolla Ltd.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Digia.
**
****************************************************************************/

#include "merdevicexmlparser.h"

#include <utils/fileutils.h>

#include <QAbstractMessageHandler>
#include <QAbstractXmlReceiver>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlQuery>
#include <QXmlStreamWriter>
#include <QStack>
#include <QFileInfo>

//Parser for:
//https://wiki.merproject.org/wiki/SDK_on_VirtualBox/Design

//<devices>
// <engine name="Mer SDK" type="vbox">
//  <subnet>10.220.220</subnet
// </engine>
// <device name="Nemo N9" type="real">
//  <ip>192.168.0.12</ip>
//  <sshkeypath></sshkeypath>
// </device>
// <device name="SailfishOS Emulator" type="vbox">
//  <index>2</index>
//  <subnet>10.220.220</subnet>
//  <sshkeypath></sshkeypath>
//  <mac>08:00:27:7C:A1:AF</mac>
// </device>
//</devices>

const char DEVICE[] = "device";
const char DEVICES[] = "devices";
const char NAME[] = "name";
const char TYPE[] = "type";
const char ENGINE[] = "engine";
const char IP[] = "ip";
const char SSH_PATH[] = "sshkeypath";
const char SUBNET[] = "subnet";
const char MAC[] = "mac";
const char INDEX[] = "index";

namespace Mer {

namespace {

class MessageHandler : public QAbstractMessageHandler
{
public:
    QString errorString() const
    {
        return QObject::tr("Error %1:%2:%3 %4").arg(m_identifier.toString())
                .arg(m_sourceLocation.line())
                .arg(m_sourceLocation.column())
                .arg(m_description);
    }

protected:
    virtual void handleMessage(QtMsgType /*type*/, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation)
    {
        m_description = description;
        m_identifier = identifier;
        m_sourceLocation = sourceLocation;
    }

private:
    QString m_description;
    QSourceLocation m_sourceLocation;
    QUrl m_identifier;
};

class XmlReceiver : public QAbstractXmlReceiver
{
public:
    XmlReceiver(const QXmlNamePool &namePool) : m_namePool(namePool) {}

    QList<MerDeviceData> deviceData() const
    {
        return m_devices;
    }

    MerEngineData engineData() const
    {
        return m_engineData;
    }


protected:
    virtual void startElement(const QXmlName &name)
    {
        const QString element = name.localName(m_namePool);
        if (element == QLatin1String(DEVICE))
            m_currentDevice.clear();
        if (element == QLatin1String(ENGINE))
            m_engineData.clear();
        m_currentElementStack.push(element);

    }

    virtual void endElement()
    {
        const QString element = m_currentElementStack.pop();
        if (element == QLatin1String(DEVICE)) {
            m_devices.append(m_currentDevice);
            m_currentDevice.clear();
        }
        if (element == QLatin1String(ENGINE)) {
            m_engineData.m_name = m_currentDevice.m_name;
            m_engineData.m_type = m_currentDevice.m_type;
            m_currentDevice.clear();
        }
    }

    virtual void attribute(const QXmlName &name,
                           const QStringRef &value)
    {
        const QString attributeName = name.localName(m_namePool);
        const QString attributeValue = value.toString();
        const QString element = m_currentElementStack.top();
        if (element == QLatin1String(DEVICE) || element == QLatin1String(ENGINE))
        {
            if(attributeName == QLatin1String(NAME))
                 m_currentDevice.m_name = attributeValue;
            if(attributeName == QLatin1String(TYPE))
                 m_currentDevice.m_type = attributeValue;
        }
    }

    virtual void characters(const QStringRef &value)
    {
        const QString element = m_currentElementStack.top();
        if (element == QLatin1String(IP))
            m_currentDevice.m_ip = value.toString();
        if (element == QLatin1String(SUBNET))
            m_currentDevice.m_subNet = value.toString();
        if (element == QLatin1String(MAC))
            m_currentDevice.m_name = value.toString();
        if (element == QLatin1String(SSH_PATH))
            m_currentDevice.m_sshKeyPath = value.toString();
        if (element == QLatin1String(NAME))
            m_currentDevice.m_name = value.toString();
        if (element == QLatin1String(TYPE))
            m_currentDevice.m_type = value.toString();
        if (element == QLatin1String(INDEX))
            m_currentDevice.m_index = value.toString().toInt();
    }

    virtual void comment(const QString &/*value*/)
    {
    }

    virtual void startDocument()
    {
    }

    virtual void endDocument()
    {
    }

    virtual void processingInstruction(const QXmlName &/*target*/, const QString &/*value*/)
    {
    }

    virtual void atomicValue(const QVariant &/*value*/)
    {
    }
    virtual void namespaceBinding(const QXmlName &/*name*/)
    {
    }
    virtual void startOfSequence()
    {
    }
    virtual void endOfSequence()
    {
    }

private:
    QXmlNamePool m_namePool;
    QStack<QString> m_currentElementStack;
    QList<MerDeviceData> m_devices;
    MerDeviceData m_currentDevice;
    MerEngineData m_engineData;
    QString m_attributeValue;
};

#ifdef Q_OS_MAC
#  define SHARE_PATH "/../Resources"
#else
#  define SHARE_PATH "/../share/qtcreator"
#endif

static QString applicationDirPath()
{
    return QCoreApplication::applicationDirPath();
}

static inline QString sharedDirPath()
{
    QString appPath = applicationDirPath();

    return QFileInfo(appPath + QLatin1String(SHARE_PATH)).absoluteFilePath();
}

} // Anonymous

class MerDevicesXmlReaderPrivate
{
public:
    MerDevicesXmlReaderPrivate()
        : receiver(new XmlReceiver(query.namePool())),
          error(false)
    {}

    ~MerDevicesXmlReaderPrivate()
    {
        delete receiver;
    }

    QXmlQuery query;
    XmlReceiver *receiver;
    MessageHandler messageHandler;
    bool error;
    QString errorString;
};

MerDevicesXmlReader::MerDevicesXmlReader(const QString &fileName, QObject *parent)
    : QObject(parent),
      d(new MerDevicesXmlReaderPrivate)
{
    Utils::FileReader reader;
    d->error = !reader.fetch(fileName, QIODevice::ReadOnly);
    if (d->error) {
        d->errorString = reader.errorString();
        return;
    }

    QXmlSchema schema;
    schema.setMessageHandler(&d->messageHandler);

    Utils::FileReader schemeReader;
    d->error = !schemeReader.fetch(QString::fromLatin1("%1/mer/devices.xsd").arg(sharedDirPath()),
            QIODevice::ReadOnly);
    if (d->error) {
        d->errorString = schemeReader.errorString();
        return;
    }
    schema.load(schemeReader.data());
    d->error = !schema.isValid();
    if (d->error) {
        d->errorString = d->messageHandler.errorString();
        return;
    }

    QXmlSchemaValidator validator(schema);
    validator.setMessageHandler(&d->messageHandler);
    d->error = !validator.validate(reader.data());
    if (d->error) {
        d->errorString = d->messageHandler.errorString();
        return;
    }

    d->query.setQuery(QString::fromLatin1("doc('%1')").arg(fileName));
    d->query.setMessageHandler(&d->messageHandler);
    d->error = !d->query.isValid();
    if (d->error) {
        d->errorString = d->messageHandler.errorString();
        return;
    }

    d->error = !d->query.evaluateTo(d->receiver);
    if (d->error)
        d->errorString = d->messageHandler.errorString();
}

MerDevicesXmlReader::~MerDevicesXmlReader()
{
    delete d;
}

bool MerDevicesXmlReader::hasError() const
{
    return d->error;
}

QString MerDevicesXmlReader::errorString() const
{
    return d->errorString;
}

QList<MerDeviceData> MerDevicesXmlReader::deviceData() const
{
    return d->receiver->deviceData();
}

MerEngineData MerDevicesXmlReader::engineData() const
{
    return d->receiver->engineData();
}

void MerDeviceData::clear()
{
    m_name.clear();
    m_type.clear();
    m_ip.clear();
    m_subNet.clear();
    m_mac.clear();
    m_sshKeyPath.clear();
}

void MerEngineData::clear()
{
    m_name.clear();
    m_type.clear();
    m_subNet.clear();
}

class MerDevicesXmlWriterPrivate
{
public:
    MerDevicesXmlWriterPrivate(const QString &fileName) : fileSaver(fileName, QIODevice::WriteOnly) {}

    Utils::FileSaver fileSaver;
};

MerDevicesXmlWriter::MerDevicesXmlWriter(const QString &fileName,
                                         const QList<MerDeviceData> &devices,  const MerEngineData &engine, QObject *parent)
    : QObject(parent),
      d(new MerDevicesXmlWriterPrivate(fileName))
{
    QByteArray data;
    QXmlStreamWriter writer(&data);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QLatin1String(DEVICES));
    writer.writeStartElement(QLatin1String(ENGINE));
    writer.writeAttribute(QLatin1String(NAME), engine.m_name);
    writer.writeAttribute(QLatin1String(TYPE), engine.m_type);
    writer.writeStartElement(QLatin1String(SUBNET));
    writer.writeCharacters(engine.m_subNet);
    writer.writeEndElement(); // subnet
    writer.writeEndElement(); // eninge
    foreach (const MerDeviceData &data, devices) {
        writer.writeStartElement(QLatin1String(DEVICE));
        writer.writeAttribute(QLatin1String(NAME), data.m_name);
        writer.writeAttribute(QLatin1String(TYPE), data.m_type);
        if(!data.m_index.isEmpty()) {
            writer.writeStartElement(QLatin1String(INDEX));
            writer.writeCharacters(data.m_index);
            writer.writeEndElement(); // index
        }
        if(!data.m_ip.isEmpty()) {
            writer.writeStartElement(QLatin1String(IP));
            writer.writeCharacters(data.m_ip);
            writer.writeEndElement(); // ip
        }
        if(!data.m_subNet.isEmpty()) {
            writer.writeStartElement(QLatin1String(SUBNET));
            writer.writeCharacters(data.m_subNet);
            writer.writeEndElement(); // subnet
        }
        if(!data.m_sshKeyPath.isEmpty()) {
            writer.writeStartElement(QLatin1String(SSH_PATH));
            writer.writeCharacters(data.m_sshKeyPath);
            writer.writeEndElement(); // ssh
        }
        if(!data.m_mac.isEmpty()) {
            writer.writeStartElement(QLatin1String(MAC));
            writer.writeCharacters(data.m_mac);
            writer.writeEndElement(); // ssh
        }
        writer.writeEndElement(); // device
    }
    {
        // This is a hack. This hack allows external modifications to the xml file for devices
        // Only devices that are of type=vbox are overwritten.
        // REMOVE THESE LINES
        // STARTS HERE
        Utils::FileReader fileReader;
        if (fileReader.fetch(fileName)) {
            QXmlStreamReader xmlReader(fileReader.data());
            while (!xmlReader.atEnd()) {
                xmlReader.readNext();
                if (xmlReader.isStartElement() && xmlReader.name().toLatin1() == DEVICE) {
                    QXmlStreamAttributes attributes = xmlReader.attributes();
                    if (attributes.value(QLatin1String(TYPE)) != QLatin1String("vbox")) {
                        writer.writeStartElement(QLatin1String(DEVICE));
                        writer.writeAttributes(attributes);
                        while (xmlReader.readNext()) {
                            if (xmlReader.isStartElement()) {
                                writer.writeStartElement(xmlReader.name().toString());
                                attributes = xmlReader.attributes();
                                writer.writeAttributes(attributes);
                            } else if (xmlReader.isEndElement()) {
                                writer.writeEndElement();
                                if (xmlReader.name() == DEVICE)
                                    break;
                            } else if (xmlReader.isCharacters()) {
                                writer.writeCharacters(xmlReader.text().toString());
                            }
                        }
                    }
                }
            }
        }
        // ENDS HERE
    }
    writer.writeEndElement(); // devices
    writer.writeEndDocument();
    d->fileSaver.write(data);
    d->fileSaver.finalize();
}

MerDevicesXmlWriter::~MerDevicesXmlWriter()
{
    delete d;
}

bool MerDevicesXmlWriter::hasError() const
{
    return d->fileSaver.hasError();
}

QString MerDevicesXmlWriter::errorString() const
{
    return d->fileSaver.errorString();
}

} // Mer
