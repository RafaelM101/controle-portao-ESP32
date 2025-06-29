## Descrição do Projeto

Este exemplo demonstra como usar o cliente MQTT 5 do ESP-IDF para inscrever o ESP32 no tópico:

/ifpe/ads/embarcados/esp32/led

Quando a mensagem publicada no tópico for `"1"`, o LED interno (GPIO2) acenderá; quando for `"0"`, o LED apagará.

---

## Requisitos

- **ESP-IDF v5.4.2** (ou superior)  
- Placa **ESP32 DevKitC** (ou equivalente)  
- Broker MQTT compatível com MQTT 5 (por exemplo `test.mosquitto.org`)  
- Rede Wi-Fi disponível

## Arquivos Importantes: 
- `main/app_main.c` – Código-fonte do cliente MQTT5 e controle do LED  
- `sdkconfig.defaults` – Valores padrão para URI do Broker MQTT  
- `CMakeLists.txt` – Configuração de build do ESP-IDF  
- `README.md` – Este arquivo  

---

## Configuração

### Broker MQTT

Por padrão, o arquivo `sdkconfig.defaults` já define:

`CONFIG_BROKER_URL="mqtt://test.mosquitto.org:1883"`

Se desejar alterar:

** Execute:**
- idf.py menuconfig

** Navegue até:**
- Example Configuration  →  Broker URL

** Defina a URI no formato:**
- mqtt://<ENDEREÇO_DO_BROKER>:<PORTA>

## Credenciais Wi-Fi
Em Example Configuration configure:
Example Connection Configuration  →  WiFi SSID
                                  →  WiFi Password

Preencha com seu SSID e senha. Sem isso, o ESP32 não conseguirá conectar-se à rede e ao broker.

# Testando
Use o cliente de sua preferência (por ex. mosquitto_pub):

## Acender o LED
mosquitto_pub -h test.mosquitto.org -t /ifpe/ads/embarcados/esp32/led -m "1"

## Apagar o LED
mosquitto_pub -h test.mosquitto.org -t /ifpe/ads/embarcados/esp32/led -m "0"


