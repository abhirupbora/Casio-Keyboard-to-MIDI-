// Arduino Uno Matrix Scanner
// Matches the exact 14 pins used in this build
int pins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1}; 
int numPins = 14; 

void setup() {
  Serial.begin(115200); 
  // Set all pins to a safe INPUT_PULLUP state
  for(int i = 0; i < numPins; i++) {
    pinMode(pins[i], INPUT_PULLUP);
  }
  Serial.println("Uno Matrix Scanner Ready! Press a key...");
}

void loop() {
  for(int i = 0; i < numPins; i++) {
    // Turn one pin into an output and send a LOW signal
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
    
    // Listen on ALL other pins for that signal (Scanning both directions bypasses the diode trap)
    for(int j = 0; j < numPins; j++) {
      if (i == j) continue; 
      
      if(digitalRead(pins[j]) == LOW) {
        Serial.print("KEY PRESSED! Output Pin: ");
        Serial.print(pins[i]);
        Serial.print(" | Input Pin: ");
        Serial.println(pins[j]);
        delay(300); // 300ms delay prevents screen spamming
      }
    }
    pinMode(pins[i], INPUT_PULLUP);
  }
}