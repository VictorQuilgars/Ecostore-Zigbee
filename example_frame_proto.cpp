#include "frame_proto.h"

// 1. Émission
void send_servo_command(void) {
    FrameMsg_t msg = {
        .dest_id = 3,
        .src_id  = 0,
        .cmd     = FRAME_CMD_WRITE, // d8 = 1
        .value   = -90              // d9 = 1, magnitude = 90 (0x5A)
    };

    uint8_t tx[4];
    frame_pack(&msg, tx);

    // Octet 0 : 0xAA
    // Octet 1 : (3 << 5) | (0 << 2) | (1 << 1) | (1) = 0x60 | 0x02 | 0x01 = 0x63
    // Octet 2 : 0x5A (90 en hexadécimal)
    // Octet 3 : (0xAA + 0x63 + 0x5A) & 0xFF = 0x67
    // Trame générée : AA 63 5A 67
    
    // Transmettre tx_buf sur l'UART (ex: HAL_UART_Transmit, uart_write...)
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
            // Traiter received_msg.data en fonction de received_msg.src_id
        }
    }
}
