#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Relés
const int FONTE_SOLAR = 1;
const int FONTE_EOLICA = 2;
const int FONTE_BIOMASSA = 3;

// Cargas
const int CARGA_CRITICA = 5;
const int CARGA_NORMAL = 6;

// Sensores
const int SENSOR_BATERIA = A0;
const int SENSOR_IRRADIANCIA = A1;

// Variáveis
float tensaoBateria;
int percentualLuz;

String estadoSistema;

void setup() {

  Serial.begin(9600);

  // Relés
  pinMode(FONTE_SOLAR, OUTPUT);
  pinMode(FONTE_EOLICA, OUTPUT);
  pinMode(FONTE_BIOMASSA, OUTPUT);

  // Cargas
  pinMode(CARGA_CRITICA, OUTPUT);
  pinMode(CARGA_NORMAL, OUTPUT);

  // Inicializa desligado
  digitalWrite(FONTE_SOLAR, HIGH);
  digitalWrite(FONTE_EOLICA, HIGH);
  digitalWrite(FONTE_BIOMASSA, HIGH);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("MICRORREDE HIBRIDA");

  lcd.setCursor(0,1);
  lcd.print("CTEP-RM CAMACARI");

  delay(3000);
  lcd.clear();
}

void loop() {

  // Leitura da bateria
  int valorBat = analogRead(SENSOR_BATERIA);
  tensaoBateria = valorBat * (15.0 / 1023.0);

  // Leitura irradiância
  int valorLuz = analogRead(SENSOR_IRRADIANCIA);
  percentualLuz = map(valorLuz, 0, 1023, 0, 100);

  // =========================
  // ESTADO NORMAL
  // =========================
  if (tensaoBateria > 12.0 && percentualLuz > 60) {

    estadoSistema = "NORMAL";

    digitalWrite(FONTE_SOLAR, LOW);
    digitalWrite(FONTE_EOLICA, HIGH);
    digitalWrite(FONTE_BIOMASSA, HIGH);

    digitalWrite(CARGA_CRITICA, HIGH);
    digitalWrite(CARGA_NORMAL, HIGH);
  }

  // =========================
  // ESTADO ECONOMIA
  // =========================
  else if (tensaoBateria > 11.3 && percentualLuz <= 60) {

    estadoSistema = "ECONOMIA";

    digitalWrite(FONTE_SOLAR, HIGH);
    digitalWrite(FONTE_EOLICA, LOW);
    digitalWrite(FONTE_BIOMASSA, HIGH);

    digitalWrite(CARGA_CRITICA, HIGH);
    digitalWrite(CARGA_NORMAL, LOW);
  }

  // =========================
  // ESTADO EMERGENCIA
  // =========================
  else {

    estadoSistema = "EMERGENCIA";

    digitalWrite(FONTE_SOLAR, HIGH);
    digitalWrite(FONTE_EOLICA, HIGH);
    digitalWrite(FONTE_BIOMASSA, LOW);

    digitalWrite(CARGA_CRITICA, HIGH);
    digitalWrite(CARGA_NORMAL, LOW);
  }

  // LCD
  lcd.setCursor(0,0);
  lcd.print("BAT: ");
  lcd.print(tensaoBateria);
  lcd.print("V   ");

  lcd.setCursor(0,1);
  lcd.print("SOL: ");
  lcd.print(percentualLuz);
  lcd.print("%   ");

  lcd.setCursor(0,2);
  lcd.print("ESTADO:");

  lcd.setCursor(0,3);
  lcd.print(estadoSistema);

  // Serial
  Serial.print("Bateria: ");
  Serial.print(tensaoBateria);

  Serial.print(" | Irradiancia: ");
  Serial.print(percentualLuz);

  Serial.print("% | Estado: ");
  Serial.println(estadoSistema);

  delay(1000);
}
