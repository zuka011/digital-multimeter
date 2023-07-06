double average(double newValue) {
  return (newValue + lastValue * nValuesOnHold) / ++nValuesOnHold;
}

double readValue(int nSamples) {

  double sum = 0;
  for (int i = 0; i < nSamples; i++) {

    double currValue = readValue();
    sum += currValue;

    if (modeSwitched) {

      modeSwitched = false;
      updateMode();
      return 0;
    }

    if (currValue == 0) nSamples--;
  }
  return sum / nSamples;
}

double readValue() {

  if (mode == Voltmeter)    return measureVoltage();

  if (mode == Ammeter)      return measureCurrent();

  if (mode == Ohmmeter)     return measureResistance();

  if (mode == Capacitance)  return measureCapacitance();

  if (mode == Inductance)   return measureInductance();

  if (mode == Temperature)  return measureTemperature();

  if (mode == Lighting)     return measureLighting();

  if (mode == Noise)        return measureNoise();

  return 0;
}

double measureVoltage() {

  static const double CONST_TERM = 1 / ADC_DIVISIONS * VOLTMETER_MAX;

  return analogRead(VOLTMETER_PIN) * CONST_TERM;
}

double measureCurrent() {

  static const double CONST_TERM = 1 / ADC_DIVISIONS * AMMETER_MAX / R_SHUNT;

  return analogRead(VOLTMETER_PIN) * CONST_TERM;
}

double measureResistance() {

  static const double CONST_TERM = 1 / ADC_DIVISIONS * ADC_MAX;

  double V = analogRead(VOLTMETER_PIN) * CONST_TERM;

  if (V > V_SUPPLY) return 0;

  return R_SUPPLY / (V_SUPPLY / V - 1) * OHM_CALIBRATOR;
}

double measureCapacitance() {

  static const double CONST_TERM1 = 1 / ADC_DIVISIONS * ADC_MAX;
  static const double CONST_TERM2 = 1e-3 / R_SUPPLY;

  double V = analogRead(VOLTMETER_PIN) * CONST_TERM1;

  digitalWrite(CONTROL_PINS[0], HIGH);
  digitalWrite(CONTROL_PINS[1], LOW);

  while (V > CHARGE_LOWER_BOUND) {

    if (mode != Capacitance) return 0;
    V = analogRead(VOLTMETER_PIN) * CONST_TERM1;
  }

  digitalWrite(CONTROL_PINS[0], LOW);
  digitalWrite(CONTROL_PINS[1], HIGH);

  unsigned long timer = micros();
  unsigned long difference = 0;

  while (V < CHARGE_UPPER_BOUND) {

    if (mode != Capacitance) return 0;
    V = analogRead(VOLTMETER_PIN) * CONST_TERM1;
    difference = micros() - timer;
  }

  return difference * CONST_TERM2 / log(1 / (1 - V / V_SUPPLY));
}

double measureInductance() {

  static const double CONST_TERM1 = 1 / ADC_DIVISIONS * ADC_MAX;
  double V = analogRead(VOLTMETER_PIN) * CONST_TERM1;

  digitalWrite(CONTROL_PINS[0], HIGH);
  digitalWrite(CONTROL_PINS[1], HIGH);

  while (V > CHARGE_LOWER_BOUND) {

    if (mode != Inductance) return 0;
    V = analogRead(VOLTMETER_PIN) * CONST_TERM1;
  }

  digitalWrite(CONTROL_PINS[0], LOW);
  digitalWrite(CONTROL_PINS[1], LOW);
  digitalWrite(CONTROL_PINS[4], HIGH);

  unsigned long timer = micros();
  unsigned long difference = 0;

  while (V > CHARGE_LOWER_BOUND) {

    if (mode != Inductance) return 0;
    V = analogRead(VOLTMETER_PIN) * CONST_TERM1;
    difference = micros() - timer;
  }

  return difference * R_SUPPLY / log(V_SUPPLY / V);
}

double measureTemperature() {

  double V = analogRead(THERMISTOR_PIN) / ADC_DIVISIONS * ADC_MAX / THERMISTOR_GAIN;
  double R = R_THERMISTOR_REFERENCE / (THERMISTOR_SUPPLY / V - 1);

  double T = REF_TEMPERATURE * THERMISTOR_B / (THERMISTOR_B + REF_TEMPERATURE * log(R / REF_RESISTANCE)) - ZERO_CELSIUS_IN_KELVIN;
  return T;
}

double measureLighting() {

  double V = analogRead(PHOTORESISTOR_PIN) / ADC_DIVISIONS * ADC_MAX;

  return exp(V - ADC_MAX) * 100;
}

double measureNoise() {

  double V = analogRead(NOISE_PIN) / ADC_DIVISIONS * ADC_MAX;

  return V;
}
