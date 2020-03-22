#include "ZorkPicUSB.h"

uint8_t m_USBBufferRx0[C_ENDPOINTLENGTH_0];
uint8_t m_USBBufferRx1[C_ENDPOINTLENGTH_1];
uint32_t m_UsbBufferRxLen[C_ENDPOINTS];

uint8_t m_SetAddr = 0;

StringDescriptor m_StringDesc;

// If length is nonzero then there is still a control transfer pending
uint8_t m_ControlBuffer[MAX_CONTROL_LENGTH];
uint16_t m_ControlLength;
uint16_t m_ControlIndex;

void USBInit() {

    USBOTGbits.BDEV = 0;
    USBCSR0 = 0;
    USBCSR1 = 0;
    USBCSR2 = 0;
    USBCSR3 = 0;
    USBE0CSR0 = 0;
    USBE0CSR2 = 0;
    USBE0CSR3 = 0;
    USBCRCON = 0;

    USBInitEndpoints();

    // USB MODULE
    USBCRCONbits.USBIE = 0;
    USBCSR0bits.HSEN = 1; // High speed negotiation
    USBCSR0bits.FUNC = 0; // Address 0
    USBCSR0bits.SOFTCONN = 1; // D+/D- active

}

void USBInitEndpoints() {

    // TODO: endpoint length is not linked to config 

    // EP 0
    USBE0CSR0bits.TXMAXP = C_ENDPOINTLENGTH_0;
    USBE0CSR2bits.SPEED = 0b01;
    USBE0CSR0bits.FLUSH = 1;

    // EP 1
    USBCSR3bits.ENDPOINT = 1;

    // TX 1
    USBIENCSR0bits.TXMAXP = C_ENDPOINTLENGTH_1;
    USBIENCSR3bits.TXFIFOSZ = 0b1001;
    USBFIFOAbits.TXFIFOAD = 0x0080;
    USBIENCSR2bits.PROTOCOL = 0b10;
    USBIENCSR2bits.TEP = 0x01;
    USBIENCSR0bits.FLUSH = 1;
    USBOTGbits.TXFIFOSZ = 0b0110;

    // RX 1
    USBIENCSR1bits.RXMAXP = C_ENDPOINTLENGTH_1;
    USBIENCSR3bits.RXFIFOSZ = 0b1001;
    USBFIFOAbits.RXFIFOAD = 0x0280;
    USBIENCSR3bits.PROTOCOL = 0b10;
    USBIENCSR3bits.TEP = 0x01;
    USBIENCSR1bits.FLUSH = 1;
    USBOTGbits.RXFIFOSZ = 0b0110;
}

void USBService() {

    uint32_t usbcsr0_mem = *((uint32_t*) (0xBF8E3000));
    __USBCSR0bits_t* usbcsr0 = (__USBCSR0bits_t*) & usbcsr0_mem;

    if (m_SetAddr) {
        USBCSR0bits.FUNC = m_SetAddr;
        m_SetAddr = 0;
    }

    if (USBCSR2bits.RESETIF) {
        USBCSR0bits.FUNC = 0;
        USBInitEndpoints();
    }

    if (!USBE0CSR0bits.TXRDY && m_ControlLength) {
        uint16_t transmitLength = min(m_ControlIndex + C_ENDPOINTLENGTH_0, m_ControlLength);

        volatile uint8_t* ep0Fifo = (uint8_t*) (0xBF8E3020);
        for (; m_ControlIndex < transmitLength; ++m_ControlIndex) {
            *ep0Fifo = m_ControlBuffer[m_ControlIndex];
        }
        if (m_ControlIndex == m_ControlLength) {
            USBIE0CSR0bits.DATAEND = 1; // End of Data Control bit (Device mode) 
            m_ControlIndex = 0;
            m_ControlLength = 0;
        }
        USBE0CSR0bits.TXRDY = 1;
    }

    if (!USBE0CSR0bits.TXRDY && usbcsr0->EP0IF) {

        if (USBE0CSR0bits.RXRDY) {
            
            volatile uint8_t* ep0Fifo = (uint8_t*) (0xBF8E3020);
            m_UsbBufferRxLen[0] = USBE0CSR2bits.RXCNT;
            for (uint32_t i = 0; i < m_UsbBufferRxLen[0]; ++i) {
                m_USBBufferRx0[i] = *(ep0Fifo + (i & 3));
            }
            USBE0CSR0bits.RXRDYC = 1;

            delay_us(100);
            
            SetupPacket* sp = (SetupPacket*) (m_USBBufferRx0);
            if (sp->REQUEST.type == REQUEST_TYPE_STANDARD) {
                USBSetup(sp);
            }

        }

    }

    if (USBIE0CSR0bits.SETEND) {
        USBIE0CSR0bits.SETENDC = 1;
    }

}

