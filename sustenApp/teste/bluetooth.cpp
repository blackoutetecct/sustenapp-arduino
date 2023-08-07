#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
# error Bluetooth is not enabled!Please run `make menuconfig`to and enable it
#endif

BluetoothSerial bluetooth;

void setup() {
    bluetooth.begin("ESP32"); //Bluetooth device name
}

void loop() {
    char leitura;

    if (bluetooth.available()) {
        leitura = bluetooth.read();
        bluetooth.write(leitura);
    }
}