#include "xbee_at.h"
#include <stdio.h>
#include <string.h>

static xbee_driver_t g_driver;

void xbee_init(const xbee_driver_t *driver) {
    g_driver = *driver;
}

static bool send_at_command(const char *cmd) {
    g_driver.write((const uint8_t *)cmd, (uint16_t)strlen(cmd));
    return g_driver.wait_response("OK\r", 1000);
}

bool xbee_enter_command_mode(void) {
    // Temps de garde initial (1 s de silence requis par défaut)
    g_driver.delay_ms(1100);

    const char *guard = "+++";
    g_driver.write((const uint8_t *)guard, 3);

    // Attente du "OK\r" avec temps de garde final
    bool ok = g_driver.wait_response("OK\r", 1500);
    g_driver.delay_ms(1100);
    return ok;
}

bool xbee_exit_command_mode(void) {
    return send_at_command("ATCN\r");
}

bool xbee_set_destination_64(uint32_t dh, uint32_t dl) {
    char cmd[32];

    // 1. Configurer DH
    snprintf(cmd, sizeof(cmd), "ATDH %08lX\r", (unsigned long)dh);
    if (!send_at_command(cmd)) return false;

    // 2. Configurer DL
    snprintf(cmd, sizeof(cmd), "ATDL %08lX\r", (unsigned long)dl);
    if (!send_at_command(cmd)) return false;

    // 3. Appliquer immédiatement en RAM
    if (!send_at_command("ATAC\r")) return false;

    return true;
}

bool xbee_set_destination_broadcast(void) {
    return xbee_set_destination_64(0x00000000, 0x0000FFFF);
}