void USBTransmit(uint8_t endpoint, uint8_t* data, uint16_t length) {
    if (endpoint < 1 || endpoint > 7) return;
    USBCSR3bits.ENDPOINT = endpoint;
    if (USBIENCSR0bits.TXPKTRDY) return;
    volatile uint8_t* usbFifo = (uint8_t*) (0xBF8E3020 + endpoint * 4);
    for (uint16_t i = 0; i < length; ++i) {
        *usbFifo = data[i];
    }
    USBIENCSR0bits.TXPKTRDY = 1;
}

uint16_t USBReceive(uint8_t endpoint, uint8_t* data, uint16_t length) {
    if (endpoint < 1 || endpoint > 7) return 0;
    USBCSR3bits.ENDPOINT = endpoint;
    if (!USBIENCSR1bits.RXPKTRDY) return 0;
    uint16_t received = 0;
    received = USBIENCSR2bits.RXCNT;
    volatile uint8_t* usbFifo = (uint8_t*) (0xBF8E3020 + endpoint * 4);
    for (uint16_t i = 0; i < received; ++i) {
        data[i] = *(usbFifo + (i & 3));
    }
    USBIENCSR1bits.RXPKTRDY = 0;
    return received;
}

bool USBHasData(uint8_t endpoint) {
    USBCSR3bits.ENDPOINT = endpoint;
    return (USBIENCSR1bits.RXPKTRDY) ? true : false;
}

bool USBCanTransmit(uint8_t endpoint) {
    USBCSR3bits.ENDPOINT = endpoint;
    return (USBIENCSR0bits.TXPKTRDY) ? false : true;
}

bool USBSetup(SetupPacket* sp) {

    uint8_t descType = (sp->wValue >> 8);
    uint8_t descIndx = (sp->wValue & 0xFF);

    switch (sp->bRequest) {
        case GET_DESCRIPTOR:

            switch (descType) {
                case DESC_DEVICE:
                    USBSendEp0((uint8_t*) & c_ThisDeviceDescriptor, min(sizeof (c_ThisDeviceDescriptor), sp->wLength));
                    break;
                case DESC_CONFIGURATION:
                    USBSendEp0((uint8_t*) & c_ThisConfiguration, min(sizeof (c_ThisConfiguration), sp->wLength));
                    break;
                case DESC_STRING:
                    USBSendStringDesc(descIndx, sp->wIndex);
                    break;
                default:
                    StallEp0();
                    break;
            }
            break;
        case SET_ADDRESS:
            m_SetAddr = sp->wValue;
            break;
        case SET_CONFIGURATION:
            USBSendEp0(0, 0);
        default:
            StallEp0();
            break;
    }

    return false;

}

void USBSendStringDesc(uint8_t index, uint16_t lang) {

    const char* strSrc;
    m_StringDesc.bDescriptorType = DESC_STRING;
    switch (index) {

        case SD_ZERO:
            m_StringDesc.bLength = 4;
            m_StringDesc.chars[0] = 0x0409;
            USBSendEp0((uint8_t*) & m_StringDesc, m_StringDesc.bLength);
            return;
        case SD_MANUFACTURER:
            strSrc = c_StrManufacturer;
            break;
        case SD_PRODUCT:
            strSrc = c_StrProduct;
            break;
        case SD_SERIAL:
            strSrc = c_StrSerial;
            break;
        case SD_CONFIGURATION:
            strSrc = c_StrConfiguration;
            break;
        case SD_INTERFACE:
            strSrc = c_StrInterface;
            break;
        case 0xee:
            // TODO: Microsoft Descriptor 
            StallEp0();
            return;
        default:
            StallEp0();
            return;
    }

    if (lang != 0x0409 && lang != 0x0) {
        StallEp0();
        return;
    }

    uint8_t len;

    for (len = 0; len < SD_MAX_LEN; ++len) {
        if (!strSrc[len]) break;
        m_StringDesc.chars[len] = (uint16_t) strSrc[len];
    }

    ++len;

    m_StringDesc.bLength = len * 2; // numberOfUnicodeChars + lengthByte + descType == strLen + zeroByte

    USBSendEp0((uint8_t*) & m_StringDesc, m_StringDesc.bLength);

}

void USBSendEp0(uint8_t* data, uint32_t len) {

    memcpy(m_ControlBuffer, data, len);
    m_ControlLength = len;
    return;

}

void StallEp0() {
    USBE0CSR0bits.STALL = 1;
}

void delay_us(unsigned int us) {
#define SYS_FREQ 240000000UL
    // Convert microseconds us into how many clock ticks it will take
    us *= SYS_FREQ / 1000000 / 2; // Core Timer updates every 2 ticks
    __builtin_mtc0(9, 0, 0);
    while (us > __builtin_mfc0(9,0)); // Wait until Core Timer count reaches the number we calculated earlier
}