#pragma once

#include "link.h"
#include "protocol.h"
class Sensor : public QObject
{
    Q_OBJECT
public:
    Sensor();
    ~Sensor();

    Q_PROPERTY(Protocol* protocol READ protocol NOTIFY protocolUpdate)
    Protocol* protocol() { return _protocol; };

    Q_PROPERTY(AbstractLink* link READ link NOTIFY linkUpdate)
    AbstractLink* link() { return _linkIn ? _linkIn->self() : nullptr; };

    Q_PROPERTY(AbstractLink* linkLog READ linkLog NOTIFY linkLogUpdate)
    AbstractLink* linkLog() { return _linkOut ? _linkOut->self() : nullptr; };

    Q_INVOKABLE void connectLink(const QString& connString);
    Q_INVOKABLE void connectLinkLog(const QString& connString);

private:
    Link* _linkIn;
    Link* _linkOut;
    Protocol* _protocol;

signals:
    // In
    void connectionClose();
    void connectionOpen();
    void protocolUpdate();
    void linkUpdate();

    // Out
    void linkLogUpdate();
};
