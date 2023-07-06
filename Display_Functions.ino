String multimeterModes(int mode) {

  switch (mode) {
    case 0: return F("Voltmeter\n\nRange: 0-30 V");
    case 1: return F("Ammeter\n\nRange: 0-10 A");
    case 2: return F("Ohmmeter\n\nRange: 1-100 kOhms");
    case 3: return F("Capacitance\n\nRange: 1-2500 uF");
    case 4: return F("Inductance\n\nRange: 0-100 uH");
    case 5: return F("Temperature\n\nRange: 15-50 C");
    case 6: return F("Lighting\n\nRange: 0-100 %");
    case 7: return F("Noise\n\nRange: 0-100 Db");
  }
}

String measurementUnits(int mode) {

  switch (mode) {
    case 0: return F("V");
    case 1: return F("A");
    case 2: return F("K Ohm");
    case 3: return F("uF");
    case 4: return F("uH");
    case 5: return F("C");
    case 6: return F("%");
    case 7: return F("Db");
  }
}

void welcomeScreen() {

  static const int ANIMATION_FRAMES = 30;

  OLEDScreen.clearDisplay();

  OLEDScreen.setTextSize(2.5);
  OLEDScreen.setTextColor(SSD1306_WHITE);
  OLEDScreen.cp437(true);
  OLEDScreen.setCursor(10, 15);
  OLEDScreen.print(F("Welcome"));

  OLEDScreen.display();

  delay(1000);

  for (int i = 0; i < ANIMATION_FRAMES; i++) {

    OLEDScreen.clearDisplay();

    OLEDScreen.setTextSize(2.5);
    OLEDScreen.setTextColor(WHITE);
    OLEDScreen.setCursor(10 - 3.5 * i, 15);
    OLEDScreen.print(F("Welcome"));

    OLEDScreen.display();
  }
}

void displayMode() {

  OLEDScreen.clearDisplay();

  OLEDScreen.setTextSize(1.2);
  OLEDScreen.setTextColor(WHITE);
  OLEDScreen.setCursor(0, 0);
  OLEDScreen.print(F("Mode: "));
  OLEDScreen.print(multimeterModes(mode));

  OLEDScreen.display();
}

void displayValue(float value) {

  displayMode();
  if (holdMode) showHold();

  OLEDScreen.setTextSize(2);
  OLEDScreen.setTextColor(WHITE);
  OLEDScreen.setCursor(0, 33);

  if (value  > UPPER_BOUNDS[mode]) OLEDScreen.print(F("OUT OF RANGE"));
  else if (mode == Ohmmeter && value < SHORT_CIRCUIT) {

    OLEDScreen.print(F("-SHORT-\n"));
    OLEDScreen.setTextSize(1);
    OLEDScreen.print(F("< "));
    OLEDScreen.print(SHORT_CIRCUIT * 1000);
    OLEDScreen.print(F(" Ohms"));
  }
  else if (value < LOWER_BOUNDS[mode]) {
    
    OLEDScreen.print(F("< "));
    OLEDScreen.print(String(LOWER_BOUNDS[mode], 3));
    OLEDScreen.print(measurementUnits(mode));
  }
  else {

    OLEDScreen.print(String(value, 3));
    OLEDScreen.print(measurementUnits(mode));
  }

  OLEDScreen.display();
}

void showHold() {

  OLEDScreen.setTextSize(1);
  OLEDScreen.setTextColor(WHITE);
  OLEDScreen.setCursor(120, 0);
  OLEDScreen.print(F("H"));
  OLEDScreen.setCursor(120, 8);
  OLEDScreen.print(F("O"));
  OLEDScreen.setCursor(120, 16);
  OLEDScreen.print(F("L"));
  OLEDScreen.setCursor(120, 24);
  OLEDScreen.print(F("D"));
  
  OLEDScreen.display();
}
