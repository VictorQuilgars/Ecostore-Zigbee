# ecostore-hub

Projet créé via go new.

## Arborescence

Le dépôt suit la structure d'un dossier de croquis Arduino :

```
hub/
  hub.ino                     croquis principal
libraries/
  FrameProtocol/              protocole de trame 4 octets
    library.properties
    src/frame_protocol.{h,c}
    examples/FrameProtocolBasic/
  XBeeAT/                     configuration XBee en mode AT
    library.properties
    src/xbee_at.{h,c}
    examples/XBeeSetDestination/
```

## Compilation

Arduino IDE : dans Préférences, régler l'emplacement du carnet de croquis sur la racine du dépôt, puis ouvrir `hub/hub.ino`.

arduino-cli :

```bash
arduino-cli compile --fqbn arduino:avr:uno --libraries libraries hub
```
