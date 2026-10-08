#include "frame_protocol.h"
#include <stdlib.h>

static uint8_t compute_checksum(const uint8_t *data, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

void frame_parser_init(FrameParser_t *parser) {
    if (!parser) return;
    parser->state = PARSER_STATE_WAIT_START;
}

void frame_pack(const FrameMsg_t *msg, uint8_t out_frame[FRAME_TOTAL_SIZE]) {
    out_frame[0] = FRAME_START_BYTE;

    uint8_t cmd_bit = 0;
    switch (msg->cmd) {
        case FRAME_CMD_WRITE:
            cmd_bit = 1; // d9 = 0 et d8 = 1 pour WRITE
            break;
        case FRAME_CMD_READ:
            cmd_bit = 0; // d9 = 0 et d8 = 0 pour READ
            break;
        case FRAME_CMD_ERROR:
            cmd_bit = 3; // d9 = 1 et d8 = 1 pour ERROR
            break;
    }

    // Octet 1 : Dest (7-5) | Src (4-2) | Cmd d9-d8 (bits 1-0)
    out_frame[1] = (uint8_t)(((msg->dest_id & 0x07) << 5) |
                            ((msg->src_id  & 0x07) << 2) |
                            (cmd_bit       & 0x03));

    // Octet 2 : Valeur non signée 8 bits (d7-d0)
    out_frame[2] = msg->value;

    // Octet 3 : Checksum sur les 3 premiers octets
    out_frame[3] = compute_checksum(out_frame, 3);
}

bool frame_unpack(const uint8_t in_frame[FRAME_TOTAL_SIZE], FrameMsg_t *msg) {
    if (in_frame[0] != FRAME_START_BYTE) {
        return false;
    }

    uint8_t expected_checksum = compute_checksum(in_frame, 3);
    if (in_frame[3] != expected_checksum) {
        return false;
    }

    msg->dest_id = (in_frame[1] >> 5) & 0x07;
    msg->src_id  = (in_frame[1] >> 2) & 0x07;
    
    // Extraction commande (d9-d8), la valeur 0b10 n'est pas définie
    switch (in_frame[1] & 0x03) {
        case FRAME_CMD_READ:
            msg->cmd = FRAME_CMD_READ;
            break;
        case FRAME_CMD_WRITE:
            msg->cmd = FRAME_CMD_WRITE;
            break;
        case FRAME_CMD_ERROR:
            msg->cmd = FRAME_CMD_ERROR;
            break;
        default:
            return false;
    }

    msg->value = in_frame[2];

    return true;
}

bool frame_parse_byte(FrameParser_t *parser, uint8_t byte, FrameMsg_t *out_msg) {
    switch (parser->state) {
        case PARSER_STATE_WAIT_START:
            if (byte == FRAME_START_BYTE) {
                parser->raw_buffer[0] = byte;
                parser->state = PARSER_STATE_HEADER;
            }
            break;

        case PARSER_STATE_HEADER:
            parser->raw_buffer[1] = byte;
            parser->state = PARSER_STATE_PAYLOAD;
            break;

        case PARSER_STATE_PAYLOAD:
            parser->raw_buffer[2] = byte;
            parser->state = PARSER_STATE_CHECKSUM;
            break;

        case PARSER_STATE_CHECKSUM:
            parser->raw_buffer[3] = byte;
            parser->state = PARSER_STATE_WAIT_START;
            return frame_unpack(parser->raw_buffer, out_msg);

        default:
            parser->state = PARSER_STATE_WAIT_START;
            break;
    }
    return false;
}
