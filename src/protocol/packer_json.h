#pragma once

#include "packer.h"
//#include "message.h"
//#include "messageJson.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QObject>


class PackerJson : public Packer
{
    Q_OBJECT
public:
//    void _parseBuffer(const QByteArray& buf);
//    void _parseByte(const char& byte);

//    void decode(const QByteArray &data) override { _parseBuffer(data); };


    void _parssseByte(const char& byte)
    {
        static QByteArray buf;
        qDebug() << "parsing" << byte;
        switch(state) {
        case WaitStart:
            if (byte == '{') {
                buf.append(byte);
                state = WaitEnd;
            } else {
                errors++;
            }
            break;
        case WaitEnd:
            buf.append(byte);
            if (byte == '}') {
                QJsonDocument doc = QJsonDocument::fromJson(buf);
                if (doc.isNull()) {
                    // error
                    qDebug() << "Error parsing JSON document from buffer";
                    errors++;
                } else {
                    if (!doc.isObject()) {
                        qDebug() << "Error JSON document is not object";
                        errors++;
                    } else {
                        parsed++;
                        QJsonObject obj= doc.object();
                        foreach(const QString& key, obj.keys()) {
                            QJsonValue value = obj.value(key);
                            qDebug() << "Key = " << key << ", Value = " << value;
                            emit newMessage(obj);
                        }
                    }
                }
                state = WaitStart;
                buf.clear();
            }
            break;
        }
    }

    void _parssseBuffer(const QByteArray& buf) {
        for (int i = 0; i < buf.size(); i++) {
            _parssseByte(buf.at(i));
        }
    }
    uint32_t parsed;
    uint32_t errors;
    typedef enum {
        WaitStart = 0,
        WaitEnd
    } state_t;

    state_t state;



signals:
    void newMessage(const QJsonObject& obj);
};


