#ifndef ZORKPICUSBCONFIG_H
#define ZORKPICUSBCONFIG_H

#include "ZorkPicUSBStructs.h"

#define C_ENDPOINTLENGTH_0 64
#define C_ENDPOINTLENGTH_1 512
#define C_ENDPOINTS 2

static const char c_StrManufacturer[] = "Zork";
static const char c_StrProduct[] = "PIC32MZ USB";
static const char c_StrSerial[] = "00000000";
static const char c_StrConfiguration[] = "Default";
static const char c_StrInterface[] = "LibUSB";

static const DeviceDescriptor c_ThisDeviceDescriptor = {
	sizeof (DeviceDescriptor), // bLength
	DESC_DEVICE, // bDescriptorType
	0x0200, // 0x0200 = USB 2.0, 0x0110 = USB 1.1
	DEVICE_CLASS_VENDOR_SPECIFIC, /* Device class */
	0x00, /* Device Subclass. */
	0x00, /* Protocol. */
	C_ENDPOINTLENGTH_0, // bMaxPacketSize0
	0x04D8, // Vendor
	0xFFFF, // Product
	0x0100, // device release (1.0)
	SD_MANUFACTURER, // Manufacturer
	SD_PRODUCT, // Product
	SD_SERIAL, // Serial
	1 // NumConfigurations
};

static const Configuration c_ThisConfiguration = {
	{
		// Members from struct configuration_descriptor
		sizeof(ConfigurationDescriptor),
		DESC_CONFIGURATION,
		sizeof (c_ThisConfiguration), // wTotalLength (length of the whole packet)
		1, // bNumInterfaces
		1, // bConfigurationValue
		SD_CONFIGURATION, // iConfiguration (index of string descriptor)
		0b10000000,
		100 / 2, // 100/2 indicates 100mA
	}, 
	{
		sizeof(InterfaceDescriptor), //bLength
		DESC_INTERFACE,
		0x0, // InterfaceNumber
		0x0, // AlternateSetting
		0x2, // bNumEndpoints
		0xFF, // bInterfaceClass - Vendor Specific
		0xFF, // bInterfaceSubclass - Vendor Specific
		0xFF, // bInterfaceProtocol
		SD_INTERFACE, // iInterface (index of string describing interface)
	},
	{
		sizeof(EndpointDescriptor),
		DESC_ENDPOINT,
		0x01, //endpoint 2 - 0x00 = OUT
		EP_BULK, // bmAttributes
		C_ENDPOINTLENGTH_1, // wMaxPacketSize
		0, // bInterval in ms.
	},
	{
		sizeof(EndpointDescriptor),
		DESC_ENDPOINT,
		0x81, // endpoint 1 - 0x80 = IN
		EP_BULK, // bmAttributes
		C_ENDPOINTLENGTH_1, // wMaxPacketSize
		0 // bInterval in ms.
	}
};

#endif