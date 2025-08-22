#define SENSOR_PIN 34

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
}

void loop() {
  int sensorValue = analogRead(SENSOR_PIN);
  float voltage = sensorValue * (3.3 / 4095.0);

  // Serial.print("Valor analogico ");
  // Serial.print(" Tensao ");
  // Serial.println(voltage);

  if (sensorValue > 650) {
    Serial.print("Ambiente está barulhento ");
    Serial.println(sensorValue);
  } else {
    Serial.print("Ambiente está tranquilo ");
    Serial.println(sensorValue);
  }

  delay(3000);
}