/*
 * SPDX-FileCopyrightText: 2025 Seu Nome
 * SPDX-License-Identifier: Apache-2.0
 *
 * Exemplo MQTT 5 para ESP32: controla o LED onboard (GPIO2)
 * via tópico "/ifpe/ads/embarcados/esp32/led".
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "mqtt_client.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"

#include "driver/gpio.h"

static const char *TAG = "mqtt5_led_ctrl";

/// Pino do LED (devkit típico usa o LED interno em GPIO2)
#define LED_GPIO        GPIO_NUM_2
/// Tópico para controle do LED
#define TOPIC_LED       "/ifpe/ads/embarcados/esp32/led"

/**
 * @brief Inicializa o LED como saída e mantém apagado.
 */
static void led_init(void)
{
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0);
}

/**
 * @brief Callback de eventos MQTT
 */
static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: inscrevendo em %s", TOPIC_LED);
        esp_mqtt_client_subscribe(client, TOPIC_LED, 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED (msg_id=%d)", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA: TOPIC=%.*s  PAYLOAD=%.*s",
                 event->topic_len, event->topic,
                 event->data_len,  event->data);

        if (strncmp(event->topic, TOPIC_LED, event->topic_len) == 0) {
            // Espera payload "1" ou "0"
            if (event->data_len == 1 && event->data[0] == '1') {
                gpio_set_level(LED_GPIO, 1);
                ESP_LOGI(TAG, "→ LED ACESO");
            } else {
                gpio_set_level(LED_GPIO, 0);
                ESP_LOGI(TAG, "→ LED APAGADO");
            }
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        break;

    default:
        break;
    }
}

/**
 * @brief Inicializa o cliente MQTT 5 e conecta ao broker.
 */
static void mqtt5_app_start(void)
{
    // Inicializa transport TCP
    esp_transport_handle_t transport = esp_transport_tcp_init();

    // Conﬁguração MQTT 5
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri            = CONFIG_BROKER_URL,
        .network.transport             = transport,
        .network.disable_auto_reconnect= false,
        .session.protocol_ver          = MQTT_PROTOCOL_V_5,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    // Logging
    esp_log_level_set("*",            ESP_LOG_INFO);
    esp_log_level_set("mqtt5_led_ctrl", ESP_LOG_INFO);

    ESP_LOGI(TAG, "Inicializando NVS, netif e event loop...");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    ESP_LOGI(TAG, "Inicializando GPIO do LED...");
    led_init();

    ESP_LOGI(TAG, "Iniciando cliente MQTT5...");
    mqtt5_app_start();
}
