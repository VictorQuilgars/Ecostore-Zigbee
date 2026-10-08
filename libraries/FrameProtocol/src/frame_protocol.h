#ifndef FRAME_PROTOCOL_H
#define FRAME_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FRAME_START_BYTE 0xAA
#define FRAME_TOTAL_SIZE 4

typedef enum {
    FRAME_CMD_READ  = 0x0,
    FRAME_CMD_WRITE = 0x1,
    FRAME_CMD_ERROR = 0x3
} FrameCmd_t;

typedef struct {
    uint8_t dest_id;    // 3 bits (0 à 7)
    uint8_t src_id;     // 3 bits (0 à 7)
    FrameCmd_t cmd;     // 2 bits : FRAME_CMD_READ, FRAME_CMD_WRITE ou FRAME_CMD_ERROR (d9-d8)
    uint8_t value;      // 8 bits non signés (0 à 255, d7-d0)
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

#ifdef __cplusplus
}
#endif

#endif // FRAME_PROTOCOL_H
