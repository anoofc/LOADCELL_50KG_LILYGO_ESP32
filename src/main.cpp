#include <Arduino.h>
#include <HX711.h>
#include <BluetoothSerial.h>
#include <Preferences.h>

// ===================== PIN DEFINITIONS =====================
#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN  13

// ===================== TIMING =====================
#define RUN_INTERVAL_MS 10

// ===================== OBJECTS =====================
HX711 scale;
BluetoothSerial SerialBT;
Preferences preferences;

// ===================== SYSTEM MODE =====================
enum SystemMode {
    MODE_RUN,
    MODE_CAL
};

SystemMode currentMode = MODE_RUN;

// ===================== GLOBAL VARIABLES =====================
float calibrationFactor = -7050.0;
unsigned long lastRunTime = 0;
String btBuffer = "";


// =============================================================
// ===================== NVS STORAGE ===========================
// =============================================================

void saveCalibrationFactor() {

  preferences.begin("loadcell", false);
  preferences.putFloat("calFactor", calibrationFactor);
  preferences.end();
}

void loadCalibrationFactor() {

  preferences.begin("loadcell", true);

  if (preferences.isKey("calFactor")) {
    calibrationFactor = preferences.getFloat("calFactor");
  }

  preferences.end();
}

// =============================================================
// ===================== CALIBRATION LOGIC =====================
// =============================================================

void processCalibrationCommand(String cmd) {

  if      (cmd == "+") { calibrationFactor += 10; }
  else if (cmd == "-") { calibrationFactor -= 10; } 
  else    { SerialBT.println("Invalid command"); return;}

  scale.set_scale(calibrationFactor);

  if (scale.is_ready()) {
    float weight = scale.get_units(5);

    SerialBT.print("Weight: ");
    SerialBT.print(weight);
    SerialBT.print(" | CalFactor: ");
    SerialBT.println(calibrationFactor);
  }
}

// =============================================================
// ===================== MODE FUNCTIONS ========================
// =============================================================

void enterCalibrationMode() {

  currentMode = MODE_CAL;
  SerialBT.println("Entered Calibration Mode");
}

void enterRunMode() {

  saveCalibrationFactor();
  scale.set_scale(calibrationFactor);

  currentMode = MODE_RUN;
  SerialBT.println("Calibration Saved. Entered Run Mode");
}

// =============================================================
// ===================== BLUETOOTH HANDLER =====================
// =============================================================

void handleBluetooth() {

  while (SerialBT.available()) {
    char c = SerialBT.read();

    if (c == '\n' || c == '\r') {
      btBuffer.trim();
      btBuffer.toUpperCase();

      if (btBuffer == "CAL") { enterCalibrationMode(); }
      else if (btBuffer == "RUN") { enterRunMode(); }
      else if (currentMode == MODE_CAL) { processCalibrationCommand(btBuffer); }
      btBuffer = "";
    } else { btBuffer += c;}
  }
}


void setup() {

  Serial.begin(115200);

  SerialBT.begin("ESP32_LoadCell");   // Bluetooth device name

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  loadCalibrationFactor();

  scale.set_scale(calibrationFactor);
  scale.tare();

  currentMode = MODE_RUN;
}

// =============================================================

void loop() {

  handleBluetooth();

  if (currentMode == MODE_RUN) {

    if (millis() - lastRunTime >= RUN_INTERVAL_MS) {
      lastRunTime = millis();

      if (scale.is_ready()) {
        float weight = scale.get_units(5);   // small averaging
        SerialBT.println(weight);
      }
    }
  }
}



