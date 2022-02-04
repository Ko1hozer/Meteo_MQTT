// Настройки WiFi
#define wifi_ssid             "SKLAD"
#define wifi_password         "888888889"

// Сервер MQTT
#define mqtt_server           "192.168.2.11"
    
// Настройки OTA
const char *ota_hostname      = "WeatherStation";
const char *ota_pwd           = "22031989";

#define ESPHostname           "WeatherStation"

// MQTT топики
#define outTopic              "service/out"
#define inTopic               "sensor/in"
#define humidity_topic        "sensor/humidity"
#define temperature_topic     "sensor/temperature"
#define pressure_topic        "sensor/pressure"
#define vitessevent_topic     "sensor/vitessevent"
#define vitesserafale_topic   "sensor/vitesserafale"
#define dirwind_topic         "sensor/dirwind"
#define windangw_topic        "sensor/windangw"
