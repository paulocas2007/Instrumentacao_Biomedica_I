/*
  Engenharia Biomedica I
  Paulo Cesar Menegon de Castro RA11201721788

  Funcao: exibe informacoes sobre a distância percorrida em m e km e a velocidade
  em m/s e km/hora em um display LCD ou em um celular via Blynk

  Data: 06.12.2021
*/

// Carrega bibliotecas
#include <Wire.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_PRINT Serial
#include <LiquidCrystal_I2C.h>                              // Controle do display LCD via I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);                         // Display 4 linhas x 20 caracteres
int pinoAD = 34;                                            // Pino do conversor AD - GPIO 34 (Analog ADC1_CH6)
int pinoInt = 32;                                           // Pino da interrupcao
const float raioRoda = 0.335;                               // Raio da roda da bicicleta(m)
const float circRoda = 0.349 * raioRoda;                    // Comprimento de 1/18 de uma volta completa
volatile long int pulsos = 0;                               // Numero de pulsos do sensor indutivo
volatile int valorAD = 0;                                   // Valor da conversao AD (0 = 0V, 4095 = 3.3V)
volatile float dist = 0;                                    // Variaveis usadas dentro e fora da interrupcao
volatile float dist_ant = 0;                                // sao do tipo volatile
volatile float velo = 0;
volatile int km;
unsigned long tmpLeituraAD = 0;                             // Intervalo para leitura do conversor AD
unsigned long tmpEscritaLCD = 0;                            // Intervalo para escrita no LCD
unsigned long tmpVelocidade = 0;                            // Intervalo para escrita no LCD
char auth[] = "C8VR0ysWFrMSemx2pB50-QB_xEtMf6QD";           // Autenticacao Blynk
// Autenticacao rede WiFi
const char* ssid = "manoela_2g";
const char* password = "05032011";
BlynkTimer timer;

void IRAM_ATTR funcao_ISR()                                 // Funcao chamada quando ha interrupcao
{
  valorAD = analogRead(pinoAD);
  if (valorAD > 1500 && valorAD < 2500) {
    pulsos++;
  }
}

void setup()
{
  // Comunicacao serial
  lcd.init();                                               // Inicia o LCD via I2C
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Instrumentacao");
  lcd.setCursor(4, 1);
  lcd.print("Biomedica I");
  lcd.setCursor(1, 2);
  lcd.print("Paulo C. M. Castro");
  lcd.setCursor(3, 3);
  lcd.print("RA11201721788");
  delay(5000);
  lcd.clear();
  pinMode(pinoAD, INPUT);
  pinMode(pinoInt, INPUT);
  attachInterrupt(pinoInt, funcao_ISR, HIGH);
  // Autenticacao Blynk
  Blynk.begin(auth, ssid, password);
  // Intervalo de 1s entre as leituras
  timer.setInterval(1000L, principal);
}

void loop()
{
  timer.run();
  Blynk.run();
}

void principal() {
  if (millis() - tmpEscritaLCD >= 1000) {
    tmpEscritaLCD = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distancia Percorrida");
    lcd.setCursor(0, 1);
    km = dist / 1000;
    lcd.print(km);
    lcd.print(" km ");
    lcd.print(dist - km * 1000);
    lcd.print(" m");
    lcd.setCursor(0, 2);
    lcd.print("Velocidade");
    lcd.setCursor(0, 3);
    lcd.print(velo);
    lcd.print(" m/s ");
    lcd.print(velo * 3.6);
    lcd.print(" km/h");
    dist_ant = dist;
  }
  dist = pulsos * circRoda, 2;
  velo = dist - dist_ant;
  // envia dados para o Blynk
  Blynk.virtualWrite(V0, velo);
  Blynk.virtualWrite(V1, velo * 3.6);
  Blynk.virtualWrite(V2, km);
  Blynk.virtualWrite(V3, dist - km * 1000);
}
