#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

#define MAXP 10

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.init();
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando");
  delay(300);
}

char senhaCorreta[MAXP], senha[MAXP], incomingByte;
int digitoCorreta = 0, corretaState = 0, i, digitoSenha = 0, destrancada = 0, delayWaiting = 0;

// corretaState:
// 0: inicial
// 1: aguardando senha
// 2: recebido caracter de texto, aguardando \n
// 3: senha correta salva

void loop() {
  if (corretaState == 0) {
    corretaState = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aguarde...");
    lcd.setBacklight(HIGH);
  }
  if (Serial.available() > 0) { 
    incomingByte = Serial.read();
    Serial.println("Incoming Byte, digitoCorreta, corretaState ");
    Serial.println(incomingByte, DEC);
    Serial.println(digitoCorreta);
    Serial.println(corretaState);
    if (incomingByte == 10) {
      Serial.println(senhaCorreta);
      if (corretaState == 2) {
        corretaState = 1;
        digitoCorreta = 0;
      }
      else if (corretaState == 1 && digitoCorreta > 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Digite sua senha");
        lcd.setBacklight(HIGH);
        corretaState = 3;
      }
      digitoCorreta = 0;
    }
    else if ((incomingByte < 47 || incomingByte > 58) && incomingByte != 13) {
      corretaState = 2;
      digitoCorreta = 0;
    }
    else if (incomingByte != 13) {
      if (digitoCorreta == 0) 
        for (i = 0; i < MAXP; i++) 
          senhaCorreta[i] = 0;
      senhaCorreta[digitoCorreta] = incomingByte;
      digitoCorreta++;
    }
  }
  char customKey = customKeypad.getKey();
  if (delayWaiting && customKey) {
    delayWaiting = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Digite sua senha");
    lcd.setBacklight(HIGH);
    destrancada = 0;
  }
  if (!delayWaiting) {
    if (customKey){
      switch (customKey) {
        case '*':
          for (i = 0; i < digitoSenha; i++) {
            senha[i] = 0;
            lcd.setCursor(i, 1);
            lcd.print(' ');
          }
          digitoSenha = 0;
          break; 
        case '#':
          destrancada = 1;
          for (i = 0; i < MAXP; i++) 
            if (senhaCorreta[i] != senha[i]) {
              destrancada = 0;
              break;
            }
          if (destrancada) {
            lcd.clear();
            lcd.setCursor(2,0);
            lcd.print("Porta aberta");
            lcd.setCursor(2,1);
            lcd.print("Bem vindo(a)");
            delayWaiting = 1;
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Senha incorreta");
            lcd.setCursor(2,1);
            lcd.print("Tente de novo");
            delayWaiting = 1;

            Serial.println(senha);
          }
          for (i = 0; i <= digitoSenha; i++) 
            senha[i] = 0;
          digitoSenha = 0;
          break;
        default:
          if ('0' <= customKey && '9' >= customKey) {
            senha[digitoSenha] = customKey;
            lcd.setCursor(digitoSenha,1);
            lcd.print('*');
            digitoSenha++;
          }
          break;
      }
    }
  }
}
