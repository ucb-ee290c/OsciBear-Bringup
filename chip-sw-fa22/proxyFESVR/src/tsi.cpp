#include "tsi.h"
#include "../rs232/Rs232.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

Tsi::Tsi(uint8_t z, uint8_t o, uint8_t a, uint8_t w) {
    // For Osci & SCuM-V we expect: 4, 4, 32, 8. For Bearly we expect: 4, 4, 64, 8.
    // The z and o could be 13 though for SCuM-V and Bearly, nobody actually checked...
    Tsi::z = z;
    Tsi::o = o;
    Tsi::a = a;
    Tsi::w = w;
}

/* To understand why, see https://github.com/ucberkeley-ee290c/fa22/tree/main/labs/lab2-tsi-flow */
size_t TsiFpgaUart::bufferBitLength() {
    return 3 + 3 + 3 + z + o + a + 9*((size_t)w) + 1 + 1;
}

size_t TsiFpgaUart::bufferByteLength() {
    size_t bitlen = bufferBitLength();
    return 1u + bitlen/8u + (((bitlen % 8u) == 0u) ? 0u : 1u);
}

/** 
 * Serializes message given paremeters.
 * Reminder: signal orders and all the actual bits are reversed (as in the whole packet is LSBit).
 * UART is LSBit first already, so just simply reverse the assembly order for the fields. 
 * Recommend working in uint8_t arrays so that there are no endian issues. 
 */
int TsiFpgaUart::serialize(TsiPacket packet) {    
    // Set the buffers to 0.
    for (size_t i = 0; i < TsiFpgaUart::bufferByteLength(); i++) {
        writeBuffer[i] = 0u;
    }
    
    packet.rawHeader = TsiMsg_getHeader(packet.type);
    
    size_t bitOffset = 0u;
    // this is the header on top of UART to tell FPGA how big of a TSI packet to expect and transfer, max size 255.
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, bufferBitLength(), 8u);
    bitOffset += 8u;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.corrupt ? 0b11u : 0b01u, 2u);
    bitOffset += 2u;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.mask, w);
    bitOffset += w;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.data, w*8u);
    bitOffset += w*8u;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.addr, a);
    bitOffset += a;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.source, o);
    bitOffset += o;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.size, z);
    bitOffset += z;
    put_uint64_into_buffer((uint8_t*)writeBuffer, bitOffset, packet.rawHeader, TSI_HEADER_LEN);
    bitOffset += TSI_HEADER_LEN;
    size_t bytes = (bitOffset/8u + ((bitOffset % 8u) > 0u ? 1u : 0u));

    if (bytes != bufferByteLength()) {
        throw "Internal Tsi error, assembled packet byte length is not expected";
    }
    if (bufferBitLength() != (bitOffset - 8u)) {
        throw "Internal Tsi error, assembled packet bit length is not expected";
    }
    // Defined arguments: size, source, address, data, mask, corrupt(0), last(1);
    // tsimsg msgtype; uint8_t size; uint8_t source; uint32_t address; uint64_t data; uint8_t mask; 
    /*
    // Pack the buffer in reverse byte order
    // buffer[0][1:0] = {corrupt, last}; buffer[0][6+2-1:2] = mask[6-1:0];
    buffer[0] = loadBits(corrupt ? 0b00000011 : 0b00000001, Tsi::mask, 6u, 2u, 0u);
    // buffer[1][2+0-1:0] = mask[2+6-1:6]; buffer[1][6+2-1:2] = data[6-1:0]
    buffer[1] = loadBits(loadBits(buffer[1], Tsi::mask, 2u, 0u, 6u), Tsi::data, 6u, 2u, 0u);
    for (size_t i = 0; i < 7; i++) {
        // buffer[2+i][8+0-1:0] = data[((i+1)*8+6)-1 : i*8+6]
        buffer[2u+i] = loadBits(buffer[2u+i], Tsi::data, 8u, 0u, i*8u+6u);
    }
    // buffer[9][2+0-1:0] = data[(7*8+6)+2-1 : 7*8+6]; buffer[9][6+2-1:2] = addr[6-1:0]
    buffer[9] = loadBits(loadBits(buffer[9], Tsi::data, 2u, 0u, 7*8u+6u), Tsi::address, 6u, 2u, 0u);
    
    // ====WARNING: LOADS 32 BIT ADDRESS ONLY, DOES NOT WORK FOR 64 bit addr busses====
    for (size_t i = 0; i < 3; i++) {
        // buffer[10+i][8+0-1:0] = addr[((i+1)*8+6)-1 : i*8+6]
        buffer[10u+i] = loadBits(buffer[10u+i], Tsi::address, 8u, 0u, i*8u+6u);
    }
    
    // buffer[13][2+0-1:0] = addr[(3*8+6)+2-1 : 7*8+6]; 
    buffer[13] = loadBits(buffer[14], Tsi::address, 2u, 0u, 3*8u+6u);
    // buffer[13][4+2-1:2] = source[4-1:0]
    buffer[13] = loadBits(buffer[14], Tsi::source, 4u, 2u, 0u);
    // buffer[13][2+6-1:6] = size[2-1:0]
    buffer[13] = loadBits(buffer[14], Tsi::size, 2u, 2u, 0u);

    // buffer[14][2+0-1:0] = size[2+2-1:2]
    buffer[14] = loadBits(buffer[15], Tsi::size, 2u, 0u, 2u);
    // buffer[14][6+2-1:2] = header[6-1:0]
    buffer[14] = loadBits(buffer[15], header, 6u, 2u, 0u);

    // buffer[15][3+0-1:0] = header[3+6-1:6]
    buffer[15] = loadBits(buffer[15], header, 3u, 0u, 6u);
    */
    writeDriver(writeBuffer, bytes); 
    return 0;
}

