#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
# error Bluetooth is not enabled!Please run `make menuconfig`to and enable it
#endif

BluetoothSerial bluetooth;

void setup() {
    bluetooth.begin("ESP32"); 
}

void loop() {
    if (bluetooth.available()) {
        String leitura = bluetooth.readString();
        enviaInformacaoBluetooth(leitura);
    }
}

void enviaInformacaoBluetooth(String informacao) {
    String JSON_AUXILIAR = "{" + informacao + "}";
    char RETURN[JSON_AUXILIAR.length()];
    memcpy(RETURN, JSON_AUXILIAR.c_str(), JSON_AUXILIAR.length());
    bluetooth.write((uint8_t * ) RETURN, JSON_AUXILIAR.length());
}