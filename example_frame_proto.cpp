#include "frame_proto.h"

#define MY_NODE_ID 3

// 1. Émission
void send_servo_command(void) {
    FrameMsg_t msg = {
        .dest_id = 3,
        .src_id  = 0,
        .cmd     = FRAME_CMD_WRITE, // d9-d8 = 01
        .value   = 29               // 0x1D
    };

    uint8_t tx[4];
    frame_pack(&msg, tx);

    // Octet 0 : 0xAA
    // Octet 1 : (3 << 5) | (0 << 2) | 0x01 = 0x60 | 0x01 = 0x61
    // Octet 2 : 0x1D (29 en hexadécimal)
    // Octet 3 : (0xAA + 0x61 + 0x1D) & 0xFF = 0x28
    // Trame générée : AA 61 1D 28

    // Transmettre tx sur l'UART (ex: HAL_UART_Transmit, uart_write...)
}

// 2. Réception (ex. dans le callback / interruption UART RX)
FrameParser_t my_parser;

void setup(void) {
    frame_parser_init(&my_parser);
}

void on_uart_rx_char(uint8_t incoming_byte) {
    FrameMsg_t received_msg;
    
    if (frame_parse_byte(&my_parser, incoming_byte, &received_msg)) {
        // Trame valide reçue sans corruption !
        if (received_msg.dest_id == MY_NODE_ID) {
            // Traiter received_msg.cmd / received_msg.value en fonction de received_msg.src_id
        }
    }
}
