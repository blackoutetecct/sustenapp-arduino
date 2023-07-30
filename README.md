<h1 align=center>SUSTENAPP-ARDUINO</h1>

<p align="center">
  <img src="logo_sustenapp.png" width="500">
</p>

#
### TECNOLOGIAS

![Arduino](https://img.shields.io/badge/Arduino-0D1117?style=for-the-badge&logo=Arduino&logoColor=00979D&labelColor=0D1117)&nbsp;
![C++](https://img.shields.io/badge/C%2B%2B-0D1117?style=for-the-badge&logo=C%2B%2B&logoColor=00599C&labelColor=0D1117)&nbsp;
![BLUETOOTH](https://img.shields.io/badge/bluetooth-0D1117?style=for-the-badge&logo=bluetooth&logoColor=00B2FC&labelColor=0D1117)&nbsp;
![JSON](https://img.shields.io/badge/json-0D1117?style=for-the-badge&logo=json&logoColor=5E5C5C&labelColor=0D1117)&nbsp;

#
### DOCUMENTACAO DA APLICACAO

- *CONTROLE DE DISPOSITIVO*

```json
{
    "comando": "controlador",
    "porta": <int>
}
```

```json
{
    "status" : <int>,
    "estado" : <int>
}
```

- *ESTADO DE DISPOSITIVO*

```json
{
    "comando": "dispositivo",
    "porta": <int>
}
```

```json
{
    "status" : <int>,
    "estado" : <int>
}
```

-  *DECLARACAO DE DISPOSITIVO*

```json
{
    "comando": "declaracao",
    "tipo": "dispositivo",
    "porta" : int
}
```

```json
{
    "status" : <int>,
    "mensagem" : <string>
}
```

- *DECLARACAO DE CAPACIDADE DE RESERVATORIO*

```json
{
    "comando": "declaracao",
    "tipo": "reservatorio",
    "capacidade" : <double>
}
```

```json
{
    "status" : <int>,
    "mensagem" : <string>
}
```


- *CONSUMO*


```json
{
    "comando": "consumo",
    "tipo": "<hidrico/eletrico>",
    "renovavel": <bool>
}
```

```json
{
    "consumo": <float>,
    "unidade": <string>
}
```

# 
### BIBLIOTECAS E REFERENCIAS

- https://www.arduino.cc/reference/en/libraries/emonlib/
- https://www.arduino.cc/reference/en/libraries/ultrasonic/
- https://www.arduino.cc/reference/en/libraries/arduinojson/

#
