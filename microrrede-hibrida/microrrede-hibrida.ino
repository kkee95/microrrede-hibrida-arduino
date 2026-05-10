#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuração do LCD I2C (Endereço 0x27, 20 colunas, 4 linhas)
LiquidCrystal_I2C lcd(0x27, 20, 4);
// Definição dos Pinos de Saída (Relés de Chaveamento de Fonte)
const int FONTE_SOLAR = 2;
const int FONTE_EOLICA = 3;
const int FONTE_BIOMASSA = 4;

// Definição dos Pinos de Entrada (Sensores Analógicos)
const int SENSOR_BATERIA = A0; 
const int SENSOR_IRRADIANCIA = A1;

void setup() {
  //Abre o canal de comunicação com o PC
  Serial.begin(9600);

  // Configura os pinos dos relés como saída
  pinMode(FONTE_SOLAR, OUTPUT);
  pinMode(FONTE_EOLICA, OUTPUT);
  pinMode(FONTE_BIOMASSA, OUTPUT);
  
  // Inicia com os relés desligados (HIGH no Wokwi geralmente desliga)
  digitalWrite(FONTE_SOLAR, HIGH);
  digitalWrite(FONTE_EOLICA, HIGH);
  digitalWrite(FONTE_BIOMASSA, HIGH);

  // Inicialização do Display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA MICROREDE");
  lcd.setCursor(0, 1);
  lcd.print("CTEP-RM CAMACARI");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Leitura dos dados e conversão para grandezas elétricas
  int valorBat = analogRead(SENSOR_BATERIA);
  float tensaoBateria = valorBat * (15.0 / 1023.0); // Simula leitura até 15V
  
  int valorLuz = analogRead(SENSOR_IRRADIANCIA);
  int percentualLuz = map(valorLuz, 0, 1023, 0, 100);

  // Exibição de Dados no LCD
  lcd.setCursor(0, 0);
  lcd.print("Bateria: "); lcd.print(tensaoBateria); lcd.print("V");
  
  lcd.setCursor(0, 1);
  lcd.print("Irradiancia: "); lcd.print(percentualLuz); lcd.print("%");

  // LÓGICA DE GERENCIAMENTO DE CARGA
  lcd.setCursor(0, 3);
  
  // Condição 1: Prioridade para Energia Solar (Dia com sol)
  if (percentualLuz > 60) { 
    digitalWrite(FONTE_SOLAR, LOW);  // Ativa Solar
    digitalWrite(FONTE_EOLICA, HIGH);
    digitalWrite(FONTE_BIOMASSA, HIGH);
    lcd.print("MODO: SOLAR       ");
  } 
  // Condição 2: Bateria crítica à noite ou chuva (Ativa Biomassa)
  else if (percentualLuz <= 60 && tensaoBateria < 11.5) {
    digitalWrite(FONTE_SOLAR, HIGH);
    digitalWrite(FONTE_EOLICA, HIGH);
    digitalWrite(FONTE_BIOMASSA, LOW); // Ativa Biomassa
    lcd.print("MODO: BIOMASSA    ");
  }
  // Condição 3: Operação padrão (Energia Eólica)
  else {
    digitalWrite(FONTE_SOLAR, HIGH);
    digitalWrite(FONTE_EOLICA, LOW);  // Ativa Eólica
    digitalWrite(FONTE_BIOMASSA, HIGH);
    lcd.print("MODO: EOLICA      ");
  }
  // Envia os valores da memória para o PC via USB
  Serial.print(tensaoBateria); 
  Serial.print(",");
  Serial.println(percentualLuz);

  delay(1000); // Taxa de atualização de 1 segundo
}