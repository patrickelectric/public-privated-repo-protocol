#pragma once

class Packer
{
public:
    Packer();
    ~Packer();

    bool validadeData(const QByteArray& data);
    QVariantList decode(QByteArray data);
    QString checkPackString(const QString& packString);
    QVariantList unpack(const QString& packString, QByteArray data);
    QVariant undo(QByteArray& data, const QChar& format);
    QByteArray messagePack(const QVariant& messageID, const QVariant& var);
    QByteArray messagePack(const QVariant& messageID, const QVariantList& var);
    QByteArray pack(const QByteArray& packString, const QVariant& var);
    QByteArray pack(const QByteArray& packString, const QVariantList& varList);
    QByteArray convert(const QVariant& var, const QChar& format);
    QByteArray merge(const QByteArray& header, const QByteArray& payload);
    QByteArray populateHeader(int messageID = 120, int srcDevID = 0, int dstDevID = 0, int payload = -1);
    QByteArray request(const QVariant& messageID, int srcDevID = 0, int dstDevID = 0);
};