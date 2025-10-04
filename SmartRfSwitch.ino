// ESP32C3FN4 SuperMini Board
// ===============================================================
// Arduino IDE settings:
//   - Board: ESP32C3 Dev Module
//   - ESP CDC On Boot: Enabled
//   - CPU Frequency: 80MHz (WiFi)
//   - Core Debug Level: None
//   - Erase All Flash Before Sketch Upload: Disabled
//   - Flash frequency: 80Mhz
//   - Flash Mode: QIO
//   - Flash Size: 4MB (32Mb)
//   - JTAG Adapter: Disabled
//   - Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
//   - Upload Speed: 921600
//   - Zigbee Mode: Disabled
//   - Programmer: Esptool
// ===============================================================

#include <HomeSpan.h>


/**************************************************************************************/
/*                                        Pins                                        */

#define STATUS_LED_PIN      GPIO_NUM_8
#define CONTROL_PIN         GPIO_NUM_9

#define SWITCH_SIGNAL_PIN   GPIO_NUM_10
#define SWITCH_CONTROL_PIN  GPIO_NUM_3

/**************************************************************************************/


/**************************************************************************************/
/*                                  HomeSpan sensors                                  */

bool LampState = false;
bool CurrentLampState = false;

struct RfSwitchingLamp : Service::LightBulb
{
    SpanCharacteristic* FPower;
    
    RfSwitchingLamp() : Service::LightBulb()
    {
        FPower = new Characteristic::On();
    }
    
    bool update()
    {
        LampState = FPower->getNewVal();
        CurrentLampState = LampState;
        digitalWrite(SWITCH_CONTROL_PIN, LampState);
        return true;
    }
};

/**************************************************************************************/


/**************************************************************************************/
/*                                 Arduino  rountines                                 */

RfSwitchingLamp* Lamp;

void IRAM_ATTR SwitchSignalInterrupt()
{
    LampState = !LampState;
}

// Arduino initialization routine.
void setup()
{
    // Initialize debug serial port.
    Serial.begin(115200);

    // Initialize pins.
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(CONTROL_PIN, INPUT);

    pinMode(SWITCH_SIGNAL_PIN, INPUT);
    pinMode(SWITCH_CONTROL_PIN, OUTPUT);

    // Turn all output pins off.
    digitalWrite(STATUS_LED_PIN, HIGH);
    digitalWrite(SWITCH_CONTROL_PIN, LOW);

    // Initialize HomeSpan.
    homeSpan.setControlPin(CONTROL_PIN);
    homeSpan.setStatusPin(STATUS_LED_PIN);
    homeSpan.setPairingCode("63105322");
    homeSpan.begin(Category::Lighting, "DroneTales RF Switching Lamp");

    // Build device's serial number.
    char Sn[24];
    snprintf(Sn, 24, "DRONETALES-%llX", ESP.getEfuseMac());

    // Configure device.
    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();
    new Characteristic::Manufacturer("DroneTales");
    new Characteristic::SerialNumber(Sn);
    new Characteristic::Model("DroneTales RF Switching Lamp");
    new Characteristic::FirmwareRevision("1.0.0.0");
    Lamp = new RfSwitchingLamp();

    // Attach remote control interrupt.
    attachInterrupt(SWITCH_SIGNAL_PIN, SwitchSignalInterrupt, CHANGE);
}

// Arduino main loop.
void loop()
{
    if (CurrentLampState != LampState)
    {
        CurrentLampState = LampState;
        Lamp->FPower->setVal(CurrentLampState);
        digitalWrite(SWITCH_CONTROL_PIN, LampState ? HIGH : LOW);
    }

    homeSpan.poll();
}

/**************************************************************************************/