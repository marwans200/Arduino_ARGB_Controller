#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define PIN A5
#define NUM_LEDS 12
#define MODE_ADDR 0             // EEPROM address to store the mode
#define BACKLIGHT_ADDR 1        // EEPROM address to store the backlight state
#define MESSAGE_ADDR 120        // EEPROM starting address to store the message
#define MAX_MESSAGE_LENGTH 32   // Maximum length of the message to be stored
#define SPEED_ADDR 26           // EEPROM address to store the speed value
#define COLOR_SEQUENCE_ADDR 30  // EEPROM starting address to store the color sequence
#define MAX_SEQUENCE_LENGTH 100 // Maximum length of the color sequence
#define R_COLOR 101             // EEPROM address to store color
#define SPEED_CC 102            // EEPROM address to store Color Sequence Speed
#define SPEED_R 106             // EEPROM address to store rotate mode Speed
#define YIN_YANG_START_ADDR 110 // EEPROM addres to store Yin Yang values. From 110 - 115
#define PRESET_VALUE 116
#define SINGLE_LED_ADDR 117        // EEPROM address to store the Single LED RGB Values. From 117 - 119

#define BACKLIGHT_PIN A4        // Define your backlight control pin

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal lcd(5, 13, 12, 6, 4, 3, 2); // Define the LCD pins

enum RainbowMode {
  RAINBOW_WITH_OFFSET,
  RAINBOW_NO_OFFSET,
  COLOR_SEQUENCE,
  ROTATE_WITH_SHADOW,
  YIN_YANG,
  SINGLE_LED
};

RainbowMode currentMode = RAINBOW_WITH_OFFSET;
bool backlightOn = true; // Default backlight state

int ledSpeed = 10;       // Default speed
int speedCC = 50;
int speedR = 50;
int msgL=14;
String colorSequence = ""; // Store the color sequence
char r_color ="r";

int yingYangColors[6];
int SingleLEDColors[3];

int presetval = 0;
int maxpresetval = 15;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows

  // Load the last chosen mode from EEPROM
  currentMode = (RainbowMode)EEPROM.read(MODE_ADDR);

  // Load the last backlight state from EEPROM
  backlightOn = EEPROM.read(BACKLIGHT_ADDR);

  // Load the last speed value from EEPROM
  ledSpeed = EEPROM.read(SPEED_ADDR);
  speedCC = EEPROM.read(SPEED_CC);
  speedR = EEPROM.read(SPEED_R);

  if (ledSpeed <= 0) ledSpeed = 10; // Ensure the speed is a positive value
  if (speedCC <= 0) speedCC = 10; // Ensure the speed is a positive value
  if (speedR <= 0) speedR = 10; // Ensure the speed is a positive value
  r_color = EEPROM.read(R_COLOR);

  for(int i = 0; i < 6;i++){
    yingYangColors[i] = EEPROM.read(YIN_YANG_START_ADDR+i);
  }
  for(int i = 0; i < 3;i++){
    SingleLEDColors[i] = EEPROM.read(SINGLE_LED_ADDR+i);
  }
  presetval = EEPROM.read(PRESET_VALUE);
  // Load the color sequence from EEPROM
  char sequence[MAX_SEQUENCE_LENGTH];
  for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++) {
    sequence[i] = EEPROM.read(COLOR_SEQUENCE_ADDR + i);
  }
  colorSequence = String(sequence);

  pinMode(7,INPUT_PULLUP);

  // Set backlight pin as output
  pinMode(BACKLIGHT_PIN, OUTPUT);
  // Set the initial backlight state
  setBacklight(backlightOn);

  // Load the last message from EEPROM
  char message[MAX_MESSAGE_LENGTH];
  for (int i = 0; i < MAX_MESSAGE_LENGTH; i++) {
    message[i] = EEPROM.read(MESSAGE_ADDR + i);
  }
  message[MAX_MESSAGE_LENGTH - 1] = '\0'; // Ensure the message is null-terminated
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    if (message[i] == '\0') break;
    lcd.print(message[i]);
  }
  lcd.setCursor(0, 1);
  for (int i = 16; i < 32; i++) {
    if (message[i] == '\0') break;
    lcd.print(message[i]);
  }

  // Print initial states for debugging
  Serial.println("Initial States:");
  Serial.print("Mode: "); Serial.println(currentMode);
  Serial.print("Backlight: "); Serial.println(backlightOn);
  Serial.print("Speed: "); Serial.println(ledSpeed);
  Serial.print("Color Sequence Speed: "); Serial.println(speedCC);
  Serial.print("Rotate Speed: "); Serial.println(speedR);
  Serial.print("Color Sequence: "); Serial.println(colorSequence);
  Serial.print("Message: "); Serial.println(message);
  Serial.print("Current Preset:"); Serial.println(presetval);
}

