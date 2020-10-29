#ifndef __MQTT_TYPES__
#define __MQTT_TYPES__

#include <mbed.h>
#include <map>
using namespace std;
namespace SDT_MQTT
{
    enum MQTTQoS
    {
        MQTT_QOS_0 = 0,
        MQTT_QOS_1 = 1,
        MQTT_QOS_2 = 2
    };

    enum MQTTVersion
    {
        MQTT_VER_3_1 = 3,
        MQTT_VER_3_1_1 = 4,
        MQTT_VER_5 = 5
    };

    enum MQTTStatus
    {
        MQTT_STATUS_SUCCESS = 0,
        MQTT_STATUS_FAILURE = -1,
        MQTT_STATUS_PERSISTENCE_ERROR = -2,
        MQTT_STATUS_DISCONNECTED = -3,
        MQTT_STATUS_MAX_MESSAGES_INFLIGHT = -4,
        MQTT_STATUS_BAD_UTF8_STRING = -5,
        MQTT_STATUS_NULL_PARAMETER = -6,
        MQTT_STATUS_TOPICNAME_TRUNCATED = -7,
        MQTT_STATUS_BAD_STRUCTURE = -8,
        MQTT_STATUS_BAD_QOS = -9,
        MQTT_STATUS_NO_MORE_MSGIDS = -10,
        MQTT_STATUS_OPERATION_INCOMPLETE = -11,
        MQTT_STATUS_MAX_BUFFERED_MESSAGES = -12,
        MQTT_STATUS_SSL_NOT_SUPPORTED = -13,
        MQTT_STATUS_BAD_PROTOCOL = -14,
        MQTT_STATUS_BAD_MQTT_OPTION = -15,
        MQTT_STATUS_WRONG_MQTT_VERSION = -16,
        MQTT_STATUS_0_LEN_WILL_TOPIC = -17
    };

    enum MQTTPropertyCodes
    {
        MQTT_PROPERTY_CODE_PAYLOAD_FORMAT_INDICATOR = 1,            /**< The value is 1 */
        MQTT_PROPERTY_CODE_MESSAGE_EXPIRY_INTERVAL = 2,             /**< The value is 2 */
        MQTT_PROPERTY_CODE_CONTENT_TYPE = 3,                        /**< The value is 3 */
        MQTT_PROPERTY_CODE_RESPONSE_TOPIC = 8,                      /**< The value is 8 */
        MQTT_PROPERTY_CODE_CORRELATION_DATA = 9,                    /**< The value is 9 */
        MQTT_PROPERTY_CODE_SUBSCRIPTION_IDENTIFIER = 11,            /**< The value is 11 */
        MQTT_PROPERTY_CODE_SESSION_EXPIRY_INTERVAL = 17,            /**< The value is 17 */
        MQTT_PROPERTY_CODE_ASSIGNED_CLIENT_IDENTIFER = 18,          /**< The value is 18 */
        MQTT_PROPERTY_CODE_SERVER_KEEP_ALIVE = 19,                  /**< The value is 19 */
        MQTT_PROPERTY_CODE_AUTHENTICATION_METHOD = 21,              /**< The value is 21 */
        MQTT_PROPERTY_CODE_AUTHENTICATION_DATA = 22,                /**< The value is 22 */
        MQTT_PROPERTY_CODE_REQUEST_PROBLEM_INFORMATION = 23,        /**< The value is 23 */
        MQTT_PROPERTY_CODE_WILL_DELAY_INTERVAL = 24,                /**< The value is 24 */
        MQTT_PROPERTY_CODE_REQUEST_RESPONSE_INFORMATION = 25,       /**< The value is 25 */
        MQTT_PROPERTY_CODE_RESPONSE_INFORMATION = 26,               /**< The value is 26 */
        MQTT_PROPERTY_CODE_SERVER_REFERENCE = 28,                   /**< The value is 28 */
        MQTT_PROPERTY_CODE_REASON_STRING = 31,                      /**< The value is 31 */
        MQTT_PROPERTY_CODE_RECEIVE_MAXIMUM = 33,                    /**< The value is 33*/
        MQTT_PROPERTY_CODE_TOPIC_ALIAS_MAXIMUM = 34,                /**< The value is 34 */
        MQTT_PROPERTY_CODE_TOPIC_ALIAS = 35,                        /**< The value is 35 */
        MQTT_PROPERTY_CODE_MAXIMUM_QOS = 36,                        /**< The value is 36 */
        MQTT_PROPERTY_CODE_RETAIN_AVAILABLE = 37,                   /**< The value is 37 */
        MQTT_PROPERTY_CODE_USER_PROPERTY = 38,                      /**< The value is 38 */
        MQTT_PROPERTY_CODE_MAXIMUM_PACKET_SIZE = 39,                /**< The value is 39 */
        MQTT_PROPERTY_CODE_WILDCARD_SUBSCRIPTION_AVAILABLE = 40,    /**< The value is 40 */
        MQTT_PROPERTY_CODE_SUBSCRIPTION_IDENTIFIERS_AVAILABLE = 41, /**< The value is 41 */
        MQTT_PROPERTY_CODE_SHARED_SUBSCRIPTION_AVAILABLE = 42       /**< The value is 241 */
    };

