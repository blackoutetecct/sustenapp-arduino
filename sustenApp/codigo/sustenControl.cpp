#include "BluetoothSerial.h"
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>

/* BLUETOOTH */

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled!Please run `make menuconfig`to and enable it
#endif

/* SERIAL */

HardwareSerial serial_hidrico(0);
HardwareSerial serial_eletrico(1);

/* PORTAS */

const int RESERVATORIO_ECHO = 22, RESERVATORIO_TRIGGER = 23, LED_ACIONEMENTO = 25;
int portasDisponiveis[18] = {4,5,12,13,14,15,25,26,27,32,33,34,35,36,39,18,19};

/* VARIAVEIS */

const int CAPACITY_JSON = 96,TENSAO = 110,CALIBRACAO = 6.0606;
long double ALTURA_RESERVATORIO = 0, CAPACIDADE_RESERVATORIO = 0, KW = 0;

/* INSTANCIACAO */

BluetoothSerial bluetooth;
Ultrasonic ultrasonic(RESERVATORIO_TRIGGER, RESERVATORIO_ECHO);
StaticJsonDocument < CAPACITY_JSON > JSON;
const int LED_ACIONAMENTO = 2;

/* CONTROLE DE LEITURA */

double ULTIMA_LEITURA_HIDRICA = 0, ULTIMA_LEITURA_ELETRICA = 0;

void setup() {
    bluetooth.begin("SUSTENAPP - CONTROL");

    /* SERIAL */
    Serial.begin(115200);
    serial_hidrico.begin(9600);
    serial_eletrico.begin(4800);

    /* LED ACIONAMENTO */
    pinMode(LED_ACIONAMENTO, OUTPUT);
    digitalWrite(LED_ACIONEMENTO, HIGH);
}

void loop() {
    leituraBluetooth();
    delay(1000);
}

/* LEITURA CONTINUA */

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
                    enviaRelatorio(retornaConsumoEletrico(), "W");
                }
            } else if (retornaStringJSON("comando") == "controlador") {
                controlaDispositivo(retornaIntJSON("porta"));
            } else if (retornaStringJSON("comando") == "dispositivo") {
                enviaEstadoDispositivo(200, retornaEstadoDispositivo(retornaIntJSON("porta")));
            } else if (retornaStringJSON("comando") == "declaracao") {
                if (retornaStringJSON("tipo") == "reservatorio") {
                    declaraReservatorio(retornaDoubleJSON("capacidade"));
                } else if (retornaStringJSON("tipo") == "dispositivo") {
                    declaracaoDispositivo(retornaIntJSON("porta"));
                } else if (retornaStringJSON("tipo") == "eletricidade") {
                    declaraEletricidade();
                } else if (retornaStringJSON("tipo") == "hidricidade") {
                    declaraHidricidade();
                }
            } else if (retornaStringJSON("comando") == "disponibilidade") {
                informaPortaDisponivel();
            } else {
                enviaStatus(500, "FALHA NA COMUNICACAO");
            }
        }
    }
}

/* COMUNICACAO SERIAL */

String recebeInformacaoSerial(String tipo) {
    if (tipo == "hidrico") {
        recebeInformacaoHidrico();
    } else if (tipo == "eletrico") {
        recebeInformacaoEletrico();
    }

    return "";
}

double recebeInformacaoHidrico() {
    serial_hidrico.println("relatorio_hidrico");

    while (serial_hidrico.available()) {
        double leitura = serial_hidrico.readString().toDouble();
        double estimativa = leitura - ULTIMA_LEITURA_HIDRICA;
        ULTIMA_LEITURA_HIDRICA = leitura;

        return estimativa;
    }

    enviaStatus(400, "FALHA NA OBTENCAO DE CONSUMO");
}

double recebeInformacaoEletrico() {
    serial_eletrico.println("relatorio_eletrico");

    while (serial_eletrico.available()) {
        double leitura = serial_eletrico.readString().toDouble();
        double estimativa = leitura - ULTIMA_LEITURA_ELETRICA;
        ULTIMA_LEITURA_ELETRICA = leitura;

        return estimativa;
    }

    enviaStatus(400, "FALHA NA OBTENCAO DE CONSUMO");
}

/* COMUNICACAO BLUETOOTH */

String recebeInformacaoBluetooth() {
    return bluetooth.readString();
}

