#ifdef ESP32
#include <ESPmDNS.h>
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif
#include <ESPUI.h>
#include <ESP_EEPROM.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#include "WiFiConfig.h" 


struct Settings
{
    uint8_t irrigationHour;
    uint8_t irrigationMinute;
    uint8_t irrigationDuration;
    bool irrigationEnabled;
} settings;

bool irrigated = false;

const uint8_t pump = D5;

bool firstStart = false;


const char* hostname = "farmer";


uint8_t lastMinute = 0;


int irrigationHourNI, irrigationMinuteNI, irrigationDurationNI, enableButton, resetButton;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.ume.tubitak.gov.tr", 10800, 60000);

time_t getNTPTime()
{
    return timeClient.getEpochTime();
}

bool forceTimeSync()
{
    Serial.print("Forcing time sync! ");
    timeClient.forceUpdate();
    setSyncInterval(0);
    time_t time = now();
    setSyncInterval(300);
    const bool success = year(time) > 2019;
    Serial.println(success ? "Successful" : "Failed");
    return success;
}


bool connectToAP(String ssid, String password)
{
    if (WiFi.isConnected())
    {
        WiFi.disconnect();
    }
    else
    {
        WiFi.softAPdisconnect();
    }
   
    WiFi.mode(WIFI_STA);
    
    WiFi.begin(ssid.c_str(), password.c_str());

    
    uint8_t state = WiFi.status();
    while (true)
    {
        if (state == WL_CONNECTED || state == WL_CONNECT_FAILED || state == WL_NO_SSID_AVAIL)
        {
            break;
        }
        delay(500);
        state = WiFi.status();
    }
    const bool connected = state == WL_CONNECTED;
    if (connected)
    {
        WiFi.setAutoReconnect(true);
    }
    return connected;
}

void beginEEPROM()
{
    
    EEPROM.begin(sizeof(Settings));

    firstStart = EEPROM.percentUsed() < 0;

    if (!firstStart)
    {
        loadSettings();
    }

    // EEPROM.commitReset(); 
}


void loadSettings()
{
    EEPROM.get(0, settings);
}


bool storeSettings()
{
    EEPROM.put(0, settings);
    return EEPROM.commit();
}

void enabledPump()
{
    Serial.println("Selenoid Aktif");
    digitalWrite(D4, LOW);
    digitalWrite(pump, HIGH);
    delay(300);
    digitalWrite(pump, LOW);
}

void disabledPump()
{
    Serial.println("Selenoid Deaktif");
    digitalWrite(D4, HIGH);
    digitalWrite(pump, HIGH);
    delay(300);
    digitalWrite(pump, LOW);
}

void updateIrrigationHour(Control* sender, int value)
{
    Serial.println("Updating IrrigationHour to " + sender->value);
    settings.irrigationHour = sender->value.toInt();
    storeSettings();
}

void updateIrrigationMinute(Control* sender, int value)
{
    Serial.println("Updating IrrigationMinute to " + sender->value);
    settings.irrigationMinute = sender->value.toInt();
    storeSettings();
}

void updateIrrigationDuration(Control* sender, int value)
{
    Serial.println("Updating IrrigationDuration to " + sender->value);
    settings.irrigationDuration = sender->value.toInt();
    storeSettings();
}

void updateEnableButton(Control* sender, int value)
{
    Serial.println("Updating EnableButton to " + String(value));
    settings.irrigationEnabled = sender->value.toInt();
    storeSettings();
}

void updateResetButton(Control* sender, int value)
{
    if (value == B_DOWN)
    {
        Serial.println("Updating ResetButton to " + String(value));
        irrigated = false;
    }
}

void updateTestButton(Control* sender, int value)
{
    if (value == B_DOWN)
    {
        enabledPump();
    }
    else
    {
        disabledPump();
    }
}

void setupGUI()
{
   ESPUI.label("Farmer Pickles", ControlColor::Peterriver, "<img src='https://static.wikia.nocookie.net/btb/images/1/14/Farmer_Pickles.png/'>");
    
    irrigationHourNI = ESPUI.number(
        "Sulama Saati", updateIrrigationHour, ControlColor::Peterriver, settings.irrigationHour, 0, 24);
    irrigationMinuteNI = ESPUI.number(
        "Sulama Dakikası", updateIrrigationMinute, ControlColor::Peterriver, settings.irrigationMinute, 0, 60);
    irrigationDurationNI = ESPUI.number(
        "Sulama Süresi (1 Birim = 30 Dakika)", updateIrrigationDuration, ControlColor::Peterriver, settings.irrigationDuration, 0, 255);
    enableButton = ESPUI.switcher(
        "Sulama Durumu", updateEnableButton, ControlColor::Peterriver, settings.irrigationEnabled);
    resetButton = ESPUI.button("Ortamı Resetle", updateResetButton, ControlColor::Alizarin, "reset");
    ESPUI.button("Sulama Testi", updateTestButton, ControlColor::Alizarin, "test");

    ESPUI.begin("Farmer Pickles");
}

void setup()
{
    WiFi.hostname(hostname);

    Serial.begin(115200);

    Serial.println("My ssid: " + String(ssid));

    
    pinMode(D4, OUTPUT);
    pinMode(pump, OUTPUT);
    disabledPump();

    
    beginEEPROM();

    if (firstStart)
    {
        settings.irrigationHour = 18;
        settings.irrigationMinute = 0;
        settings.irrigationDuration = 10;
        settings.irrigationEnabled = false;
        storeSettings();
    }

    // Setup wifi
    bool connected = false;
    const String ssidString(ssid);
    const String ssidPassword(password);
    while (!connected)
    {
        connected = connectToAP(ssidString, ssidPassword);
    }

    
    MDNS.begin(hostname);

    setupGUI();

    timeClient.begin();
    setSyncProvider(getNTPTime);
    bool synced = false;
    while (!synced)
    {
        synced = forceTimeSync();
        delay(500);
    }
}

void loop()
{
    const time_t currentTime = now();
    const uint8_t currentMinute = minute(currentTime);

    if (settings.irrigationEnabled && currentMinute != lastMinute)
    {
        lastMinute = currentMinute;
        const uint8_t currentHour = hour(currentTime);
        Serial.println("Current time: " + String(currentHour) + ":" + String(currentMinute));

        if (currentHour == settings.irrigationHour && currentMinute == settings.irrigationMinute && irrigated == false)
        {
            irrigated = true;

            enabledPump();
            delay(settings.irrigationDuration * 1800000);
            disabledPump();
        }

        if (currentHour == 0)
        {
            irrigated = false;
        }
    }

    
    timeClient.update(); 
    
    MDNS.update();
    yield();
}
