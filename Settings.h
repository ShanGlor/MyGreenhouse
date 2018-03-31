#ifndef SETTINGS_H
#define SETTINGS_H

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Running on Mega
#define MY_RF24_CE_PIN 49
#define MY_RF24_CS_PIN 53

#define MY_GATEWAY_MQTT_CLIENT
// Enable gateway ethernet module type
#define MY_GATEWAY_TINYGSM

// Define the GSM modem
#define TINY_GSM_MODEM_ESP8266
//#define TINY_GSM_MODEM_A6
#define MY_GSM_APN "internet"
#define MY_GSM_USR ""
#define MY_GSM_PIN ""
#define MY_GSM_PSW "E76GBLKI"
#define MY_GSM_SSID "4G-Gateway-8FAB"

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial3
// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

// Mosquitto does not support 3.1.1, so need to use the older 3.1 version
#define MQTT_VERSION MQTT_VERSION_3_1

// Set this node's subscribe and publish topic prefix
#define MY_MQTT_PUBLISH_TOPIC_PREFIX "myGreenhouse-out"
#define MY_MQTT_SUBSCRIBE_TOPIC_PREFIX "myGreenhouse-in"

// Set MQTT client id
#define MY_MQTT_CLIENT_ID "myGreenhouse"

// Enable these if your MQTT broker requires usenrame/password
#define MY_MQTT_USER "MyGreenhouse"
#define MY_MQTT_PASSWORD "luOavOXFFFDxrQFlboaVK"

// MQTT broker ip address or url. Define one or the other.
#define MY_CONTROLLER_URL_ADDRESS "home.raitwebs.com"

// The MQTT broker port to to open
#define MY_PORT 9993

// Security and signing
#define MY_SIGNING_SOFT
#define MY_SIGNING_SOFT_RANDOMSEED_PIN 0
#define MY_SIGNING_REQUEST_SIGNATURES

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
//#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
//#define MY_INCLUSION_MODE_BUTTON_PIN  3

// Set blinking period
#define MY_DEFAULT_LED_BLINK_PERIOD 300

// Inverses the behavior of leds
//#define MY_WITH_LEDS_BLINKING_INVERSE

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED

#define REPORT_INTERVAL           600 // define report interval in seconds for all sensors. This is when a full report is sent, no matter what
#define VCC_VOLTAGE_READ          5.0 // Actual supply voltage going to the VCC. Use a multimeter to check
#define VCC_VOLTAGE_REPORTED      4.97 // Set this to the same as above for your first boot. Then insert the value reported in the Serial window during boot
#define ONE_MSECOND_IN_MICROS     100 // Depends on your crystal. 8MHz crystal:500000, 16MHz crystal:1000000, etc. If your time intervals are off, this is the reason

// Air humidity and temperature sensor settings (SHT21)
#define MG_USE_AHUM
#define AHUM_MEASUREMENT_INTERVAL  10 // Time in seconds, between consecutive measurements
#define AHUM_REPORT_THRESHOLD      50 // The difference in ppm between two consecutive readings to send an immediate report
#define AHUM_HUM_ID      6
#define AHUM_TMP_ID      7
#define AHUM_DEW_ID      8

// PPM (EC) sensor settings
#define MG_USE_PPM
#define PPM_MEASUREMENT_INTERVAL  10 // Time in seconds, between consecutive measurements
#define PPM_REPORT_THRESHOLD      50 // The difference in ppm between two consecutive readings to send an immediate report
#define PPM_SENSE_PIN   A13 // Analog pin used for the probe
#define PPM_POWER_PIN   A8 // Digital pin used to supply power to the probe
#define PPM_GROUND_PIN   A15
#define PPM_RESISTOR    560 // No less than 300 and no more than 1K resistor should be used. Around 500 gives the best resolution
#define PPM_CALIBRATION 1.00  // EC value of Calibration solution in s/cm
#define PPM_TEMP_COEF   0.019 // 0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info]
#define PPM_EC_CONVERSION 0.64 // USA: 0.5, EU: 0.64, AU: 0.7
#define PPM_TEMP_SENSOR 10 // Temperature sensor ID that is measuring your solution temperature
#define PPM_ID          4 // MySensors Child ID
#define PPM_UNIT_ID     5 // Mysensors Child ID for selecting the unit (ms/cm or ppm)
#define PPM_CAL_ID      3 // Mysensors Child ID for the K value

// Relay settings. TODO: Make it easy to use more than two relays
#define MG_USE_RLY_1
#define MG_USE_RLY_2
#define RLY_PIN_1   5
#define RLY_PIN_2   6
#define RLY_ON      0  // GPIO value to write to turn on attached relay
#define RLY_OFF     1 // GPIO value to write to turn off attached relay
#define RLY_ID_1    1 // MySensors Child ID
#define RLY_ID_2    2 // MySensors Child ID

// LCD settings
#define LCD_BACKLIGHT_PIN 12
#define LCD_DEFAULT_CONTRAST 200
#define LCD_FONT u8g2_font_6x12_t_symbols
#define LCD_FONT_X 6
#define LCD_FONT_Y 12
#define LCD_OFFSET_X 5
#define LCD_OFFSET_Y 2
#define LCD_WIDTH 120
#define LCD_HEIGHT 60

//*********** Menu setup **********//
#define MENU_MAX_DEPTH 2
// rotary encoder pins
#define encA    3
#define encB    4
#define encBtn  2
#define ENC_STEPS_PER_NOTCH 4
#define ENC_DECODER ENC_NORMAL
//********* End Menu setup *******//

#endif // SETTINGS_H
