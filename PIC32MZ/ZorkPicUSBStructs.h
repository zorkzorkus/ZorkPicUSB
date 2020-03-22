#ifndef ZORKPICUSBSTRUCTS_H
#define ZORKPICUSBSTRUCTS_H

#include <stdint.h>

// Disable struct-padding
#pragma pack(push, 1)

#define SD_MAX_LEN 32

enum PID {
    PID_OUT = 0x01,
    PID_IN = 0x09,
    PID_SOF = 0x05,
    PID_SETUP = 0x0D,
    PID_DATA0 = 0x03,
    PID_DATA1 = 0x0B,
    PID_DATA2 = 0x07,
    PID_MDATA = 0x0F,
    PID_ACK = 0x02,
    PID_NAK = 0x0A,
    PID_STALL = 0x0E,
    PID_NYET = 0x06,
    PID_PRE = 0x0C,
    PID_ERR = 0x0C,
    PID_SPLIT = 0x08,
    PID_PING = 0x04,
    PID_RESERVED = 0x00,
};

enum DestinationType {
    DEST_DEVICE = 0,
    DEST_INTERFACE = 1,
    DEST_ENDPOINT = 2,
    DEST_OTHER_ELEMENT = 3,
};

enum RequestType {
    REQUEST_TYPE_STANDARD = 0,
    REQUEST_TYPE_CLASS = 1,
    REQUEST_TYPE_VENDOR = 2,
    REQUEST_TYPE_RESERVED = 3,
};

enum StandardControlRequest {
    GET_STATUS = 0x0,
    CLEAR_FEATURE = 0x1,
    SET_FEATURE = 0x3,
    SET_ADDRESS = 0x5,
    GET_DESCRIPTOR = 0x6,
    SET_DESCRIPTOR = 0x7,
    GET_CONFIGURATION = 0x8,
    SET_CONFIGURATION = 0x9,
    GET_INTERFACE = 0xA,
    SET_INTERFACE = 0xB,
    SYNCH_FRAME = 0xC,
};

enum DescriptorTypes {
    DESC_DEVICE = 0x1,
    DESC_CONFIGURATION = 0x2,
    DESC_STRING = 0x3,
    DESC_INTERFACE = 0x4,
    DESC_ENDPOINT = 0x5,
    DESC_DEVICE_QUALIFIER = 0x6,
    DESC_OTHER_SPEED_CONFIGURATION = 0x7,
    DESC_INTERFACE_POWER = 0x8,
    DESC_OTG = 0x9,
    DESC_DEBUG = 0xA,
    DESC_INTERFACE_ASSOCIATION = 0xB,
};

enum DeviceClassCodes {
    DEVICE_CLASS_DEFINED_AT_INTERFACE_LEVEL = 0x0,
    DEVICE_CLASS_MISC = 0xef,
    DEVICE_CLASS_APPLICATION_SPECIFIC = 0xfe,
    DEVICE_CLASS_VENDOR_SPECIFIC = 0xff,
};

enum EndpointAttributes {
    EP_CONTROL = 0x0,
    EP_ISOCHRONOUS = 0x1,
    EP_BULK = 0x2,
    EP_INTERRUPT = 0x3,
};

enum StringDescriptors {
    SD_ZERO,
    SD_MANUFACTURER,
    SD_PRODUCT,
    SD_SERIAL,
    SD_CONFIGURATION,
    SD_INTERFACE
};

typedef struct {

    union {

        struct {
            uint8_t destination : 5; /**< @see enum DestinationType */
            uint8_t type : 2; /**< @see enum RequestType */
            uint8_t direction : 1; /**< 0=out, 1=in */
        };
        uint8_t bmRequestType;
    } REQUEST;
    uint8_t bRequest; /**< Dependent on @p type. @see enum StandardControlRequest */
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SetupPacket;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType; /**< set to DESC_DEVICE */
    uint16_t bcdUSB; /**< Set to 0x0200 for USB 2.0 */
    uint8_t bDeviceClass;
    uint8_t bDeviceSubclass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0; /**< Max packet size for ep 0. Must be 8, 16, 32, or 64. */
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer; /**< index of manufacturer string descriptor */
    uint8_t iProduct; /**< index of product string descriptor */
    uint8_t iSerialNumber; /**< index of serial number string descriptor */
    uint8_t bNumConfigurations;
} DeviceDescriptor;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType; /**< Set to DESC_CONFIGURATION */
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration; /**< index of string descriptor */
    uint8_t bmAttributes;
    uint8_t bMaxPower; /**< one-half the max power required by this device. */
} ConfigurationDescriptor;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType; /**< Set to DESC_INTERFACE */
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubclass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} InterfaceDescriptor;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType; /**< Set to DESC_ENDPOINT */
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} EndpointDescriptor;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType; /**< Set to DESC_STRING */
    uint16_t chars[SD_MAX_LEN];
} StringDescriptor;

typedef struct {
    uint8_t bLength; /**< Set to 8 bytes */
    uint8_t bDescriptorType; /**< Set to DESC_INTERFACE_ASSOCIATION = 0xB */
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction; /**< String Descriptor Index */
} InterfaceAssociationDescriptor;

typedef struct {
    ConfigurationDescriptor configurationDescriptor;
    InterfaceDescriptor interfaceDescriptor;
    EndpointDescriptor endpointIn;
    EndpointDescriptor endpointOut;
} Configuration;

typedef struct {
    uint8_t bLength; /**< set to 0x12 */
    uint8_t bDescriptorType; /**< set to 0x3 */
    uint16_t qwSignature[7]; /**< set to "MSFT100" (Unicode) (no NULL) */
    uint8_t bMS_VendorCode; /**< Set to the bRequest by which the host should ask for the Compat ID and Property descriptors. */
    uint8_t bPad; /**< Set to 0x0 */
} MicrosoftOsDescriptor;

//Re-enable struct-padding
#pragma pack(pop)

#endif