#pragma once

#include <QObject>
#include <QVariantList>

class Packer : public QObject
{
    Q_OBJECT
public:
    Packer();
    ~Packer();

    enum ValidateData {
        NewPackage = 0,
        WrongStart,
        WrongCheckSum,
        NeedMoreData,
        NoData,
    };

    void decode(const QByteArray& data);
    QByteArray createPack(int messageID, const QVariantList& vars, int srcDevID = 0, int dstDevID = 0);
    QByteArray request(int messageID, int srcDevID = 0, int dstDevID = 0);

private:
    ValidateData validateData(QByteArray& data, QVariantList& package);
    QVariantList unpack(const QString& packString, QByteArray data);
    QVariant unconvert(QByteArray& data, const QChar& format);
    QByteArray messagePack(int messageID, const QVariant& var);
    QByteArray messagePack(int messageID, const QVariantList& var);
    QByteArray pack(const QByteArray& packString, const QVariant& var);
    QByteArray pack(const QByteArray& packString, const QVariantList& varList);
    int byteInFormatString(const QString& formatString);
    QByteArray convert(const QVariant& var, const QChar& format);
    QByteArray merge(const QByteArray& header, const QByteArray& payload);
    QByteArray populateHeader(int messageID = 120, int srcDevID = 0, int dstDevID = 0, int payload = -1);

signals:
    void newPackage(const QVariantList& package);
    void newRawPackage(const QByteArray& package);
};