void enviaInformacaoBluetooth(String informacao) {
    String JSON_AUXILIAR = "{" + informacao + "}";
    char RETURN[JSON_AUXILIAR.length()];
    memcpy(RETURN, JSON_AUXILIAR.c_str(), JSON_AUXILIAR.length());

    if (bluetooth.connected()) {
        bluetooth.write((uint8_t * ) RETURN, JSON_AUXILIAR.length());
    }
}

/* CONVERSAO JSON */

void descompactaJSON() {
    String request = recebeInformacaoBluetooth();

    DeserializationError error = deserializeJson(JSON, request);
    if (error != DeserializationError::Ok) {
        // enviaInformacaoBluetooth(error.c_str());
    }
}

String compactaJSON() {
    String resposta;
    serializeJson(JSON, resposta);

    return resposta;
}

/* EXTRACAO DE VALORES JSON */

String retornaStringJSON(String atributo) {
    return JSON[atributo].as < String > ();
}

int retornaIntJSON(String atributo) {
    return JSON[atributo].as < int > ();
}

double retornaDoubleJSON(String atributo) {
    return JSON[atributo].as < double > ();
}

bool retornaBoolJSON(String atributo) {
    return JSON[atributo].as < bool > ();
}

/* CONTROLADOR DE DISPOSITIVO */

void controlaDispositivo(int porta) {
    int estado = !retornaEstadoDispositivo(porta);
    digitalWrite(porta, estado);
    enviaEstadoDispositivo(200, !estado);
}

/* STATUS */

double retornaConsumoEletrico() {
    return recebeInformacaoSerial("eletrico");
}

double retornaConsumoHidrico() {
    return recebeInformacaoSerial("hidrico");
}

double retornaVolumeReservatorio() {
    return ((CAPACIDADE_RESERVATORIO / ALTURA_RESERVATORIO) * (ultrasonic.read() * 100) / ALTURA_RESERVATORIO);
}

int retornaEstadoDispositivo(int porta) {
    return digitalRead(porta);
}

/* DECLARACAO */

void declaraReservatorio(double capacidade) {
    CAPACIDADE_RESERVATORIO = capacidade; // L
    ALTURA_RESERVATORIO = ultrasonic.read(); // CM

    enviaStatus(200, "RESERVATORIO CONFIGURADO");
}


void declaraEletricidade() {
  enviaStatus(200, "ELETRICIDADE MONITORAVEL");
}

void declaraHidricidade() {
  enviaStatus(200, "ELETRICIDADE MONITORAVEL");
}

/* GERENCIAMENTO DE PORTAS */

void informaPortaDisponivel() {
    for (int i = 0; i < 18; i++) {
        if (portasDisponiveis[i]) {
            enviaPortaDisponivel(200, portasDisponiveis[i]);
            return;
        }
    }

    enviaStatus(200, "ALCANCE DE LIMITE DE PORTAS UTILIZAVEIS");
}

void declaracaoDispositivo(int porta) {
    for (int i = 0; i < 18; i++) {
        if (portasDisponiveis[i] == porta) {
            portasDisponiveis[i] = NULL;
            declaraPorta(porta, OUTPUT);
            enviaStatus(200, "DISPOSITIVO ADICIONADO");
            return;
        }
    }

    enviaStatus(402, "FALHA NA ADICAO DO DISPOSITIVO");
}

void declaraPorta(int porta, int fluxo) {
    pinMode(porta, fluxo);

    if (fluxo == OUTPUT) {
        digitalWrite(porta, LOW);
    }
}

/* RELATORIO */

void enviaRelatorio(double consumo, String unidade) {
    enviaInformacaoBluetooth(
        "'consumo':" + String(consumo) + ",'unidade':'" + unidade + "'"
    );
}

void enviaStatus(int status, String mensagem) {
    enviaInformacaoBluetooth(
        "'status':" + String(status) + ",'mensagem':'" + mensagem + "'"
    );
}

void enviaEstadoDispositivo(int status, int estado) {
    enviaInformacaoBluetooth(
        "'status':" + String(status) + ",'estado':" + String(estado)
    );
}

void enviaPortaDisponivel(int status, int porta) {
    enviaInformacaoBluetooth(
        "'status':" + String(status) + ",'porta':" + String(porta)
    );
}
