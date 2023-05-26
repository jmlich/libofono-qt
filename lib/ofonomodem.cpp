/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtDBus/QtDBus>
#include <QtCore/QObject>

#include "ofonomodem.h"
#include "ofonointerface.h"
#include "ofonomodemmanager.h"

OfonoModem::OfonoModem(SelectionSetting setting, const QString &modemPath, QObject *parent)
	: QObject(parent), m_selectionSetting(setting)
{
    
    m_mm = new OfonoModemManager(this);
    connect(m_mm, SIGNAL(modemAdded(QString)), this, SLOT(modemAdded(QString)));
    connect(m_mm, SIGNAL(modemRemoved(QString)), this, SLOT(modemRemoved(QString)));

    QString finalModemPath;

    if (setting == AutomaticSelect)
        finalModemPath = m_mm->modems().value(0);
    else if (setting == ManualSelect)
        finalModemPath = modemPath;
    
    if (finalModemPath.isEmpty()) {
        finalModemPath = "/";
    } 
    m_if = new OfonoInterface(finalModemPath, "org.ofono.Modem", OfonoGetAllOnStartup, this);
    connect(m_if, SIGNAL(propertyChanged(const QString&, const QVariant&)), 
            this, SLOT(propertyChanged(const QString&, const QVariant&)));
    connect(m_if, SIGNAL(setPropertyFailed(const QString&)), 
            this, SLOT(setPropertyFailed(const QString&)));
    m_isValid = m_mm->modems().contains(finalModemPath);
}

OfonoModem::~OfonoModem()
{
}

void OfonoModem::propertyChanged(const QString& property, const QVariant& value)
{
    if (property == "Online")
        emit onlineChanged(value.value<bool>());
    else if (property == "Powered")
        emit poweredChanged(value.value<bool>());
    else if (property == "Lockdown")
        emit lockdownChanged(value.value<bool>());
    else if (property == "Emergency")
        emit emergencyChanged(value.value<bool>());
    else if (property == "Name")
        emit nameChanged(value.value<QString>());
    else if (property == "Manufacturer")
        emit manufacturerChanged(value.value<QString>());
    else if (property == "Model")
        emit modelChanged(value.value<QString>());
    else if (property == "Revision")
        emit revisionChanged(value.value<QString>());
    else if (property == "Serial")
        emit serialChanged(value.value<QString>());
    else if (property == "Type")
        emit typeChanged(value.value<QString>());
    else if (property == "Features")
        emit featuresChanged(value.value<QStringList>());
    else if (property == "Interfaces")
        emit interfacesChanged(value.value<QStringList>());
}

void OfonoModem::setPropertyFailed(const QString& property)
{
    if (property == "Online")
        emit setOnlineFailed();
    else if (property == "Powered")
        emit setPoweredFailed();
    else if (property == "Lockdown")
        emit setLockdownFailed();
}

void OfonoModem::modemAdded(const QString& /*modem*/)
{
    modemsChanged();
}

void OfonoModem::modemRemoved(const QString& /*modem*/)
{
    modemsChanged();
}

void OfonoModem::modemsChanged()
{
    // validity has changed
    if (isValid() != m_mm->modems().contains(path())) {
        m_isValid = m_mm->modems().contains(path());
        emit validityChanged(isValid());
    }
    if (!m_mm->modems().contains(path())) {
        if (m_selectionSetting == AutomaticSelect) {
            QString modemPath = m_mm->modems().value(0);
            if (modemPath.isEmpty()) {
                modemPath = "/";
            }
            m_if->setPath(modemPath);
            emit pathChanged(modemPath);
        }
    }
    // validity has changed
    if (isValid() != m_mm->modems().contains(path())) {
        m_isValid = m_mm->modems().contains(path());
        emit validityChanged(isValid());
    }
}


bool OfonoModem::isValid() const
{
    return m_isValid;
}

QString OfonoModem::path() const
{
    return m_if->path();
}

QString OfonoModem::errorName() const
{
    return m_if->errorMessage();
}

QString OfonoModem::errorMessage() const
{
    return m_if->errorMessage();
}

bool OfonoModem::powered() const
{
    return m_if->properties()["Powered"].value<bool>();
}

void OfonoModem::setPowered(bool powered)
{
    m_if->setProperty("Powered", QVariant::fromValue(powered));
}

bool OfonoModem::online() const
{
    return m_if->properties()["Online"].value<bool>();
}

void OfonoModem::setOnline(bool online)
{
    m_if->setProperty("Online", QVariant::fromValue(online));
}

bool OfonoModem::lockdown() const
{
    return m_if->properties()["Lockdown"].value<bool>();
}

void OfonoModem::setLockdown(bool lockdown)
{
    m_if->setProperty("Lockdown", QVariant::fromValue(lockdown));
}

bool OfonoModem::emergency() const
{
    return m_if->properties()["Emergency"].value<bool>();
}

QString OfonoModem::name() const
{
    return m_if->properties()["Name"].value<QString>();
}

QString OfonoModem::manufacturer() const
{
    return m_if->properties()["Manufacturer"].value<QString>();
}

QString OfonoModem::model() const
{
    return m_if->properties()["Model"].value<QString>();
}

QString OfonoModem::revision() const
{
    return m_if->properties()["Revision"].value<QString>();
}

QString OfonoModem::serial() const
{
    return m_if->properties()["Serial"].value<QString>();
}

QString OfonoModem::type() const
{
    return m_if->properties()["Type"].value<QString>();
}

QStringList OfonoModem::features() const
{
    return m_if->properties()["Features"].value<QStringList>();
}

QStringList OfonoModem::interfaces() const
{
    return m_if->properties()["Interfaces"].value<QStringList>();
}


