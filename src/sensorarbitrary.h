#pragma once

#include "sensor.h"

class SensorArbitrary : public Sensor
{
    Q_OBJECT
public:

    SensorArbitrary()
        : Sensor()
    {
        connect(_protocol,&Protocol::emitJson, this, &SensorArbitrary::update);
        Sensor::connectLink("2:/dev/ttyUSB2:460800");
    }

    Q_PROPERTY(QVariant value READ value NOTIFY valueUpdate)
    QVariant value() { return _value; };
    Q_PROPERTY(QString name READ name NOTIFY nameUpdate)
    QString name() { return _name; };

signals:
    void valueUpdate(QVariant value);
    void nameUpdate(QString name);

private:

    QString _name;
    QVariant _value;

    void update(const QJsonObject& obj) {
        _name = (obj.begin().key());
        emit nameUpdate(_name);
        _value = (obj.begin().value().toVariant());
        emit valueUpdate(_value);
    };

    // TODO, maybe store history/filtered history of values in this object for access by different visual elements without need to recompute
};
