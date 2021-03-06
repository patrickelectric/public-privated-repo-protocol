#pragma once

#include <QDataStream>
#include <QFile>
#include <QTime>

#include "abstractlink.h"
#include "logthread.h"

class FileLink : public AbstractLink
{
public:
    FileLink();
    ~FileLink();

    bool isOpen() final { return _file.isWritable() && _file.isReadable(); };
    bool setConfiguration(const QString& arg) final;
    bool startConnection() final;
    bool finishConnection() final;
    QString errorString() final { return _file.errorString(); };

    bool isOnline() final { return false; };
    void start() final { if(_logThread) _logThread->startJob(); };
    void pause() final { if(_logThread) _logThread->pauseJob(); };
    qint64 byteSize() final { return _file.bytesAvailable(); };
    int packageSize() final { return _logThread ? _logThread->packageSize() : 0; };
    int packageIndex() final { return _logThread ? _logThread->packageIndex() : 0; };
    void setPackageIndex(int index) { if(_logThread) _logThread->setPackageIndex(index); }
    QTime totalTime() final { return _logThread ? _logThread->totalTime() : QTime(); };
    QTime elapsedTime() final { return _logThread ? _logThread->elapsedTime() : QTime(); };

private:
    struct Pack {
        QString time;
        QByteArray data;
    };

    QIODevice::OpenModeFlag _openModeFlag;
    QTime _time;

    QFile _file;
    QDataStream _inout;

    LogThread* _logThread;

    void _writeData(const QByteArray& data);
};
