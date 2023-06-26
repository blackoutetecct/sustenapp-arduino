#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>

const int TX = 0, RX = 1, HIDROMETRO = 3, SCT = 4, RESERVATORIO_ECHO = 5, RESERVATORIO_TRIGGER = 6; 
const int DISPOSTIVO_01 = 10, DISPOSTIVO_02 = 11, DISPOSTIVO_03 = 12; 
const int LIMITE = 13, CAPACITY = 96, TENSAO = 110;
double ALTURA_RESERVATORIO = 0, KWH = 0;

const int portasSaida[LIMITE] = {DISPOSTIVO_01};
const int portasEntrada[LIMITE] = {};

SoftwareSerial bluetooh(RX, TX);
Ultrasonic ultrasonic(RESERVATORIO_TRIGGER, RESERVATORIO_ECHO);
EnergyMonitor energia;
StaticJsonDocument<CAPACITY> JSON;

void setup() {
    bluetooh.begin(9600);
    energia.current(SCT, 0.0607);
    declaraPortasDeSaida();
}

void loop() {
    leituraBluetooth();
    leituraEnergia();
    delay(1000);
}

void declaraPortasDeEntrada() {
    for(int i = 0; i < LIMITE; i++) {
        if(!portasEntrada[i]) {
            break;
        }

        pinMode(portasEntrada[i], INPUT);
    }
}

void declaraPortasDeSaida() {
    for(int i = 0; i < LIMITE; i++) {
        if(!portasSaida[i]) {
            break;
        }

        pinMode(portasSaida[i], OUTPUT);
        digitalWrite(portasSaida[i], LOW);
    }
}

// LEITURA CONTINUA 

void leituraBluetooth() {
    if(bluetooh.available()) {
        while(bluetooh.available()){
            descompactaJSON();

            if(retornaStringJSON("comando") == "consumo") {
                if(retornaStringJSON("tipo") == "hidrico") {
                    if(retornaBoolJSON("renovavel")) {
                        enviaRelatorio(retornaVolumeReservatorio());
                    } else{
                        enviaRelatorio(retornaConsumoHidrico());
                    }
                } else if(retornaStringJSON("tipo") == "eletrico") {
                    if(retornaBoolJSON("renovavel")) {
                        enviaRelatorio(retornaVolumePainelSolar());
                    } else{
                        enviaRelatorio(retornaConsumoEletrico());
                    }
                }
            } else if(retornaStringJSON("comando") == "controlador") {
                controlaDispositivo(retornaIntJSON("porta"));
            } else if(retornaStringJSON("comando") == "declaracao") {
                if(retornaStringJSON("tipo") == "reservatorio") {
                    declaraAlturaReservatorio(retornaFloatJSON("altura"));
                }
            } else {
                enviaInformacaoBluetooth("travou");
            }
        }
    }
}

void leituraEnergia() {
    double IRMS = energia.calcIrms(1480); // Calcula o valor da Corrente, usando 1480 amostras

    if(IRMS < 0.02) {
        return;
    } 

    KWH += (IRMS * TENSAO)/1000;
}

// COMUNICACAO

String recebeInformacaoBluetooth() {
    return String(bluetooh.readString());
}

void enviaInformacaoBluetooth(String informacao) {
    bluetooh.println(informacao);
}

// -> CONVERSAO

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

// -> EXTRACAO DE INFORMACAO

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

// CONTROLADOR

void controlaDispositivo(int porta) {
    digitalWrite(porta, !retornaEstadoDispositivo(porta));
}

// RENOVAVEL

double retornaConsumoEletrico() {
    double KWH_RETURN = KWH;
    KWH = 0;
    return KWH_RETURN;
}

double retornaConsumoHidrico() {
    return 0;
}

double retornaVolumePainelSolar() {
    return 0;
}

double retornaVolumeReservatorio() {
    return ALTURA_RESERVATORIO - ultrasonic.read() / 100;
}

// DECLARACAO

void declaraAlturaReservatorio(float altura) {
    ALTURA_RESERVATORIO = altura; // CM
}

// RELATORIO

void enviaRelatorio(double consumo) {
    JSON["consumo"] = consumo;
    enviaInformacaoBluetooth(compactaJSON());
}
