#ifndef XBEE_AT_H
#define XBEE_AT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types de pointeurs de fonction pour l'UART (agnostique du hardware)
typedef void (*xbee_uart_write_fn)(const uint8_t *data, uint16_t len);
typedef bool (*xbee_wait_str_fn)(const char *expected, uint32_t timeout_ms);
typedef void (*xbee_delay_ms_fn)(uint32_t ms);

typedef struct {
    xbee_uart_write_fn write;
    xbee_wait_str_fn   wait_response;
    xbee_delay_ms_fn   delay_ms;
} xbee_driver_t;

// Initialisation du driver
void xbee_init(const xbee_driver_t *driver);

// Entre en mode commande (envoi de "+++")
bool xbee_enter_command_mode(void);

// Sort du mode commande ("ATCN")
bool xbee_exit_command_mode(void);

// Modifie l'adresse unicast 64 bits de destination (DH et DL)
// Note : applique en RAM via ATAC sans user la flash.
bool xbee_set_destination_64(uint32_t dh, uint32_t dl);

// Raccourci pour basculer la cible en broadcast (DH=0x0, DL=0xFFFF)
bool xbee_set_destination_broadcast(void);

#ifdef __cplusplus
}
#endif

#endif // XBEE_AT_H
