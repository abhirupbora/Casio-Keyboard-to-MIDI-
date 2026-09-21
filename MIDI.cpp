// Casio SA-21 to USB MIDI Converter (Uno Optimized)
const int numRows = 8;
const int numCols = 4;

// Output pins (Rows)
const int rowPins[numRows] = {2, 9, 4, 6, 13, 12, 8, 3};
// Input pins (Columns)
const int colPins[numCols] = {10, 5, 11, 7};

// The 32 keys mapped to actual MIDI note numbers (Starting at F2 = 41)
const int midiMap[numRows][numCols] = {
  {41, 49, 57, 65}, // Row Pin 2
  {42, 50, 58, 66}, // Row Pin 9
  {43, 51, 59, 67}, // Row Pin 4
  {44, 52, 60, 68}, // Row Pin 6
  {45, 53, 61, 69}, // Row Pin 13
  {46, 54, 62, 70}, // Row Pin 12
  {47, 55, 63, 71}, // Row Pin 8
  {48, 56, 64, 72}  // Row Pin 3
};

bool currentKeyState[numRows][numCols];

void setup() {
  Serial.begin(115200);
  for (int r = 0; r < numRows; r++) pinMode(rowPins[r], INPUT);
  for (int c = 0; c < numCols; c++) pinMode(colPins[c], INPUT_PULLUP);
  
  for (int r = 0; r < numRows; r++) {
    for (int c = 0; c < numCols; c++) {
      currentKeyState[r][c] = false;
    }
  }
}

void loop() {
  for (int r = 0; r < numRows; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    
    // Crucial hardware debounce: wait 10 microseconds for the voltage to settle
    delayMicroseconds(10);
    
    for (int c = 0; c < numCols; c++) {
      bool isPressed = (digitalRead(colPins[c]) == LOW);
      if (isPressed != currentKeyState[r][c]) {
        currentKeyState[r][c] = isPressed;
        if (isPressed) {
          sendMIDI(0x90, midiMap[r][c], 127); // Note ON
        } else {
          sendMIDI(0x80, midiMap[r][c], 0);   // Note OFF
        }
        // Tiny 2ms delay prevents bouncy rubber pads from double-triggering
        delay(2);
      }
    }
    pinMode(rowPins[r], INPUT);
  }
}

void sendMIDI(byte command, byte data1, byte data2) {
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}