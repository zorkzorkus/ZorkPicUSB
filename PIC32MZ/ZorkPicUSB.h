#ifndef ZORKPICUSB_H
#define ZORKPICUSB_H

#include "ZorkPicUSBConfig.h"
#include <proc/p32mz2048efh064.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef min
#define min(x,y) ((x<y)?x:y)
#endif

#define MAX_CONTROL_LENGTH 512

void USBInit();
void USBInitEndpoints();
void USBService();

void USBTransmit(uint8_t endpoint, uint8_t *data, uint16_t length);
uint16_t USBReceive(uint8_t endpoint, uint8_t *data, uint16_t length);
bool USBHasData(uint8_t endpoint);
bool USBCanTransmit(uint8_t endpoint);

bool USBSetup(SetupPacket* sp);
void USBSendStringDesc(uint8_t index, uint16_t lang);
void USBSendEp0(uint8_t* data, uint32_t len);
void StallEp0();

void delay_us(unsigned int us);

// TODO: as of now, ep 0 and ep 1 are configured in USBInit
//void _USBConfigureEndpoint(uint8_t ep);

#endif