/* Deserialize message and return parameter. 
 * Edge cases (esp if chip can initiate requests): 
 * 1. multiple messages recieved at once will be directed to an internal FIFO class buffer. 
 * 2. recieves a message that is incomplete, in this case, poll again until complete message is recieved. 
 * 3. if both cases above occurs, buffer excess processed packets internally, and buffer incomplete packet.
 * For now, expect fesvr to call serdes only sequentially, meaning only one request may be in flight. 
 * Repeatedly poll until message is decoded, which freezes everything, as TL should behave... 
 */
TsiPacket TsiFpgaUart::deserialize() {
    TsiPacket packet;
    
    size_t rxBytes = 0u;
    size_t expBytes = bufferByteLength();
    while (rxBytes < expBytes) {
        pollDriver(pollBuffer+rxBytes, expBytes); 
    }

    size_t bitOffset = 0u;
    // this is the header on top of UART to tell FPGA how big of a TSI packet to expect and transfer, max size 255.
    size_t expectedBits = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, 8u);
    if (expectedBits != bufferBitLength()) {
        throw "Internal Tsi error, got unexpected packet header length";
    }
    bitOffset += 8u;

    packet.corrupt = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, 2u) != 0b01u;
    bitOffset += 2u;

    packet.mask = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, w);
    bitOffset += w;
    packet.data = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, w*8u);
    bitOffset += w*8u;
    packet.addr = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, a);
    bitOffset += a;
    packet.source = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, o);
    bitOffset += o;
    packet.size = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, z);
    bitOffset += z;
    packet.rawHeader = get_uint64_from_buffer((uint8_t*)pollBuffer, bitOffset, TSI_HEADER_LEN);
    bitOffset += TSI_HEADER_LEN;

    if (expectedBits != (bitOffset - 8u)) {
        throw "Internal Tsi error, got unexpected packet length";
    }

    packet.type = TsiMsg_getType(packet.rawHeader);
    return packet;
}

int TsiFpgaUart::initDriver(unsigned comport) {
    char mode[] = {'8', 'N', '1', 0};
    if (rs232::openComport(comport, TSI_UART_BDRATE, mode, 0)) {
        printf("Can not open comport %i\n", comport);
        throw "Cannot open comport";
        return -1;
    }
    TsiFpgaUart::comport = comport;
    return 0;
}