void HandleSerial(){
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any leading or trailing whitespace

    // Check if the input is a command
    if (input.equalsIgnoreCase("O")) {
      currentMode = RAINBOW_WITH_OFFSET;
      EEPROM.write(MODE_ADDR, currentMode);
      Serial.println("Mode set to RAINBOW_WITH_OFFSET");
    } else if (input.equalsIgnoreCase("N")) {
      currentMode = RAINBOW_NO_OFFSET;
      EEPROM.write(MODE_ADDR, currentMode);
      Serial.println("Mode set to RAINBOW_NO_OFFSET");
    } else if (input.equalsIgnoreCase("B")) {
      backlightOn = !backlightOn; // Toggle backlight state
      EEPROM.write(BACKLIGHT_ADDR, backlightOn);
      setBacklight(backlightOn);
      Serial.print("Backlight state: "); Serial.println(backlightOn);
    } else if (input.startsWith("s")) {
      int newSpeed = input.substring(1).toInt();
      if (newSpeed > 0) {
        ledSpeed = newSpeed;
        EEPROM.write(SPEED_ADDR, ledSpeed);
        Serial.print("Speed set to: "); Serial.println(ledSpeed);
      }
    } else if (input.startsWith("c")) {
      colorSequence = input.substring(1);
      currentMode = COLOR_SEQUENCE;
      EEPROM.write(MODE_ADDR, currentMode);

      // Save the color sequence to EEPROM
      for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++) {
        if (i < colorSequence.length()) {
          EEPROM.write(COLOR_SEQUENCE_ADDR + i, colorSequence[i]);
        } else {
          EEPROM.write(COLOR_SEQUENCE_ADDR + i, '\0'); // Write null character to indicate end of string
        }
      }
      Serial.print("Color sequence set to: "); Serial.println(colorSequence);
    } else if (input.startsWith("R")) {
      currentMode = ROTATE_WITH_SHADOW;
      EEPROM.write(MODE_ADDR, currentMode);
      EEPROM.write(R_COLOR,input.charAt(1));
      r_color = input.charAt(1);
      rotateWithShadow(speedR, r_color);
      Serial.println("Mode set to ROTATE_WITH_SHADOW");
    }else if(input.startsWith("q")){
      int newSpeed = input.substring(1).toInt();
      if(newSpeed>0) speedCC = newSpeed;
      EEPROM.write(SPEED_CC, speedCC);
      Serial.print("Color sequence speed set to: "); Serial.println(speedCC);
    }else if(input.startsWith("r"))
    {
      int newSpeed = input.substring(1).toInt();
      if(newSpeed>0) speedR = newSpeed;
      else speedR = 50;
      EEPROM.write(SPEED_R, speedR);
      Serial.print("Rotate speed set to: "); Serial.println(speedR);
    }
    else if(input.startsWith("Y")){
      String Inp = input.substring(1);
      
      currentMode = YIN_YANG;
      EEPROM.write(MODE_ADDR, currentMode);

      int values[6];
      int index = 0;
      int start = 0;
      int end = Inp.indexOf(',');
      
      while (end != -1 && index < 6) {
        values[index] = Inp.substring(start, end).toInt();
        start = end + 1;
        end = Inp.indexOf(',', start);
        index++;
      }
      values[index] = Inp.substring(start).toInt();
      yinYang(speedR,values[0],values[1],values[2],values[3],values[4],values[5]);
      for(int i = 0; i < 6;i++){
        yingYangColors[i] = values[i];
        EEPROM.write(YIN_YANG_START_ADDR+i, values[i]);
      }
    }else if(input.equalsIgnoreCase("N")){
      if(input.length() > 1){
        presetval = input.substring(1).toInt();
      }
      else{
        presetval++;
      }
      if(presetval>maxpresetval) presetval=0;
      EEPROM.write(PRESET_VALUE, presetval);
      Serial.print("Preset Value Changed to: "); Serial.println(presetval);
    }else if(input.startsWith("~")){
      String Inp = input.substring(1);
      
      currentMode = SINGLE_LED;
      EEPROM.write(MODE_ADDR, currentMode);

      int values[3];
      int index = 0;
      int start = 0;
      int end = Inp.indexOf(',');
      
      while (end != -1 && index < 3) {
        values[index] = Inp.substring(start, end).toInt();
        start = end + 1;
        end = Inp.indexOf(',', start);
        index++;
      }
      for(int i = 0; i < 3;i++){
        EEPROM.write(SINGLE_LED_ADDR+i, values[i]);
        SingleLEDColors[i] = values[i];
      }
      SingleLED(speedR, strip.Color(values[0],values[1], values[2]));

      } else if(input.startsWith("L")){
      // Display the input on the LCD if it is not a command
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(input.substring(1,17));
      lcd.setCursor(0, 1);
      lcd.print(input.substring(17,33));


      // Save the input to EEPROM
      for (int i = 0; i < MAX_MESSAGE_LENGTH - 1; i++) {
        if (i < input.length()) {
          EEPROM.write(MESSAGE_ADDR + i, input[i+1]);
        } else {
          EEPROM.write(MESSAGE_ADDR + i, '\0'); // Write null character to indicate end of string
        }
      }
      Serial.print("Message set to: "); Serial.println(input.substring(1));
    }
  }

  if(!digitalRead(7)){
    while(!digitalRead(7)) delay(50);
    presetval++;
    if(presetval>maxpresetval) presetval=0;
    EEPROM.write(PRESET_VALUE, presetval);
    Serial.print("Preset Value Changed to: "); Serial.println(presetval);
  }
  delay(1);
}

