#include "frame_proto.h"
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

    // Détermination du bit de signe (d9) et de la valeur absolue (d0-d7)
    uint8_t sign_bit = (msg->value < 0) ? 1 : 0;
    uint8_t abs_val  = (uint8_t)(abs(msg->value) & 0xFF);
    uint8_t cmd_bit  = (msg->cmd == FRAME_CMD_WRITE) ? 1 : 0;

    // Octet 1 : Dest (7-5) | Src (4-2) | Signe d9 (bit 1) | Cmd d8 (bit 0)
    out_frame[1] = (uint8_t)(((msg->dest_id & 0x07) << 5) |
                            ((msg->src_id  & 0x07) << 2) |
                            ((sign_bit     & 0x01) << 1) |
                            (cmd_bit       & 0x01));

    // Octet 2 : Valeur absolue 8 bits (d7-d0)
    out_frame[2] = abs_val;

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
    
    // Extraction signe et commande
    uint8_t sign_bit = (in_frame[1] >> 1) & 0x01;
    msg->cmd = (FrameCmd_t)(in_frame[1] & 0x01);

    // Reconstitution de la valeur signée
    int16_t magnitude = in_frame[2];
    msg->value = (sign_bit == 1) ? -magnitude : magnitude;

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
