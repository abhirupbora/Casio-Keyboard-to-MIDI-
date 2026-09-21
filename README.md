Casio SA-21 to Zero-Latency USB MIDI ControllerWelcome to the SleepyLimee_Studios hardware repository! This project revives a vintage 30-year-old Casio SA-21 keyboard by bypassing its failing original motherboard and converting its raw key matrix into a zero-latency USB MIDI controller using an Arduino Uno.   Whether you are routing MIDI through a lightweight Tiny 11 build on a Lenovo E41-25 or plugging directly into GarageBand on a MacBook Neo, this guide covers the entire hardware and software pipeline.🛠 Components NeededVintage Casio SA-21 Keyboard (or similar matrix-based keyboard)Arduino Uno (or clone)   Jumper wires for connecting the matrix to the microcontroller   Soldering iron (or conductive wire glue / copper foil tape for solderless connections)   🧩 Step 1: Understanding the Switch MatrixVintage keyboards do not use a separate wire for every piano key. Instead, they use a Switch Matrix—a grid of Rows and Columns. The SA-21 relies on exactly an 8x4 matrix (8 rows × 4 columns = 32 keys).   The Diode Trap: The original Casio circuitry utilizes diodes (like the D210/D211 glass tubes) that act as one-way valves for electricity.   The Solution: If you scan the matrix in the wrong direction, the electricity hits a wall. The Matrix_Scanner.ino code checks pins in both directions to ensure the diodes don't block the signal.   📡 Step 2: Mapping the Keys (The Scanner Code)DANGER: Never use Pin 0 (RX) or Pin 1 (TX) on the Arduino Uno, as these are hardwired to the USB chip and will scramble your communication.   Upload the following code to your Arduino Uno to safely map your physical wire connections to the digital pins. Open the Arduino IDE Serial Monitor at a 115200 baud rate. Press each key strictly from left to right to log the data.   C++// Arduino Uno Matrix Scanner
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
🎹 Step 3: The Final MIDI Controller CodeOnce your matrix is mapped, upload the final optimized MIDI code. This script uses a 2D array to track key states and prevents the Uno's serial buffer from crashing by tracking microscopic hardware bounces.   Note: Analog joystick reads were intentionally removed from this version to keep the serial transmission ultra-lightweight and prevent Hairless MIDI from crashing on Windows.   C++// Casio SA-21 to USB MIDI Converter (Uno Optimized)
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
💻 Step 4: Software RoutingArduino Uno clones utilize a CH340 chip that communicates as a Serial COM Port, not a native USB musical instrument. You must bridge this connection using software.   For Windows Users:Install loopMIDI to create a virtual, invisible MIDI cable inside your computer.   Install Hairless MIDI-to-Serial Bridge.   Ensure the Arduino IDE Serial Monitor is completely closed (otherwise the COM port gets blocked).   In Hairless MIDI, set the input to your Arduino COM Port, the output to your loopMIDI port, and check "Serial <-> MIDI Bridge On".   For macOS (Apple Silicon) Users:
Modern M-Series Macs block the original Hairless MIDI software because it is an unsupported 32-bit application.   Activate the Mac's built-in virtual cable by opening Audio MIDI Setup, clicking Window > Show MIDI Studio, double-clicking the IAC Driver, and checking "Device is online".   Download EA Serial MIDI Bridge (by Ezequiel Abregu), which is a modern rewrite built for Apple Silicon.   Security Bypass: Right-click the app and select "Open" to bypass the Gatekeeper warning.   Set the Serial Port to your Arduino (e.g., /dev/cu.usbserial...), output to the IAC Driver, and hit Start. GarageBand will automatically detect the input.   🐛 TroubleshootingError: resp=0x00 (Arduino Lockup): The Arduino Uno can freeze if you attempt to upload new code while Hairless MIDI is holding the COM port hostage. Uncheck the bridge in Hairless, hold the physical RESET button on the Arduino, click "Upload" in the IDE, and release the button the exact millisecond the text changes to "Uploading...". 
