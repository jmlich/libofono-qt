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

#include "ofonosimmanager.h"
#include "ofonointerface.h"

OfonoSimManager::OfonoSimManager(OfonoModem::SelectionSetting modemSetting, const QString &modemPath, QObject *parent)
    : OfonoModemInterface(modemSetting, modemPath, "org.ofono.SimManager", OfonoGetAllOnStartup, parent)
{
    qRegisterMetaType<OfonoServiceNumbers>("OfonoServiceNumbers");
    qRegisterMetaType<OfonoPinRetries>("OfonoPinRetries");
    connect(m_if, SIGNAL(propertyChanged(const QString&, const QVariant&)), 
            this, SLOT(propertyChanged(const QString&, const QVariant&)));
    connect(m_if, SIGNAL(setPropertyFailed(const QString&)), 
            this, SLOT(setPropertyFailed(const QString&)));

}

OfonoSimManager::~OfonoSimManager()
{
}

void OfonoSimManager::changePin(const QString &pintype, const QString &oldpin, const QString &newpin)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "ChangePin");
    request << pintype << oldpin << newpin;

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(changePinResp()),
					SLOT(changePinErr(const QDBusError&)));
}

void OfonoSimManager::enterPin(const QString &pintype, const QString &pin)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "EnterPin");
    request << pintype << pin;

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(enterPinResp()),
					SLOT(enterPinErr(const QDBusError&)));
}

void OfonoSimManager::resetPin(const QString &pintype, const QString &puk, const QString &newpin)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "ResetPin");
    request << pintype << puk << newpin;

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(resetPinResp()),
					SLOT(resetPinErr(const QDBusError&)));
}

void OfonoSimManager::lockPin(const QString &pintype, const QString &pin)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "LockPin");
    request << pintype << pin;

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(lockPinResp()),
					SLOT(lockPinErr(const QDBusError&)));
}

void OfonoSimManager::unlockPin(const QString &pintype, const QString &pin)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "UnlockPin");
    request << pintype << pin;

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(unlockPinResp()),
					SLOT(unlockPinErr(const QDBusError&)));
}

void OfonoSimManager::getIcon(quint8 id)
{
    QDBusMessage request;

    request = QDBusMessage::createMethodCall("org.ofono",
					     path(), m_if->ifname(),
					     "GetIcon");
    request << QVariant::fromValue(id);

    QDBusConnection::systemBus().callWithCallback(request, this,
					SLOT(getIconResp(QByteArray)),
					SLOT(getIconErr(const QDBusError&)));
}

void OfonoSimManager::setSubscriberNumbers(const QStringList &numbers)
{
    m_if->setProperty("SubscriberNumbers", QVariant::fromValue(numbers));
}

bool OfonoSimManager::present() const
{
    return m_if->properties()["Present"].value<bool>();
}

QString OfonoSimManager::subscriberIdentity() const
{
    return m_if->properties()["SubscriberIdentity"].value<QString>();
}

QString OfonoSimManager::mobileCountryCode() const
{
    return m_if->properties()["MobileCountryCode"].value<QString>();
}

QString OfonoSimManager::mobileNetworkCode() const
{
    return m_if->properties()["MobileNetworkCode"].value<QString>();
}

QStringList OfonoSimManager::subscriberNumbers() const
{
    return m_if->properties()["SubscriberNumbers"].value<QStringList>();
}

OfonoServiceNumbers OfonoSimManager::serviceNumbers() const
{
    OfonoServiceNumbers map;
    m_if->properties()["ServiceNumbers"].value<QDBusArgument>() >> map;
    return map;
}

QString OfonoSimManager::pinRequired() const
{
    return m_if->properties()["PinRequired"].value<QString>();
}

QStringList OfonoSimManager::lockedPins() const
{
    return m_if->properties()["LockedPins"].value<QStringList>();
}

