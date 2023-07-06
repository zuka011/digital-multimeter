void nextMode() {

  mode = (mode + 1) % N_MODES;
  modeSwitched = true;
  updateMode();
}

void prevMode() {

  mode = (N_MODES + mode - 1) % N_MODES;
  modeSwitched = true;
  updateMode();
}

void hold() {

  static const int DEBOUNCE_DELAY = 200;
  static unsigned long debounceTimer = 0;

  if (millis() - debounceTimer > DEBOUNCE_DELAY) {

    holdMode = !holdMode;
    if(holdMode) nValuesOnHold = 1;
    
    debounceTimer = millis();
  }
}

void updateMode() {
  setControlPins(CONTROL_PIN_VALUES[mode]);
}

void setControlPins(uint8_t controlPinValues) {

  for (int i = 0; i < N_CONTROL_PINS; i++) {

    digitalWrite(CONTROL_PINS[i], controlPinValues % 2);
    controlPinValues /= 2;
  }
}
