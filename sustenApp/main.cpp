#include <SoftwareSerial.h>
#include <ArduinoJson.h>

const int TX = 0, RX = 1, HIDROMETRO = 2, SCT = 3;
const int DISPOSTIVO_01 = 3, DISPOSTIVO_02 = 4, DISPOSTIVO_03 = 5;
const int LIMITE = 13, CAPACITY = 96;

const int portasSaida[LIMITE] = {DISPOSTIVO_01};
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
        digitalWrite(portasSaida[i], LOW);
    }
}

void leituraBluetooth() {
    if(bluetooh.available()) {
        while(bluetooh.available()){
            descompactaJSON();

            if(retornaStringJSON("comando") == "controlador") {
                controlaDispositivo(retornaIntJSON("porta"));
            } else {
                bluetooh.println("travou");
            }
        }
    }
}

void descompactaJSON() {
    String request = recebeInformacaoBluetooth();
    
    DeserializationError error = deserializeJson(JSON, request);
    if(error != DeserializationError::Ok) {
        enviaInformacaoBluetooth(error.c_str());
    }
} 

String compactaJSON() {
    String resposta;
    serializeJson(JSON, resposta);

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

String retornaStringJSON(String atributo) {
    return JSON[atributo].as<String>();
}

int retornaIntJSON(String atributo) {
    return JSON[atributo].as<int>();
}

float retornaFloatJSON(String atributo) {
    return JSON[atributo].as<float>();
}

bool retornaBoolJSON(String atributo) {
    return JSON[atributo].as<bool>();
}