int TsiFpgaUart::pollDriver(unsigned char *buffer, size_t len) {
    return rs232::pollComport(comport, buffer, len);
}

int TsiFpgaUart::writeDriver(char *buffer, size_t len) {
    // WARNING: THIS WILL NOT WORK! IT STOPS AT NULL, WE WANT IT TO STOP AT A CERTAIN LENGTH.
    return rs232::sendBuf(comport, buffer, len);
}

// Helper functions generally useful for TSI
uint16_t TsiMsg_getHeader(TsiMsg type) {
    uint16_t header;
    switch(type) {
        case Get:
            header = TSI_HEADER_GET;
            break;
        case AccessAckData:
            header = TSI_HEADER_ACCESSACKDATA;
            break;
        case PutFullData:
            header = TSI_HEADER_PUTFULLDATA;
            break;
        case PutPartialData:
            header = TSI_HEADER_PUTPARTIALDATA;
            break;
        case AccessAck:
            header = TSI_HEADER_ACCESSACK;
            break;
        type = Unknown;
            throw "Cannot get header of TsiMsg: Unknown";
    }
    return header;
}

TsiMsg TsiMsg_getType(uint16_t header) {
    TsiMsg type;
    switch(header) {
        case TSI_HEADER_GET:
            type = Get;
            break;
        case TSI_HEADER_ACCESSACKDATA:
            type = AccessAckData;
            break;
        case TSI_HEADER_PUTFULLDATA:
            type = PutFullData;
            break;
        case TSI_HEADER_PUTPARTIALDATA:
            type = PutPartialData;
            break;
        case TSI_HEADER_ACCESSACK:
            type = AccessAck;
            break;
        type = Unknown;
    }
    return type;
}

/* Puts an uint64_t into an uint8_t buffer. */
void put_uint64_into_buffer(uint8_t *buffer, size_t bitOffset, uint64_t data, size_t bits) {
    size_t data_cnt = 0u;
    while (data_cnt < bits) {
        size_t buffer_i = (bitOffset+data_cnt) % 8;
        size_t buffer_j = (bitOffset+data_cnt) - buffer_i*8;
        size_t step = MIN(bits - data_cnt, 8u - buffer_j);
        // buffer[buffer_i][step+buffer_j-1:buffer_j] = data[step+data_cnt-1:data_cnt]
        loadBits(buffer[buffer_i], data, step, buffer_j, data_cnt);
        data_cnt += step;
    }
}

/* Gets an uint64_t from an uint8_t buffer. */
uint64_t get_uint64_from_buffer(uint8_t *buffer, size_t bitOffset, size_t bits) {
    uint64_t data;
    size_t data_cnt = 0u;
    while (data_cnt < bits) {
        size_t buffer_i = (bitOffset+data_cnt) % 8;
        size_t buffer_j = (bitOffset+data_cnt) - buffer_i*8;
        size_t step = MIN(bits - data_cnt, 8u - buffer_j);
        // data[step+data_cnt-1:data_cnt] = buffer[buffer_i][step+buffer_j-1:buffer_j]
        loadBits(data, buffer[buffer_i], step, data_cnt, buffer_j);
        data_cnt += step;
    }
    return data;
}

bool TsiPacket_isValidMsg(TsiPacket packet) {

}


// Get uint64_t bitmask of result[N+M:M]
uint64_t getMask(size_t n, size_t m) {
    return (n < 63u) ? ((1u << n) - 1) << m : 0xFFu << m;
}

/* load bits: dest[N+M-1:M] = src[N+K-1:K]
 * assumes bits in dest are fully zeroed so this uses or to set
 */
uint8_t loadBits(uint8_t dest, uint64_t src, size_t n, size_t m, size_t k) {
    dest |= (src & getMask(n, k)) >> k << m;
}

// TSI serializes LSB first and reverses all the fields, this might come in handy...?
uint8_t reverseBits(uint8_t in) {
    uint8_t out = 0u;
    for (int i = 0; i < 8; i++) {
        out |= (in & 1ul) << (7-i); 
        in = in >> 1;
    }
    return out;
}