// #define SENSOR_PIN 34
// #define LED_ALTO 18
// #define LED_MEDIO 19
// #define LED_BAIXO 21

// const int LIMITE_BAIXO = 800;  
// const int LIMITE_ALTO = 2000; 

// void setup() {
//   Serial.begin(115200);
//   pinMode(SENSOR_PIN, INPUT);
//   analogReadResolution(12);

//   pinMode(LED_BAIXO, OUTPUT);
//   pinMode(LED_MEDIO, OUTPUT);
//   pinMode(LED_ALTO, OUTPUT);
// }

// void loop() {
//   long sum = 0;
//   const int readings = 10;
  
//   for(int i = 0; i < readings; i++) {
//     sum += analogRead(SENSOR_PIN);
//     delay(20);
//   }
  
//   int sensorValue = sum / readings;

//   Serial.print("Valor: ");
//   Serial.print(sensorValue);
//   Serial.print("/4095 -> ");

//   if (sensorValue < LIMITE_BAIXO) {
//     Serial.println("BAIXO");
//     digitalWrite(LED_BAIXO, HIGH);
//     delay(500);   
//     digitalWrite(LED_BAIXO, LOW);

//   } else if (sensorValue < LIMITE_ALTO) {
//     Serial.println("MÉDIO");
//     digitalWrite(LED_MEDIO, HIGH);
//     delay(500);
//     digitalWrite(LED_MEDIO, LOW);


//   } else {
//     Serial.println("ALTO");
//     digitalWrite(LED_ALTO, HIGH);
//     delay(500);
//     digitalWrite(LED_ALTO, LOW);
//   }

//   delay(500); 
// }


// Média com 10s:
#define SENSOR_PIN 34
#define LED_ALTO 18
#define LED_MEDIO 19
#define LED_BAIXO 21

const int LIMITE_BAIXO = 800;  
const int LIMITE_ALTO = 1600; 

unsigned long inicioPeriodo = 0;
const unsigned long intervalo = 10000;

long soma = 0;
int contador = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  analogReadResolution(12);

  pinMode(LED_BAIXO, OUTPUT);
  pinMode(LED_MEDIO, OUTPUT);
  pinMode(LED_ALTO, OUTPUT);

  inicioPeriodo = millis(); 
}

void loop() {
  soma += analogRead(SENSOR_PIN);
  contador++;
  delay(50);

  if (millis() - inicioPeriodo >= intervalo) {
    int media = soma / contador;

    Serial.print("Média em 10s: ");
    Serial.print(media);
    Serial.print(" /4095 -> ");

    if (media < LIMITE_BAIXO) {
      Serial.println("BAIXO");
      digitalWrite(LED_BAIXO, HIGH);
      delay(1000);
      digitalWrite(LED_BAIXO, LOW);

    } else if (media < LIMITE_ALTO) {
      Serial.println("MÉDIO");
      digitalWrite(LED_MEDIO, HIGH);
      delay(1000);
      digitalWrite(LED_MEDIO, LOW);

    } else {
      Serial.println("ALTO");
      digitalWrite(LED_ALTO, HIGH);
      delay(1000);
      digitalWrite(LED_ALTO, LOW);
    }

    soma = 0;
    contador = 0;
    inicioPeriodo = millis();
  }
}
