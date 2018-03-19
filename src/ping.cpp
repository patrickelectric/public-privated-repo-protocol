#include <QDateTime>
#include <QDebug>

#include "ping.h"
#include "seriallink.h"

Ping::Ping() :
     _linkIn(new Link(AbstractLink::LinkType::Serial, "Default"))
    ,_linkOut(nullptr)
    ,_protocol(new Protocol())
{
    emit linkUpdate();

    requestTimer.setInterval(1000);
    connect(&requestTimer, &QTimer::timeout, _protocol, &Protocol::requestEchosounderProfile);

    connect(&_flasher, &Stm32Flasher::flashProgress, this, &Ping::flashProgress);
    connect(&_flasher, &Stm32Flasher::flashComplete, this, &Ping::flashComplete);
    connect(&_flasher, &Stm32Flasher::flashComplete, this, &Ping::reconnectLink);

    connectLink("2:/dev/ttyUSB0:115200");
}

void Ping::firmwareUpdate(const QUrl& fileUrl)
{
    SerialLink* serialLink = dynamic_cast<SerialLink*>(link());

    if (!serialLink) {
        return;
    }

    if(link()->isOpen()) {
        setPollFrequency(0);

        _protocol->requestGotoBootloader();

        while (serialLink->bytesToWrite()) {
            qDebug() << "Waiting for bytes to be written";
            serialLink->waitForBytesWritten();
        }

        disconnect(link(), &AbstractLink::newData, _protocol, &Protocol::handleData);
        disconnect(_protocol, &Protocol::sendData, link(), &AbstractLink::sendData);
        disconnect(_protocol, &Protocol::update, this, &Ping::protocolUpdate);

        link()->finishConnection();

        if(_linkIn) {
            delete _linkIn;
        }
    } else {
        qDebug() << "Link not available to flash device.";
    }

    QSerialPortInfo pInfo(serialLink->QSerialPort::portName());
    #ifdef Q_OS_WIN
    QString portLocation = pInfo.portName();
    #else
    QString portLocation = pInfo.systemLocation();
    #endif
    QString file = QDir::toNativeSeparators(fileUrl.toLocalFile());
    qDebug() << portLocation << file << pInfo.portName();
    _flasher.flash(portLocation, file, false /*verify*/);
}

void Ping::reconnectLink(bool success) {
    if (!success) {
        qDebug() << "Error flashing!";
    }

    // TODO change to run autoconnect routine?, (same as app startup)
    connectLink(_linkConfigString);
}

void Ping::connectLink(const QString& connString)
{
    _linkConfigString = connString;

    if(link()->isOpen()) {
        link()->finishConnection();
        disconnect(link(), &AbstractLink::newData, _protocol, &Protocol::handleData);
        disconnect(_protocol, &Protocol::sendData, link(), &AbstractLink::sendData);
        disconnect(_protocol, &Protocol::update, this, &Ping::protocolUpdate);
    }

    QStringList confList = connString.split(':');
    if(confList.length() != 3) {
        qDebug() << "wrong size !";
        return;
    }
    if(confList[0].toInt() <= 0 || confList[0].toInt() > 5) {
        qDebug() << "wrong arg !";
        return;
    }
    if(_linkIn) {
        delete _linkIn;
    }
    _linkIn = new Link((AbstractLink::LinkType)confList[0].toInt(), "Default");
    confList.removeFirst();
    QString conf = confList.join(':');

    link()->setConfiguration(conf);
    link()->startConnection();

    if(!link()->isOpen()) {
        qDebug() << "Connection fail !" << connString << link()->errorString();;
        emit connectionClose();
        return;
    }

    emit linkUpdate();
    // Disable log if playing one
    if(link()->type() == AbstractLink::LinkType::File) {
        if(!_linkOut) {
            return;
        }
        if(linkLog()->isOpen()) {
            disconnect(_protocol, &Protocol::emitRawMessages, linkLog(), &AbstractLink::sendData);
            linkLog()->finishConnection();
            _linkOut->deleteLater();
        }
    } else { // Start log, if not playing one
        QString fileName = QStringLiteral("%1.%2").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-hhmmsszzz")), "bin");
        QString config = QStringLiteral("%1:%2:%3").arg(QString::number(1), fileName, "w");
        connectLinkLog(config);
    }
    connect(link(), &AbstractLink::newData, _protocol, &Protocol::handleData);
    connect(_protocol, &Protocol::sendData, link(), &AbstractLink::sendData);
    connect(_protocol, &Protocol::update, this, &Ping::protocolUpdate);

    emit connectionOpen();
}

void Ping::connectLinkLog(const QString& connString)
{
    if(_linkOut) {
        if(!link()->isOpen()) { // TODO should be linkout()?!?!
            qDebug() << "No connection to log !" << linkLog()->errorString();
            return;
        }
        disconnect(_protocol, &Protocol::emitRawMessages, linkLog(), &AbstractLink::sendData);

        delete _linkOut;
    }

    QStringList confList = connString.split(':');
    if(confList.length() != 3) {
        qDebug() << "wrong size !" << confList;
        return;
    }
    if(confList[0].toInt() <= 0 || confList[0].toInt() > 5) {
        qDebug() << "wrong arg !" << confList;
        return;
    }

    _linkOut = new Link((AbstractLink::LinkType)confList[0].toInt(), "Log");
    confList.removeFirst();
    QString conf = confList.join(':');

    linkLog()->setConfiguration(conf);
    linkLog()->startConnection();

    if(!linkLog()->isOpen()) {
        qDebug() << "Connection with log fail !" << connString << linkLog()->errorString();
        return;
    }

    connect(_protocol, &Protocol::emitRawMessages, linkLog(), &AbstractLink::sendData);
    emit linkLogUpdate();
}

QVariant Ping::pollFrequency()
{
    if (!requestTimer.isActive()) {
        return 0;
    }
    return 1000.0f / requestTimer.interval();
}

void Ping::setPollFrequency(QVariant pollFrequency)
{
    if (pollFrequency.toInt() <= 0) {
        if (requestTimer.isActive()) {
            requestTimer.stop();
        }
    } else {
        int period_ms = 1000.0f / pollFrequency.toInt();
        qDebug() << "setting f" << pollFrequency.toInt() << period_ms;
        requestTimer.setInterval(period_ms);
        if (!requestTimer.isActive()) {
            requestTimer.start();
        }
    }

    qDebug() << "Poll period" << pollFrequency;
    emit pollFrequencyUpdate();
}

Ping::~Ping()
{
}