QString OfonoSimManager::cardIdentifier() const
{
    return m_if->properties()["CardIdentifier"].value<QString>();
}

QStringList OfonoSimManager::preferredLanguages() const
{
    return m_if->properties()["PreferredLanguages"].value<QStringList>();
}

OfonoPinRetries OfonoSimManager::pinRetries() const
{
    OfonoPinRetries retries;
    m_if->properties()["Retries"].value<QDBusArgument>() >> retries;
    return retries;
}

bool OfonoSimManager::fixedDialing() const
{
    return m_if->properties()["FixedDialing"].value<bool>();
}

bool OfonoSimManager::barredDialing() const
{
    return m_if->properties()["BarredDialing"].value<bool>();
}

void OfonoSimManager::propertyChanged(const QString& property, const QVariant& value)
{
    if (property == "Present") {	
        emit presenceChanged(value.value<bool>());
    } else if (property == "SubscriberIdentity") {	
        emit subscriberIdentityChanged(value.value<QString>());
    } else if (property == "MobileCountryCode") {	
        emit mobileCountryCodeChanged(value.value<QString>());
    } else if (property == "MobileNetworkCode") {	
        emit mobileNetworkCodeChanged(value.value<QString>());
    } else if (property == "SubscriberNumbers") {	
        emit subscriberNumbersChanged(value.value<QStringList>());
    } else if (property == "ServiceNumbers") {	
        OfonoServiceNumbers map;
        value.value<QDBusArgument>() >> map;
        emit serviceNumbersChanged(map);
    } else if (property == "PinRequired") {	
        emit pinRequiredChanged(value.value<QString>());
    } else if (property == "LockedPins") {
        emit lockedPinsChanged(value.value<QStringList>());
    } else if (property == "CardIdentifier") {	
        emit cardIdentifierChanged(value.value<QString>());
    } else if (property == "PreferredLanguages") {	
        emit preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == "Retries") {
        OfonoPinRetries retries;
        value.value<QDBusArgument>() >> retries;
        emit pinRetriesChanged(retries);
    } else if (property == "FixedDialing") {	
        emit fixedDialingChanged(value.value<bool>());
    } else if (property == "BarredDialing") {	
        emit barredDialingChanged(value.value<bool>());
    }
}

void OfonoSimManager::setPropertyFailed(const QString& property)
{
    if (property == "SubscriberNumbers")
        emit setSubscriberNumbersFailed();
}

void OfonoSimManager::changePinResp()
{
    emit changePinComplete(true);
}

void OfonoSimManager::changePinErr(QDBusError error)
{
    m_if->setError(error.name(), error.message());    
    emit changePinComplete(false);
}

void OfonoSimManager::enterPinResp()
{
    emit enterPinComplete(true);
}

void OfonoSimManager::enterPinErr(QDBusError error)
{
    m_if->setError(error.name(), error.message());
    emit enterPinComplete(false);
}

void OfonoSimManager::resetPinResp()
{
    emit resetPinComplete(true);
}

void OfonoSimManager::resetPinErr(QDBusError error)
{
    m_if->setError(error.name(), error.message());
    emit resetPinComplete(false);
}

void OfonoSimManager::lockPinResp()
{
    emit lockPinComplete(true);
}
void OfonoSimManager::lockPinErr(QDBusError error)
{
    m_if->setError(error.name(), error.message());
    emit lockPinComplete(false);
}

void OfonoSimManager::unlockPinResp()
{
    emit unlockPinComplete(true);
}
void OfonoSimManager::unlockPinErr(QDBusError error)
{
    m_if->setError(error.name(), error.message()); 
    emit unlockPinComplete(false);
}

void OfonoSimManager::getIconResp(QByteArray icon)
{
    emit getIconComplete(true, icon);
}
void OfonoSimManager::getIconErr(QDBusError error)
{
    m_if->setError(error.name(), error.message()); 
    emit getIconComplete(false, QByteArray());
}
