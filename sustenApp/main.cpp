#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include <EmonLib.h>

const int TX = 0, RX = 1, HIDROMETRO = 3, SCT = A0, RESERVATORIO_ECHO = 5, RESERVATORIO_TRIGGER = 6;
const int LIMITE = 13, CAPACITY = 96, TENSAO = 110;

double ALTURA_RESERVATORIO = 0, CAPACIDADE_RESERVATORIO = 0, KWH = 0;

// AGUA

const float FATOR_CALIBRACAO = 7.5;
const int INTERRUPCAO_SENSOR = 0;

double CONTADOR_AGUA = 0, VOLUME_TOTAL = 0;
unsigned long ULTIMA_EXECUCAO = 0;

// -----

int portasSaida[LIMITE] = {};
int portasEntrada[LIMITE] = {};
int portasDispositivo[4] = {10, 11, 12, 13};

SoftwareSerial bluetooth(RX, TX);
Ultrasonic ultrasonic(RESERVATORIO_TRIGGER, RESERVATORIO_ECHO);
EnergyMonitor energia;
StaticJsonDocument <CAPACITY> JSON;

void setup() {
    declaraPortasDeSaida();
    declaraPortasDeEntrada();
    pinMode(HIDROMETRO, INPUT_PULLUP);
    bluetooth.begin(9600);
    energia.current(SCT, 60); 
}

void loop() {
    leituraBluetooth();
    leituraEletrica();
    leituraHidrica();
}

// LEITURA CONTINUA 

void leituraBluetooth() {
    if (bluetooth.available()) {
        while (bluetooth.available()) {
            descompactaJSON();

            if (retornaStringJSON("comando") == "consumo") {
                if (retornaStringJSON("tipo") == "hidrico") {
                    if (retornaBoolJSON("renovavel")) {
                        enviaRelatorio(retornaVolumeReservatorio(), "L");
                    } else {
                        enviaRelatorio(retornaConsumoHidrico(), "M3");
                    }
                } else if (retornaStringJSON("tipo") == "eletrico") {
                    if (retornaBoolJSON("renovavel")) {
                        enviaRelatorio(retornaVolumePainelSolar(), "NA");
                    } else {
                        enviaRelatorio(retornaConsumoEletrico(), "KW/H");
                    }
                }
            } else if (retornaStringJSON("comando") == "controlador") {
                controlaDispositivo(retornaIntJSON("porta"));
            } else if (retornaStringJSON("comando") == "dispositivo") {
                JSON["estado"] = retornaEstadoDispositivo(retornaIntJSON("porta"));
                enviaInformacaoBluetooth(compactaJSON());
            } else if (retornaStringJSON("comando") == "declaracao") {
                if (retornaStringJSON("tipo") == "reservatorio") {
                    declaraReservatorio(retornaDoubleJSON("capacidade"));
                } else if (retornaStringJSON("tipo") == "dispositivo") {
                    declaracaoDispositivo(retornaIntJSON("porta"));
                }
            } else {
                enviaInformacaoBluetooth("travou");
            }
        }
    }
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480); 

    if (IRMS < 0.02) {
        return;
    }

    KWH += (IRMS * TENSAO) / 1000;
}

void leituraHidrica() {
    unsigned long INTERVALO = millis() - ULTIMA_EXECUCAO;

    if (INTERVALO > 1000) {
        detachInterrupt(INTERRUPCAO_SENSOR);

        double FLUXO = ((1000.0 / INTERVALO) * CONTADOR_AGUA) / FATOR_CALIBRACAO;
        double VOLUME = FLUXO / 60;

        VOLUME_TOTAL += VOLUME / 100; //(VOLUME / 1000);
        CONTADOR_AGUA = 0;
        ULTIMA_EXECUCAO = millis();

        attachInterrupt(INTERRUPCAO_SENSOR, incrementaContador, FALLING);
    }
}

// COMUNICACAO

String recebeInformacaoBluetooth() {
    return String(bluetooth.readString());
}

void enviaInformacaoBluetooth(String informacao) {
    bluetooth.println(informacao);
}

// -> CONVERSAO

void descompactaJSON() {
    String request = recebeInformacaoBluetooth();

    DeserializationError error = deserializeJson(JSON, request);
    if (error != DeserializationError::Ok) {
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
    return JSON[atributo].as < int > ();
}

double retornaDoubleJSON(String atributo) {
    return JSON[atributo].as<double>();
}

bool retornaBoolJSON(String atributo) {
    return JSON[atributo].as<bool>();
}

// CONTROLADOR

void controlaDispositivo(int porta) {
    digitalWrite(porta, !retornaEstadoDispositivo(porta));
}

// INCREMENTADOR

void incrementaContador() {
    CONTADOR_AGUA++;
}

// RENOVAVEL

double retornaConsumoEletrico() {
    double KWH_RETURN = KWH;
    KWH = 0;
    return KWH_RETURN;
}

double retornaConsumoHidrico() {
    double VOLUME_TOTAL_RETURN = VOLUME_TOTAL;
    VOLUME_TOTAL = 0;
    return VOLUME_TOTAL_RETURN;
}

double retornaVolumePainelSolar() {
    return 0;
}

double retornaVolumeReservatorio() {
    return ((CAPACIDADE_RESERVATORIO / ALTURA_RESERVATORIO) * (ultrasonic.read() * 100) / ALTURA_RESERVATORIO);
}

// DECLARACAO

void declaraPortasDeEntrada() {
    for (int i = 0; i < LIMITE; i++) {
        if (!portasEntrada[i]) {
            break;
        }

        declaraPorta(portasEntrada[i], INPUT);
    }
}

void declaraPortasDeSaida() {
    for (int i = 0; i < LIMITE; i++) {
        if (!portasSaida[i]) {
            break;
        }

        declaraPorta(portasSaida[i], OUTPUT);
    }
}

void declaraReservatorio(double capacidade) {
    CAPACIDADE_RESERVATORIO = capacidade; // L
    ALTURA_RESERVATORIO = ultrasonic.read(); // CM
}

void declaracaoDispositivo(int porta) {
    for (int i = 0; i < LIMITE; i++) {
        if (!portasSaida[i]) {
            portasSaida[i] = porta;
            declaraPorta(porta, OUTPUT);
        }
    }
}

void declaraPorta(int porta, int fluxo) {
    pinMode(porta, fluxo);

    if (fluxo == OUTPUT) {
        digitalWrite(porta, LOW);
    }
}

// RELATORIO

void enviaRelatorio(double consumo, String unidade) {
    JSON["consumo"] = consumo;
    JSON["unidade"] = unidade;
    enviaInformacaoBluetooth(compactaJSON());
} 