void loop() {
  
  HandleSerial();

  switch (presetval){
    case 0:
      if (currentMode == RAINBOW_WITH_OFFSET) {
        rainbowWithOffset(ledSpeed);
      } else if (currentMode == RAINBOW_NO_OFFSET) {
        rainbowNoOffset(ledSpeed);
      } else if (currentMode == COLOR_SEQUENCE) {
        displayColorSequence(colorSequence, speedCC * 10);
      } else if (currentMode == ROTATE_WITH_SHADOW) {
        rotateWithShadow(speedR, r_color);
      }else if(currentMode == YIN_YANG){
        yinYang(speedR,yingYangColors[0],yingYangColors[1],yingYangColors[2],yingYangColors[3],yingYangColors[4],yingYangColors[5]);
      }
      break;
    case 1:
      rainbowWithOffset(ledSpeed);
      break;
    case 2:
      rainbowNoOffset(ledSpeed);
      break;
    case 3:
      displayColorSequence(colorSequence, speedCC * 10);
      break;
    case 4:
      displayColorSequence("r,b", speedCC * 10);
      break;
    case 5:
      rotateWithShadow(speedR, 'r');
      break;
    case 6:
      rotateWithShadow(speedR, 'g');
      break;
    case 7:
      rotateWithShadow(speedR, 'b');
      break;
    case 8:
      rotateWithShadow(speedR, 'c');
      break;
    case 9:
      rotateWithShadow(speedR, 'f');
      break;
    case 10:
      yinYang(speedR,yingYangColors[0],yingYangColors[1],yingYangColors[2],yingYangColors[3],yingYangColors[4],yingYangColors[5]);
      break;
    case 11:
      yinYang(speedR,255,0,0,0,0,255);
      break;
    case 12:
      rotateWithShadow(speedR, 'r');
      rotateWithShadow(speedR, 'g');
      rotateWithShadow(speedR, 'b');
      break;
    case 13:
      SingleLED(speedR, strip.Color(255,0,0));
      break;
    case 14:
      SingleLED(speedR, strip.Color(0,255,0));
      break;
    case 15:
      SingleLED(speedR, strip.Color(0,0,255));
      break;
    //increase the maxpresetval variable if adding more presets
  }
  HandleSerial();
}

