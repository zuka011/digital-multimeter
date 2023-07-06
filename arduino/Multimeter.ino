#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#include <Rotary_Encoder.h>

#define VOLTMETER_PIN     A0
#define PHOTORESISTOR_PIN A1
#define THERMISTOR_PIN    A2
#define NOISE_PIN         A3

#define HOLD_BUTTON_PIN 2
#define ENCODER_A_PIN   8
#define ENCODER_B_PIN   9

#define OLED_ADDR   0x3C

#define N_CONTROL_PINS  5
#define N_MODES         8
#define SAMPLE_SIZE     5000
#define ALT_SAMPLE_SIZE 2

#define ADC_DIVISIONS 1024.0
#define ADC_MAX       5.03

#define VOLTMETER_MAX 30.55
#define AMMETER_MAX   9.9
#define R_SHUNT       0.87
#define V_SUPPLY      3.8
#define R_SUPPLY      25.3
#define SHORT_CIRCUIT 0.03

#define OHM_CALIBRATOR 1.05

#define CHARGE_LOWER_BOUND 0.02
#define CHARGE_UPPER_BOUND 0.966

#define IND_CALIBRATOR1 1.0
#define IND_CALIBRATOR2 0.0

#define THERMISTOR_SUPPLY 4.96
#define THERMISTOR_GAIN 35.45
#define R_THERMISTOR_REFERENCE 680.0e3
#define REF_TEMPERATURE 298.0
#define REF_RESISTANCE 10.0e3
#define THERMISTOR_B 3474.0
#define ZERO_CELSIUS_IN_KELVIN 273

// Controls Pins: Supply, Resistor1, Resistor2, Resistor3, Resistor4
static const int CONTROL_PINS[N_CONTROL_PINS] = {3, 4, 5, 6, 7};

enum Mode {Voltmeter, Ammeter, Ohmmeter, Capacitance, Inductance, Temperature, Lighting, Noise};

static const uint8_t CONTROL_PIN_VALUES[N_MODES] = {
  0B00001111,
  0B00000001,
  0B00000010,
  0B00000001,
  0B00000001,
  0B00001111,
  0B00001111,
  0B00001111,
};

static const int UPPER_BOUNDS[N_MODES] = {31, 11, 105, 2550, 110, 55, 100, 150};
static const float LOWER_BOUNDS[N_MODES] = {0.06, 0.015, SHORT_CIRCUIT, 0.5, 1, 0, 0, 1};

static volatile Mode mode = Voltmeter;
static volatile bool modeSwitched = false;
static volatile bool holdMode = false;

static double lastValue = 0;
static int nValuesOnHold = 0;

static double capCal = 100;

Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);
Adafruit_SSD1306 OLEDScreen(128, 64);

void setup() {

  Serial.begin(9600);

  encoder.init();
  encoder.attachFunction(nextMode, Clockwise);
  encoder.attachFunction(prevMode, CounterClockwise);

  OLEDScreen.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  pinMode(HOLD_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HOLD_BUTTON_PIN), hold, FALLING);

  welcomeScreen();

  for (int i = 0; i < N_CONTROL_PINS; i++) pinMode(CONTROL_PINS[i], OUTPUT);

  updateMode();
}

void loop() {

  double newValue = readValue((mode == Capacitance || mode == Inductance) ?  ALT_SAMPLE_SIZE : SAMPLE_SIZE);

  if(holdMode) lastValue = average(newValue);
  else lastValue = newValue;
  displayValue(lastValue);
}
