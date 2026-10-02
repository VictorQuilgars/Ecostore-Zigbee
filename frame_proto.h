#ifndef FRAME_PROTO_H
#define FRAME_PROTO_H

#include <stdint.h>
#include <stdbool.h>

#define FRAME_START_BYTE 0xAA
#define FRAME_TOTAL_SIZE 4

typedef enum {
    FRAME_CMD_READ  = 0,
    FRAME_CMD_WRITE = 1
} FrameCmd_t;

typedef struct {
    uint8_t dest_id;    // 3 bits (0 à 7)
    uint8_t src_id;     // 3 bits (0 à 7)
    FrameCmd_t cmd;     // 1 bit : FRAME_CMD_READ ou FRAME_CMD_WRITE (d8)
    int16_t value;      // Plage : -255 à +255 (d9 pour signe, d0-d7 pour magnitude)
} FrameMsg_t;

typedef enum {
    PARSER_STATE_WAIT_START,
    PARSER_STATE_HEADER,
    PARSER_STATE_PAYLOAD,
    PARSER_STATE_CHECKSUM
} ParserState_t;

typedef struct {
    ParserState_t state;
    uint8_t raw_buffer[FRAME_TOTAL_SIZE];
} FrameParser_t;

void frame_parser_init(FrameParser_t *parser);
void frame_pack(const FrameMsg_t *msg, uint8_t out_frame[FRAME_TOTAL_SIZE]);
bool frame_unpack(const uint8_t in_frame[FRAME_TOTAL_SIZE], FrameMsg_t *msg);
bool frame_parse_byte(FrameParser_t *parser, uint8_t byte, FrameMsg_t *out_msg);

#endif // FRAME_PROTO_H
