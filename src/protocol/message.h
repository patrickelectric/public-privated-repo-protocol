#pragma once

#include <QObject>

/// Message describes the types/contents/format of messages
class Message : public QObject
{
    Q_OBJECT
    Q_ENUMS(GeneralMessageID)
    Q_ENUMS(SonarMessagesID)
    Q_ENUMS(EchosounderMessageID)
public:
    Message();
    ~Message();

//    enum MessageTypes {

//        MessageTypePing,
//        MessageTypeMavlink,
//        MessageTypeJson
//    };

   /// class Message {
   ///
   /// const QByteArray data
   ///
   ///
   /// Message<MessageType> fromBuffer(QByteArray); // This does the logic to unpack a message, STATIC
    // QByteArray toBuffer(Message<MessageType>); // STATIC
   /// }
   ///
   /// class PingMessage {
   /// static const header;
   /// QByteArray data;
   ///
   /// const char* packString;
   /// uint8_t checksum;
   ///
   /// }
   ///
   /// class JsonMessage {
   /// QJsonObject obj;
   /// }

//    Message<MessageTypePing>;




    /// for ping protocol:
    /// 1. Wait for start 'BR'
    /// 2. Read payload length
    /// 3. Read id
    /// 4. Wait for length
    /// 5. Pass buffer to message to decode
    /// 6. get valid message object back from message
    ///
    ///
//    struct message {
//        uint8_t header[2];
//        QVariantList data;
//        const char* packString;
//        uint8_t checksum;
//    };

//    struct msg_gen_goto_bootloader {
//        uint8_t header[2];
//        QVariantList data;
//        static const char* packString;
//        uint8_t checksum;
//    };

//    struct msg_es_distance_simple {
//        uint8_t header[2];
//        QVariantList data;
//        static const char* packString;
//        uint8_t checksum;

////        uint32_t& distance = data[1];
////        uint8_t& confidence = data[2];
//    };

//    struct msg_es_distance {
//        uint8_t header[2];
//        QVariantList& data;
//        static const char* packString;
//        uint8_t checksum;

//        uint32_t& distance;
//        uint8_t& confidence;
//        uint16_t& pulse_usec;
//        uint32_t& ping_number;
//        uint32_t& start_mm;
//        uint32_t& length_mm;
//        uint32_t& gain_index;
//    };


    enum GeneralMessageID {
         gen_goto_bootloader = 100
        ,gen_get_version = 101
        ,gen_device_id = 110
        ,gen_new_data = 112
        ,gen_cmd_request = 120
    };

    enum SonarMessageID {
         sonar_set_velocity = 1000
    };

    enum EchosounderMessageID {
         es_distance_simple = 1100
        ,es_distance = 1101
        ,es_profile
        ,es_range = 1110
        ,es_mode
        ,es_rate
        ,es_gain
        ,es_pulse
    };

    enum MechanicalScanningSonarMessageID {
         mss_angle_profile = 1201
        ,mss_range = 1210
        ,mss_mode
        ,mss_gain
        ,mss_sector
    };

    static const QByteArray checksumPackString();
    static const QByteArray headerPackString();
    static const QByteArray packString(const QVariant& messageIDEnum);
    static const QString string(const QVariant& messageIDEnum);

private:
    static const QByteArray unpack(QString packString);
};
