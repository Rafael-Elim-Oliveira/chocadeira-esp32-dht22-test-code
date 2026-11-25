#include <DHT.h>

// ==========================================
// MAPEAMENTO DE PINOS (RNF03, RNF04 e Premissas)
// ==========================================
#define PINO_DHT        4   // Entrada digital do sensor DHT22 (RNF03)
#define PINO_AQUECEDOR  18  // Saída digital - Relé de Estado Sólido / LED (RNF04)
#define PINO_RESFRIADOR 19  // Saída digital - MOSFET / LED (RNF04)
#define PINO_REDE_ELETRICA 21 // Chave deslizante simulando energia principal (Premissa / RNF07)

// Configuração do Sensor
#define TIPO_DHT DHT22
DHT dht(PINO_DHT, TIPO_DHT);

// ==========================================
// PARÂMETROS DE CONTROLE E ALERTAS
// ==========================================
const float TEMP_MIN_IDEAL = 37.5; // Limite mínimo desejável (RF04)
const float TEMP_MAX_IDEAL = 38.0; // Limite máximo desejável (RF04)

const float TEMP_ALERTA_BAIXA = 27.0; // Limite crítico inferior (RF03)
const float TEMP_ALERTA_ALTA  = 39.0; // Limite crítico superior (RF03)

// Controle de Tempo não bloqueante (RF01, RNF01, RNF02)
unsigned long tempoUltimaLeitura = 0;
const unsigned long INTERVALO_LEITURA = 5000; // 5 segundos

// Controle de envio de e-mail para evitar spam repetitivo no mesmo estado crítico
bool alertaCriticoAtivo = false;
bool modoNobreakAtivo = false;

void setup() {
  Serial.begin(115200);
  
  // Inicializa o sensor DHT22
  dht.begin();

  // Configura os pinos dos atuadores como saída
  pinMode(PINO_AQUECEDOR, OUTPUT);
  pinMode(PINO_RESFRIADOR, OUTPUT);
  
  // Configura a chave da rede elétrica com resistor pull-up interno
  // HIGH = Rede Elétrica Normal | LOW = Falha na Rede (Operando no Nobreak)
  pinMode(PINO_REDE_ELETRICA, INPUT_PULLUP);

  // Garante estado inicial desligado dos atuadores
  digitalWrite(PINO_AQUECEDOR, LOW);
  digitalWrite(PINO_RESFRIADOR, LOW);

  Serial.println("==================================================");
  Serial.println("   SISTEMA DE SENSORIAMENTO DE CHOCADEIRA INICIADO");
  Serial.println("==================================================");
}

void loop() {
  unsigned long tempoAtual = millis();

  // Monitoramento contínuo da falha de energia (RNF07 / CA04)
  verificarAlimentacao();

  // Ciclo exato de 5 segundos sem travar o processador (RF01 / RNF01 / RNF02)
  if (tempoAtual - tempoUltimaLeitura >= INTERVALO_LEITURA) {
    tempoUltimaLeitura = tempoAtual;
    executarCicloChocadeira();
  }
}

void verificarAlimentacao() {
  // Lê o estado da chave deslizante (LOW indica chave aberta/desligada -> falha na rede)
  bool falhaRede = (digitalRead(PINO_REDE_ELETRICA) == LOW);

  if (falhaRede && !modoNobreakAtivo) {
    modoNobreakAtivo = true;
    Serial.println("\n[ENERGIA - RNF07] ALERTA: Queda na rede elétrica principal!");
    Serial.println("[ENERGIA - RNF07] Sistema operando normalmente via NOBREAK.\n");
  } 
  else if (!falhaRede && modoNobreakAtivo) {
    modoNobreakAtivo = false;
    Serial.println("\n[ENERGIA - RNF07] INFO: Rede elétrica principal restabelecida.\n");
  }
}

void executarCicloChocadeira() {
  float temperatura = dht.readTemperature();

  Serial.println("--------------------------------------------------");
  
  // 1. VERIFICAÇÃO DE FALHA NO SENSOR (RF03 / RNF01)
  if (isnan(temperatura)) {
    Serial.println("[ERRO SENSOR] Falha na leitura do DHT22 (Retorno NaN)!");
    
    // Desliga atuadores por segurança quando não há leitura válida
    digitalWrite(PINO_AQUECEDOR, LOW);
    digitalWrite(PINO_RESFRIADOR, LOW);
    Serial.println("[ATUADORES] Aquecedor e Resfriador DESLIGADOS por segurança.");

    // Dispara e-mail de alerta conforme RF02 e RF03
    simularEnvioEmail("FALHA DE HARDWARE: Leitura invalida (NaN) no sensor DHT22.");
    return;
  }

  // Exibe telemetria simulando camada de Rede (MQTT/Wi-Fi)
  Serial.print("[TELEMETRIA MQTT] Temperatura atual: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");

  // 2. VERIFICAÇÃO DE ALERTAS CRÍTICOS (RF02 / RF03 / CA02)
  if (temperatura > TEMP_ALERTA_ALTA) {
    if (!alertaCriticoAtivo) {
      simularEnvioEmail("PERIGO - TEMPERATURA CRITICA ALTA: " + String(temperatura, 1) + " °C (Acima de 39.0 °C)");
      alertaCriticoAtivo = true;
    }
  } 
  else if (temperatura < TEMP_ALERTA_BAIXA) {
    if (!alertaCriticoAtivo) {
      simularEnvioEmail("PERIGO - TEMPERATURA CRITICA BAIXA: " + String(temperatura, 1) + " °C (Abaixo de 27.0 °C)");
      alertaCriticoAtivo = true;
    }
  } 
  else {
    // Temperatura voltou para a zona segura (entre 27°C e 39°C)
    alertaCriticoAtivo = false;
  }

  // 3. CONTROLE TÉRMICO DOS ATUADORES (RF04 / RF05 / CA03)
  if (temperatura < TEMP_MIN_IDEAL) {
    // Abaixo de 37.5 °C -> Liga Aquecedor, Desliga Resfriador
    digitalWrite(PINO_AQUECEDOR, HIGH);
    digitalWrite(PINO_RESFRIADOR, LOW);
    Serial.println("[CONTROLE - CA03] Temp < 37.5°C -> AQUECEDOR LIGADO (GPIO 18) | Resfriador OFF");
  } 
  else if (temperatura > TEMP_MAX_IDEAL) {
    // Acima de 38.0 °C -> Desliga Aquecedor, Liga Resfriador
    digitalWrite(PINO_AQUECEDOR, LOW);
    digitalWrite(PINO_RESFRIADOR, HIGH);
    Serial.println("[CONTROLE - CA03] Temp > 38.0°C -> RESFRIADOR LIGADO (GPIO 19) | Aquecedor OFF");
  } 
  else {
    // Entre 37.5 °C e 38.0 °C (Faixa Ideal) -> Desliga ambos (RF05)
    digitalWrite(PINO_AQUECEDOR, LOW);
    digitalWrite(PINO_RESFRIADOR, LOW);
    Serial.println("[CONTROLE - RF05] Faixa Ideal (37.5°C - 38.0°C) -> ATUADORES DESLIGADOS");
  }
}

void simularEnvioEmail(String mensagem) {
  Serial.println("**************************************************");
  Serial.println("[CAMADA APLICACAO - ENVIO DE E-MAIL (RF02)]");
  Serial.println("Para: criador@granja.com.br");
  Serial.println("Assunto: ALERTA CRITICO - CHOCADEIRA AVIARIA");
  Serial.println("Mensagem: " + mensagem);
  Serial.println("**************************************************");
}
