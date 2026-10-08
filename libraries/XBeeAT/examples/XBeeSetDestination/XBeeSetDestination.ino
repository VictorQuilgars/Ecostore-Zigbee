#include <Arduino.h>
#include <xbee_at.h>

// Implémentation des callbacks UART pour Arduino
void xbee_uart_write(const uint8_t *data, uint16_t len) {
    Serial1.write(data, len);
}

bool xbee_wait_str(const char *expected, uint32_t timeout_ms) {
    uint32_t start = millis();
    String resp = "";
    while (millis() - start < timeout_ms) {
        while (Serial1.available()) {
            resp += (char)Serial1.read();
            if (resp.indexOf(expected) >= 0) return true;
        }
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600); // UART vers XBee

    xbee_driver_t driver = {
        .write = xbee_uart_write,
        .wait_response = xbee_wait_str,
        .delay_ms = delay
    };
    xbee_init(&driver);

    // Changer la cible vers le servomoteur (ex: 0x0013A200 / 0x40A1B2C3)
    if (xbee_enter_command_mode()) {
        xbee_set_destination_64(0x0013A200, 0x40A1B2C3);
        xbee_exit_command_mode();
        Serial.println("XBee reconfigure vers le servomoteur.");
    }
}

void loop() {
    // Envoi des trames personnalisées...
}