// Set the backlight state
void setBacklight(bool state) {
  if (state) {
    digitalWrite(BACKLIGHT_PIN, HIGH); // Turn on backlight
  } else {
    digitalWrite(BACKLIGHT_PIN, LOW); // Turn off backlight
  }
}

// Display rainbow colors across all pixels with an offset
void rainbowWithOffset(int wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) { // 5 cycles of all colors on the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i * 256 / strip.numPixels() + j) & 255));
    }
    strip.show();
    delay(wait);
    HandleSerial();
  }
}

// Display rainbow colors across all pixels without an offset
void rainbowNoOffset(int wait) {
  uint16_t j;

  for (j = 0; j < 256; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
    HandleSerial();
  }
}

// Generate rainbow colors across 0-255 positions
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

// Convert a color code to an RGB value
uint32_t getColorFromCode(String code) {
  if (code.equalsIgnoreCase("r")) {
    return strip.Color(255, 0, 0); // Red
  } else if (code.equalsIgnoreCase("g")) {
    return strip.Color(0, 255, 0); // Green
  } else if (code.equalsIgnoreCase("b")) {
    return strip.Color(0, 0, 255); // Blue
  } else if (code.equalsIgnoreCase("y")) {
    return strip.Color(255, 255, 0); // Yellow
  } else if (code.equalsIgnoreCase("w")) {
    return strip.Color(255, 255, 255); // White
  } else if (code.equalsIgnoreCase("bl")) {
    return strip.Color(0, 0, 0); // Black (off)
  }
  return strip.Color(0, 0, 0); // Default to black if no match
}

// Display the colors in sequence based on the input list
void displayColorSequence(String sequence, int wait) {
  if (sequence.length() == 0) return; // No sequence to display

  int start = 0;
  int end = sequence.indexOf(',');

  while (end != -1) {
    String colorCode = sequence.substring(start, end);
    uint32_t color = getColorFromCode(colorCode);
    fillStripColor(color);
    delay(wait);

    start = end + 1;
    end = sequence.indexOf(',', start);
    HandleSerial();
  }

  // Display the last color in the sequence
  String colorCode = sequence.substring(start);
  uint32_t color = getColorFromCode(colorCode);
  fillStripColor(color);
  delay(wait);
  HandleSerial();
}

