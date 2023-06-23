#include <SoftwareSerial.h>
#include <ArduinoJson.h>

const int LIMITE = 13, LED = 3, RX = 1, TX = 0;
const int portasSaida[LIMITE] = {LED};
const int portasEntrada[LIMITE] = {};

SoftwareSerial bluetooh(RX, TX);
StaticJsonDocument<CAPACITY> JSON;

void setup() {
    bluetooh.begin(9600);
    declaraPortasDeSaida();
}

void loop() {
    leituraBluetooth();
}

void declaraPortasDeEntrada() {
    for(int i = 0; i < LIMITE; i++) {
        if(portasEntrada[i] == NULL) {
            break;
        }

        pinMode(portasEntrada[i], INPUT);
    }
}

void declaraPortasDeSaida() {
    for(int i = 0; i < LIMITE; i++) {
        if(portasSaida[i] == NULL) {
            break;
        }

        pinMode(portasSaida[i], OUTPUT);
    }
}

void leituraBluetooth() {
    if(bluetooh.available()) {
        while(bluetooh.available()){
            if(recebeInformacaoBluetooth() == '1'){
                controlaDispositivo(LED);
            }
        }
    }
}

char* descompactaJSON() {
    DeserializationError error = deserializeJson(JSON, recebeInformacaoBluetooth());
    if(error == NULL) {
        enviaInformacaoBluetooth(error.c_str());
    }

    return deserializeJson(JSON, "{}");
} 

String compactaJSON() {
    String resposta;
    serializeJson(jsonDoc, resposta);

    return resposta;
}

void controlaDispositivo(int porta) {
    if(retornaEstadoDispositivo(porta) == HIGH) {
        digitalWrite(porta, LOW);
    } else {
        digitalWrite(porta, HIGH);
    }
}

String recebeInformacaoBluetooth() {
    return String(bluetooh.readString());
}

void enviaInformacaoBluetooth(String informacao) {
    bluetooh.println(informacao);
}

int retornaEstadoDispositivo(int porta) {
    return digitalRead(porta);
}