#define SENSOR_PIN 34
#define LED_ALTO 18
#define LED_MEDIO 19
#define LED_BAIXO 21

const int LIMITE_BAIXO = 90;
const int LIMITE_ALTO = 200;

unsigned long inicioPeriodo = 0;
const unsigned long intervalo = 10000;

long somaAmplitudes = 0;
int contador = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(LED_BAIXO, OUTPUT);
  pinMode(LED_MEDIO, OUTPUT);
  pinMode(LED_ALTO, OUTPUT);
  inicioPeriodo = millis();
}

void loop() {
  int valorMin = 4095;
  int valorMax = 0;

  unsigned long inicioJanela = millis();
  while (millis() - inicioJanela < 200) {
    int leitura = analogRead(SENSOR_PIN);
    if (leitura < valorMin) valorMin = leitura;
    if (leitura > valorMax) valorMax = leitura;
  }

  int amplitude = valorMax - valorMin;
  somaAmplitudes += amplitude;
  contador++;

  if (millis() - inicioPeriodo >= intervalo) {
    int mediaAmplitude = somaAmplitudes / contador;

    Serial.print("Média da amplitude em 10s: ");
    Serial.print(mediaAmplitude);
    Serial.print(" -> ");

    if (mediaAmplitude < LIMITE_BAIXO) {
      Serial.println("BAIXO");
      digitalWrite(LED_BAIXO, HIGH);
      delay(500);
      digitalWrite(LED_BAIXO, LOW);
    } else if (mediaAmplitude < LIMITE_ALTO) {
      Serial.println("MÉDIO");
      digitalWrite(LED_MEDIO, HIGH);
      delay(500);
      digitalWrite(LED_MEDIO, LOW);
    } else {
      Serial.println("ALTO");
      digitalWrite(LED_ALTO, HIGH);
      delay(500);
      digitalWrite(LED_ALTO, LOW);
    }

    somaAmplitudes = 0;
    contador = 0;
    inicioPeriodo = millis();
  }
}