// Fill the entire strip with a single color
void fillStripColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Display a rotating LED with shadow effect
void rotateWithShadow(int wait, char color) {
  uint32_t Colors[12];
  uint32_t shadowColor = 0;
  uint32_t trailingColor = 0;

  float d = 1.5;
  // Determine colors based on the color code
  if (color == 'r') {
    int r = 255;
    int g = 0;
    int b = 0;
    for(int i = 12; i > 0; i--){
      if(i<12)r=r/d;
      if(i<12)g=g/d;
      if(i<12)b=b/d;
      Colors[i] = strip.Color(r,g,b);
    }
  } else if (color == 'g') {
    int r = 0;
    int g = 255;
    int b = 0;
    for(int i = 12; i > 0; i--){
      if(i<12)r=r/d;
      if(i<12)g=g/d;
      if(i<12)b=b/d;
      Colors[i] = strip.Color(r,g,b);
    }
  } else if (color == 'b') {
    int r = 0;
    int g = 0;
    int b = 255;
    for(int i = 12; i > 0; i--){
      if(i<12)r=r/d;
      if(i<12)g=g/d;
      if(i<12)b=b/d;
      Colors[i] = strip.Color(r,g,b);
    }
  } else if(color == 'c'){
    int r = 223;
    int g = 0;
    int b = 253;
    for(int i = 12; i > 0; i--){
      if(i<12)r=r/d;
      if(i<12)g=g/d;
      if(i<12)b=b/d;
      Colors[i] = strip.Color(r,g,b);
    }
  }
  else {
    int r = 255;
    int g = 255;
    int b = 255;
    for(int i = 12; i > 0; i--){
      if(i<12)r=r/d;
      if(i<12)g=g/d;
      if(i<12)b=b/d;
      Colors[i] = strip.Color(r,g,b);
      //Serial.print(r);Serial.print(",");Serial.print(g); Serial.print(","); Serial.println(b);
    } // Invalid color code, do nothing
  }

  int j = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.clear(); // Clear all LEDs

    for(int k = 0; k < 12; k++){
      j= k+i;
      if(k+i > 11)j=(k+i)-12;
      strip.setPixelColor(j,Colors[k+1]);
    }
    HandleSerial();
    strip.show();
    delay(wait);
    HandleSerial();
  }
  HandleSerial();
}

void yinYang(int wait, int r, int g, int b,int r2,int g2,int b2) {
  
  uint32_t mainColor1 = strip.Color(r,g,b);
  uint32_t shadowColor1 = strip.Color(r/2,g/2,b/2);
  uint32_t trailingColor1 = strip.Color(r/2/2,g/2/2,b/2/2);
  uint32_t trailingColor12 = strip.Color(r/2/2/2,g/2/2/2,b/2/2/2);
  uint32_t trailingColor13 = strip.Color(r/2/2/2/2,g/2/2/2/2,b/2/2/2/2);
  uint32_t trailingColor14 = strip.Color(r/2/2/2/2/2,g/2/2/2/2/2,b/2/2/2/2/2);

  uint32_t mainColor2 = strip.Color(r2,g2,b2);
  uint32_t shadowColor2 = strip.Color(r2/2,g2/2,b2/2);
  uint32_t trailingColor2 = strip.Color(r2/2/2,g2/2/2,b2/2/2);
  uint32_t trailingColor22 = strip.Color(r2/2/2/2,g2/2/2/2,b2/2/2/2);
  uint32_t trailingColor23 = strip.Color(r2/2/2/2/2,g2/2/2/2/2,b2/2/2/2/2);
  uint32_t trailingColor24 = strip.Color(r2/2/2/2/2/2,g2/2/2/2/2/2,b2/2/2/2/2/2);

  int j = 0;
  int k = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.clear(); // Clear all LEDs
    j=i;
    k = i + 6;
    // Set the main LED to the specified color
    strip.setPixelColor(i, mainColor1);

    if(k>12)k = k - 12;
    strip.setPixelColor(k, mainColor2);
    
    if(i < 2) j = i + 12;
    // Set the previous LED to a dimmer color to create the shadow effect
    strip.setPixelColor(j - 1, shadowColor1);
    strip.setPixelColor(j - 2, trailingColor1);
    strip.setPixelColor(j - 3, trailingColor12);
    strip.setPixelColor(j - 4, trailingColor13);
    strip.setPixelColor(j - 5, trailingColor14);

    if(k<2)k = k + 12;
    strip.setPixelColor(k - 1, shadowColor2);
    strip.setPixelColor(k - 2, trailingColor2);
    strip.setPixelColor(k - 3, trailingColor22);
    strip.setPixelColor(k - 4, trailingColor23);
    strip.setPixelColor(k - 4, trailingColor24);

    strip.show();
    delay(wait);
    HandleSerial();
  }
}

void SingleLED(int wait, uint32_t color){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.clear();
    strip.setPixelColor(i,color);
    strip.show();
    delay(wait);
  }
}