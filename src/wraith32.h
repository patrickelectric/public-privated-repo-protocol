#pragma once

#include "sensor.h"

class SensorWraith32 : public Sensor
{
    Q_OBJECT
public:

    SensorWraith32()
        : Sensor()
    {
        connect(_protocol,&Protocol::emitJson, this, &SensorWraith32::update);
        Sensor::connectLink("2:/dev/ttyUSB2:460800");
    }

    Q_PROPERTY(QVariant input READ input NOTIFY inputUpdate)
    QVariant input() { return _input; };

    Q_PROPERTY(QVariant voltage READ voltage NOTIFY voltageUpdate)
    QVariant voltage() { return _voltage; };

    Q_PROPERTY(QVariant current READ current NOTIFY currentUpdate)
    QVariant current() { return _current; };

    Q_PROPERTY(QVariant rpm READ rpm NOTIFY rpmUpdate)
    QVariant rpm() { return _rpm; };

signals:
    void inputUpdate(QVariant input);
    void voltageUpdate(QVariant voltage);
    void currentUpdate(QVariant current);
    void rpmUpdate(QVariant rpm);

private:
    QString _name;
    QVariant _input;
    QVariant _voltage;
    QVariant _current;
    QVariant _rpm;

    void update(const QJsonObject& obj) {
        foreach(const QString& key, obj.keys()) {
            QJsonValue value = obj.value(key);
            if (key == "RPM") {
                _rpm = value;
                emit rpmUpdate(_rpm);
            } else if (key == "Input") {
                _input = value;
                emit inputUpdate(_input);
            } else if (key == "Voltage") {
                _voltage = value;
                emit voltageUpdate(_voltage);
            } else if (key == "Current") {
                _current = value;
                emit inputUpdate(_current);
            }
        }
    };

    // TODO, maybe store history/filtered history of values in this object for access by different visual elements without need to recompute
};
