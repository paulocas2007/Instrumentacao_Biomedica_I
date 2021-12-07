/*
  Engenharia Biomedica I
  Paulo Cesar Menegon de Castro RA11201721788

  Codigo usado para calibracao:
  Escreve no LCD o total de pulsos, a distancia,
  a velocidade e as rotacoes por minuto, permitindo
  verificar se as medicoes estao corretas

  Data: 06.12.2021
*/

// Bibliotecas
#include <Wire.h>                                           // Comunicacao I2C
#include <LiquidCrystal_I2C.h>                              // Display LCD comunicacao I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);                         // Display endereco 027h 4 linhas x 20 caracteres
// Definicoes
#define pinoAD 34                                           // Pino do conversor AD - GPIO 34 (Analog ADC1_CH6)
#define raioRoda 0.335                                      // Raio da roda da bicicleta(m)
#define circRoda 6.2832 * raioRoda / 18                     // Comprimento de uma volta entre os raios da roda
int pulsos = 0;                                             // Numero de pulsos do sensor indutivo
int valorAD = 0;                                            // Valor da conversao AD (0 = 0V, 4095 = 3.3V)
float dist = 0;                                             // Distancia (m)
float rpm = 0;                                              // Rotacoes por minuto
float dist_ant = 0;                                         // Distancia armazenada antes da proxima leitura
float pulsos_ant = 0;                                       // Pulso armazenado antes da proxima leitura
float velo = 0;                                             // Velocidade (m/s)
boolean leAD = 0;                                           // Habilita uma leitura somente, evitando contagem de multiplos pulsos
unsigned long tmpEscritaLCD = 0;                            // Intervalo para escrita no LCD
unsigned long tmpLeituraAD = 0;                             // Intervalo para leitura do AD

void setup()
{
  Serial.begin(9600);
  lcd.init();                                               // Inicia o LCD via I2C
  lcd.backlight();                                          // Liga o backlight do LCD
  pinMode(pinoAD, INPUT);                                   // Define pino AD como entrada (conversor AD)
}

void loop()
{
  if (millis() - tmpLeituraAD >= 4 ) {                      // Leitura do AD a cada 4 ms (250 Hz). A funcao millis() nao para o
    int i = 0;
    tmpLeituraAD = millis();                                // o processamento ao contrario da funcao delay()
    valorAD = analogRead(pinoAD);                           // Le valor analogico
    if (valorAD > 1500 && valorAD < 2500) {                  // Valores obtidos de testes com diferentes frequencias de rotacao
      if (leAD == 0) {                                      // Realiza uma leitura somente evitando multiplas leitura
        pulsos++;                                           // Contagem de pulsos
        dist = pulsos * circRoda;                           // Distancia percorrida
        leAD = 1;                                           // Permite somente uma leitura do sensor
      }                                                     // e consequentemente inibe contagem de multiplos pulsos
    }
    if (valorAD == 0) {
      leAD = 0;
    }
    Serial.println(valorAD);
  }

  if (millis() - tmpEscritaLCD >= 1000) {                   // Atualiza LCD a cada 1s. A função millis()
    tmpEscritaLCD = millis();                               // e utilizada para nao interromper o
    lcd.clear();                                            // o processamento como ocorre com delay()
    lcd.setCursor(0, 0);                                    // Coordenadas do display LCD (coluna, linha)
    lcd.print("Pulsos: ");
    lcd.print(pulsos);
    lcd.setCursor(0, 1);
    lcd.print("RPM: ");
    lcd.print(rpm);
    lcd.setCursor(0, 2);
    lcd.print("Dist.: ");
    lcd.print(dist);
    lcd.print(" m");
    lcd.setCursor(0, 3);
    lcd.print("Veloc.: ");
    lcd.print(velo);
    lcd.print(" m/s");
    dist_ant = dist;                                        // Distancia antes da proxima medida
    pulsos_ant = pulsos;                                    // Pulsos antes da proxima medida
  }
  velo = dist - dist_ant;                                   // Calculo da velocidade (m/s)
  rpm = ((pulsos - pulsos_ant) / 18) * 60;                  // Calculo das rotacoes por minuto
}