    enum MQTTPropertyTypes
    {
        MQTT_PROPERTY_TYPE_BYTE,
        MQTT_PROPERTY_TYPE_TWO_BYTE_INTEGER,
        MQTT_PROPERTY_TYPE_FOUR_BYTE_INTEGER,
        MQTT_PROPERTY_TYPE_VARIABLE_BYTE_INTEGER,
        MQTT_PROPERTY_TYPE_BINARY_DATA,
        MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING,
        MQTT_PROPERTY_TYPE_UTF_8_STRING_PAIR
    };

    typedef struct
    {
        int len;    /**< the length of the string */
        char *data; /**< pointer to the string data */
    } MQTTLenString;

    typedef struct
    {
        enum MQTTPropertyCodes identifier; /**<  The MQTT V5 property id. A multi-byte integer. */
        /** The value of the property, as a union of the different possible types. */
        union
        {
            unsigned char byte;      /**< holds the value of a byte property type */
            unsigned short integer2; /**< holds the value of a 2 byte integer property type */
            unsigned int integer4;   /**< holds the value of a 4 byte integer property type */
            struct
            {
                MQTTLenString data;  /**< The value of a string property, or the name of a user property. */
                MQTTLenString value; /**< The value of a user property. */
            };
        } value;
    } MQTTProperty;

    typedef struct
    {
        int count;           /**< number of property entries in the array */
        int max_count;       /**< max number of properties that the currently allocated array can store */
        int length;          /**< mbi: byte length of all properties */
        MQTTProperty *array; /**< array of properties */
    } MQTTProperties;

    typedef struct
    {
        char id[4];
        int version;
        int do_openssl_init;
    } MQTTInitOptions;

    typedef struct
    {
        char id[4];
        int version;
        int payloadlen;
        void *payload;
        int qos;
        int retained;
        int dup;
        int msgid;
        MQTTProperties properties;
    } MQTTMessage;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define MQTT_init_options_initializer \
    {                                 \
        {'M', 'Q', 'T', 'G'}, 0, 0    \
    }

#define MQTT_Properties_initializer \
    {                               \
        0, 0, 0, NULL               \
    }

    // class Properties
    // {
    // private:
    //     /* data */
    //     map<MQTTPropertyCodes, MQTTPropertyTypes> _code_type;

    //     void init_code_type()
    //     {
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_PAYLOAD_FORMAT_INDICATOR, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_MESSAGE_EXPIRY_INTERVAL, MQTT_PROPERTY_TYPE_FOUR_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_CONTENT_TYPE, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_RESPONSE_TOPIC, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_CORRELATION_DATA, MQTT_PROPERTY_TYPE_BINARY_DATA));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SUBSCRIPTION_IDENTIFIER, MQTT_PROPERTY_TYPE_VARIABLE_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SESSION_EXPIRY_INTERVAL, MQTT_PROPERTY_TYPE_FOUR_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_ASSIGNED_CLIENT_IDENTIFER, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SERVER_KEEP_ALIVE, MQTT_PROPERTY_TYPE_TWO_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_AUTHENTICATION_METHOD, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_AUTHENTICATION_DATA, MQTT_PROPERTY_TYPE_BINARY_DATA));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_REQUEST_PROBLEM_INFORMATION, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_WILL_DELAY_INTERVAL, MQTT_PROPERTY_TYPE_FOUR_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_REQUEST_RESPONSE_INFORMATION, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_RESPONSE_INFORMATION, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SERVER_REFERENCE, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_REASON_STRING, MQTT_PROPERTY_TYPE_UTF_8_ENCODED_STRING));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_RECEIVE_MAXIMUM, MQTT_PROPERTY_TYPE_TWO_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_TOPIC_ALIAS_MAXIMUM, MQTT_PROPERTY_TYPE_TWO_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_TOPIC_ALIAS, MQTT_PROPERTY_TYPE_TWO_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_MAXIMUM_QOS, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_RETAIN_AVAILABLE, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_USER_PROPERTY, MQTT_PROPERTY_TYPE_UTF_8_STRING_PAIR));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_MAXIMUM_PACKET_SIZE, MQTT_PROPERTY_TYPE_FOUR_BYTE_INTEGER));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_WILDCARD_SUBSCRIPTION_AVAILABLE, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SUBSCRIPTION_IDENTIFIERS_AVAILABLE, MQTT_PROPERTY_TYPE_BYTE));
    //         _code_type.insert(make_pair(MQTT_PROPERTY_CODE_SHARED_SUBSCRIPTION_AVAILABLE, MQTT_PROPERTY_TYPE_BYTE));
    //     }

    // public:
    //     Properties(/* args */)
    //     {
    //         init_code_type();
    //     }

    //     ~Properties()
    //     {
    //         _code_type.clear();
    //     }

    //     int get_type(MQTTPropertyCodes code)
    //     {
    //         if (_code_type.find(code) == _code_type.end())
    //         {
    //             return -1;
    //         }
    //         else
    //         {
    //             return _code_type[code];
    //         }
    //     }
    // };

} // namespace MQTT


typedef mbed::Callback<void(char *, uint32_t, char *, uint32_t)> MQTT_PUBLISH_CB;
typedef mbed::Callback<void(char *, uint32_t, char *, uint32_t)> MQTT_SUBSCRIBE_CB;

#endif
