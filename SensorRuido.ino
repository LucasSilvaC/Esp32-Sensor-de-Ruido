#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <sys/time.h>

const char* ssid = "SCRM-05";
const char* password = "12345678";

WebServer server(80);

#define SENSOR_PIN 34
#define LED_ALTO 18
#define LED_MEDIO 19
#define LED_BAIXO 21

const int LIMITE_BAIXO = 100;
const int LIMITE_ALTO = 200;
const unsigned long INTERVALO = 10000; 

unsigned long inicioPeriodo = 0;
long somaAmplitudes = 0;
int contador = 0;
int ultimaMedia = 67; 

#define MAX_REGISTROS 500

struct Registro {
  int mediaAmplitude;
  String dataHora;
  String status;
};

Registro registros[MAX_REGISTROS];
int registroIndex = 0;
int totalRegistros = 0;

void setLocalTimeFromBuild() {
  struct tm tm_build = {0};
  strptime(__DATE__ " " __TIME__, "%b %d %Y %H:%M:%S", &tm_build);
  time_t t = mktime(&tm_build);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

String getCircleClass(String status) {
  if (status == "ALTO") return "circle-orange";
  if (status == "MÉDIO") return "circle-pink";
  return "circle-blue";
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="5">
  <title>Nível de Ruído - SSID 05</title>
  <style>
    body {
      font-family: 'Inter', sans-serif;
      background: linear-gradient(to bottom right, #eff6ff, #e0e7ff);
      margin: 0;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      padding: 20px 0;
    }

    .card {
      width: 90%;
      max-width: 450px;
      background: linear-gradient(to bottom right, #dbeafe, #eef2ff);
      border-radius: 1.2rem;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.15);
      border: 1px solid #bfdbfe;
      overflow: hidden;
    }

    .card-header {
      background: linear-gradient(to right, #2563eb, #4f46e5);
      color: white;
      padding: 1.5rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .card-header h1 {
      margin: 0;
      font-size: 1.5rem;
      font-weight: 700;
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }

    .card-header svg {
      width: 1.5rem;
      height: 1.5rem;
    }

    .card-header p {
      margin: 0;
      font-size: 0.85rem;
      color: #bfdbfe;
    }

    .icon-bg {
      background: rgba(255, 255, 255, 0.25);
      padding: 0.75rem;
      border-radius: 50%;
    }

    .content {
      padding: 1.5rem;
      text-align: center;
    }

    .circle {
      width: 130px;
      height: 130px;
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      margin: 0 auto 1rem auto;
      box-shadow: 0 8px 15px rgba(99,102,241,0.3);
    }

    .circle-blue {
      background: linear-gradient(to bottom right, #3b82f6, #6366f1);
    }

    .circle-pink {
      background: linear-gradient(to bottom right, #ec4899, #f472b6);
      box-shadow: 0 8px 15px rgba(236, 72, 153, 0.3);
    }

    .circle-orange {
      background: linear-gradient(to bottom right, #f97316, #fb923c);
      box-shadow: 0 8px 15px rgba(249, 115, 22, 0.3);
    }

    .circle span {
      color: white;
      font-size: 2rem;
      font-weight: 700;
    }

    .subtitle {
      color: #475569;
      font-weight: 500;
      margin-bottom: 1.5rem;
    }

    .status {
      display: flex;
      justify-content: center;
      margin-bottom: 1rem;
    }

    .status-box {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      background: #dcfce7;
      color: #166534;
      padding: 0.5rem 1rem;
      border-radius: 9999px;
      font-weight: 500;
      font-size: 0.9rem;
    }

    .status-dot {
      width: 0.5rem;
      height: 0.5rem;
      background: #22c55e;
      border-radius: 50%;
      animation: pulse 1.5s infinite;
    }

    @keyframes pulse {

      0%,
      100% {
        opacity: 0.5;
        transform: scale(0.9);
      }

      50% {
        opacity: 1;
        transform: scale(1.2);
      }
    }

    .description {
      background: rgba(255, 255, 255, 0.6);
      border: 1px solid #bfdbfe;
      border-radius: 0.75rem;
      padding: 1rem;
      color: #334155;
      font-size: 0.9rem;
      line-height: 1.4;
      margin-bottom: 1rem;
    }

    .updates-section {
      margin-top: 2rem;
      text-align: left;
    }

    .updates-title {
      color: #475569;
      font-weight: 600;
      margin-bottom: 1rem;
      text-align: center;
      font-size: 1.1rem;
    }

    .update-item {
      background: rgba(255, 255, 255, 0.7);
      border: 1px solid #e2e8f0;
      border-radius: 0.75rem;
      padding: 0.75rem;
      margin-bottom: 0.5rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 0.85rem;
    }

    .update-value {
      font-weight: 600;
      color: #1e293b;
    }

    .update-time {
      color: #64748b;
      font-size: 0.75rem;
    }

    .update-status {
      padding: 0.25rem 0.5rem;
      border-radius: 0.5rem;
      font-size: 0.7rem;
      font-weight: 600;
    }

    .status-baixo {
      background: #dbeafe;
      color: #1e40af;
    }

    .status-medio {
      background: #fce7f3;
      color: #be185d;
    }

    .status-alto {
      background: #fed7aa;
      color: #c2410c;
    }

    .download-btn {
      background: linear-gradient(to right, #2563eb, #4f46e5);
      color: white;
      border: none;
      padding: 0.75rem 1.5rem;
      border-radius: 0.75rem;
      font-weight: 600;
      cursor: pointer;
      margin-top: 1rem;
      width: 100%;
      transition: all 0.3s;
    }

    .download-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(37, 99, 235, 0.4);
    }

    .footer {
      font-size: 0.75rem;
      color: #6b7280;
      margin-top: 30px;
    }
  </style>
</head>

<body>
  <div class="card">
    <div class="card-header">
      <div>
        <h1>
          <svg fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
              d="M19 11a7 7 0 01-7 7m0 0a7 7 0 01-7-7m7 7v4m0 0H8m4 0h4m-4-8a3 3 0 01-3-3V5a3 3 0 116 0v6a3 3 0 01-3 3z" />
          </svg>
          Nível de Ruído
        </h1>
        <p>Sensor IoT em tempo real</p>
      </div>
      <div class="icon-bg">
        <svg fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
            d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
        </svg>
      </div>
    </div>

    <div class="content">
      <div class="circle )rawliteral";
  
  if (totalRegistros > 0) {
    int ultimoIdx = (registroIndex - 1 + MAX_REGISTROS) % MAX_REGISTROS;
    html += getCircleClass(registros[ultimoIdx].status);
  } else {
    html += "circle-blue";
  }
  
  html += R"rawliteral(">
        <span>)rawliteral";
  
  if (totalRegistros > 0) {
    int ultimoIdx = (registroIndex - 1 + MAX_REGISTROS) % MAX_REGISTROS;
    html += String(registros[ultimoIdx].mediaAmplitude);
  } else {
    html += String(ultimaMedia);
  }
  
  html += R"rawliteral(</span>
      </div>
      <p class="subtitle">Nível de ruído</p>

      <div class="status">
        <div class="status-box">
          <div class="status-dot"></div>
          <span>Ativo</span>
        </div>
      </div>

      <div class="description">
        Monitoramento de ruído ambiente em tempo real para controle de qualidade e conforto acústico da escola e
        faculdade Senai Roberto Mange.
      </div>

      <div class="updates-section">
        <div class="updates-title">Últimas atualizações</div>
        )rawliteral";

  int numUpdates = min(totalRegistros, 5);
  if (numUpdates > 0) {
    for (int i = 0; i < numUpdates; i++) {
      int idx = (registroIndex - 1 - i + MAX_REGISTROS) % MAX_REGISTROS;
      html += "<div class='update-item'>";
      html += "<div>";
      html += "<div class='update-value'>" + String(registros[idx].mediaAmplitude) + " Nível Ruído</div>";
      html += "<div class='update-time'>" + registros[idx].dataHora + "</div>";
      html += "</div>";
      html += "<div class='update-status ";
      
      if (registros[idx].status == "BAIXO") html += "status-baixo";
      else if (registros[idx].status == "MÉDIO") html += "status-medio";
      else html += "status-alto";
      
      html += "'>" + registros[idx].status + "</div>";
      html += "</div>";
    }
  } else {
    html += "<div style='text-align: center; color: #64748b; font-size: 0.9rem;'>Nenhuma atualização disponível</div>";
  }

  html += R"rawliteral(
      </div>

      <button class="download-btn" onclick="window.location.href='/download'">
        📥 Baixar dados completos (CSV)
      </button>

      <div style="margin-top: 2rem;">
        <h3 style="color: #475569; margin-bottom: 1rem; text-align: center;">Responsáveis</h3>
        <div style="display: flex; justify-content: space-around; align-items: center; margin-bottom: 1rem;">
          <div class="circle circle-pink" style="width: 50px; height: 50px; margin: 0;">
            <span style="font-size: 1.4rem;">M</span>
          </div>
          <div class="circle circle-orange" style="width: 50px; height: 50px; margin: 0;">
            <span style="font-size: 1.4rem;">L</span>
          </div>
        </div>
        <div style="display: flex; justify-content: space-around; color: #64748b; font-size: 0.7rem;">
          <span>Maria Eduarda</span>
          <span>Lucas Camargo</span>
        </div>
      </div>

      <div class="footer">
        Última atualização: )rawliteral";
  
  if (totalRegistros > 0) {
    int ultimoIdx = (registroIndex - 1 + MAX_REGISTROS) % MAX_REGISTROS;
    html += registros[ultimoIdx].dataHora;
  } else {
    html += "N/A";
  }
  
  html += R"rawliteral(
      </div>
    </div>
  </div>
</body>
</html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleDownload() {
  String csv = "Data/Hora,Media da Amplitude (10s),Status\n";
  if (totalRegistros > 0) {
    int start = (registroIndex - totalRegistros + MAX_REGISTROS) % MAX_REGISTROS;
    for (int i = 0; i < totalRegistros; i++) {
      int idx = (start + i) % MAX_REGISTROS;
      csv += registros[idx].dataHora + "," + String(registros[idx].mediaAmplitude) + "," + registros[idx].status + "\n";
    }
  }
  server.sendHeader("Content-Type", "text/csv");
  server.sendHeader("Content-Disposition", "attachment; filename=\"ruido_medicoes.csv\"");
  server.send(200, "text/csv", csv);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(LED_ALTO, OUTPUT);
  pinMode(LED_MEDIO, OUTPUT);
  pinMode(LED_BAIXO, OUTPUT);

  Serial.println("🔄 Iniciando Access Point...");
  WiFi.softAP(ssid, password);
  Serial.print("✅ SSID: "); Serial.println(ssid);
  Serial.print("📡 IP do ESP32 (AP): "); Serial.println(WiFi.softAPIP());

  setLocalTimeFromBuild();

  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  server.begin();
  Serial.println("🌐 Servidor web iniciado.");

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

  if (millis() - inicioPeriodo >= INTERVALO) {
    int mediaAmplitude = somaAmplitudes / contador;
    somaAmplitudes = 0;
    contador = 0;
    inicioPeriodo = millis();

    String dataHora = "N/A";
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    if (timeinfo) {
      char timeStringBuff[64];
      strftime(timeStringBuff, sizeof(timeStringBuff), "%d/%m/%Y %H:%M:%S", timeinfo);
      dataHora = String(timeStringBuff);
    }

    String status;
    if (mediaAmplitude < LIMITE_BAIXO) {
      status = "BAIXO";
    } else if (mediaAmplitude < LIMITE_ALTO) {
      status = "MÉDIO";
    } else {
      status = "ALTO";
    }

    registros[registroIndex].mediaAmplitude = mediaAmplitude;
    registros[registroIndex].dataHora = dataHora;
    registros[registroIndex].status = status;
    registroIndex = (registroIndex + 1) % MAX_REGISTROS;
    if (totalRegistros < MAX_REGISTROS) totalRegistros++;

    ultimaMedia = mediaAmplitude;

    Serial.print("📊 Média da amplitude (10s): ");
    Serial.print(mediaAmplitude);
    Serial.print(" → ");
    Serial.println(status);

    if (status == "BAIXO") {
      digitalWrite(LED_BAIXO, HIGH);
      delay(500);
      digitalWrite(LED_BAIXO, LOW);
    } else if (status == "MÉDIO") {
      digitalWrite(LED_MEDIO, HIGH);
      delay(500);
      digitalWrite(LED_MEDIO, LOW);
    } else {
      digitalWrite(LED_ALTO, HIGH);
      delay(500);
      digitalWrite(LED_ALTO, LOW);
    }
  }

  server.handleClient();
}
