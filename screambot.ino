// Motor pins
const int MOTOR_PIN_1 = 6;
const int MOTOR_PIN_2 = 9;

// Microphone pin
const int MIC_PIN = 2;

// PWM properties
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 0;  // Start with motors off

void setup() {
  Serial.begin(115200);

  // Configure PWM for both motors
  ledcAttach(MOTOR_PIN_1, freq, resolution);
  ledcAttach(MOTOR_PIN_2, freq, resolution);

  // Configure microphone pin
  pinMode(MIC_PIN, INPUT);

  Serial.println("Starting sound-controlled motors...");
}

void loop() {
  // Read microphone value
  int micValue = analogRead(MIC_PIN);

  // Map microphone value (0-4095) to motor speed (0-255)
  // The louder the sound, the faster the motors

  if (micValue < 3000) {
    dutyCycle = 0;
  } else {
    dutyCycle = map(micValue, 0, 4095, 0, 255);

    // Constrain duty cycle to prevent erratic behavior
    dutyCycle = constrain(dutyCycle, 0, 255);

  }

  // Set motor speeds
  ledcWrite(MOTOR_PIN_1, dutyCycle);
  ledcWrite(MOTOR_PIN_2, dutyCycle);

  // Print debug info
  Serial.print("Mic Value: ");
  Serial.print(micValue);
  Serial.print(" | Motor Speed: ");
  Serial.println(dutyCycle);

  // Small delay to prevent overwhelming the serial output
  delay(50);
}
