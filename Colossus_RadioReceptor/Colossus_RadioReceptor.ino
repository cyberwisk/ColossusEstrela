/**************************************************************************                  
 Radio Controle Receptor - Colossus da Estrela - NRF24L01
 Aurelio Monteiro Avanzi
 22/02/2023
***************************************************************************/

#include <RF24.h>
#include <Servo.h>

// Definição dos pinos 
#define SPEAKER_PIN A4
#define ESC_PIN 5
#define GEAR_PIN 6
#define QUATTRO_PIN 9
#define STEERING_PIN 3
#define HEADLIGHT_PIN 7
#define REARLIGHT_PIN A5
#define MILELIGHT_PIN A2
#define BREAKLIGHT_PIN A3
#define RIGHTSIGNAL_PIN 2
#define LEFTSIGNAL_PIN 4

uint32_t PiscaAlerta, SetaDireita, SetaEsquerda, amostra_tempo; 

Servo myservo;
Servo SERVO_STEERING;
Servo SERVO_QUATTRO;
Servo SERVO_GEAR;
Servo SERVO_ESC;

RF24 radio(8, 10);  

struct ControleColossus  {  
  boolean Botao_0;
  boolean Botao_1;
  boolean Botao_2;
  boolean Botao_3;
  boolean Botao_4;
  byte Joystick_AY;
  byte Joystick_BX;
};

ControleColossus Controle; 
int cont = 0;

boolean Botao0  = LOW;
boolean AuxBotao0 = LOW;
boolean Botao1  = LOW;
boolean AuxBotao1 = LOW;
boolean Botao2  = LOW;
boolean AuxBotao2 = LOW;
boolean Botao3  = LOW;
boolean AuxBotao3 = LOW;

void setup() {

  Serial.begin(115200);
  delay(100);   
  Serial.println(" Colossus Estrela - NRF24L01+ "); 
    
  pinMode(2, OUTPUT);     // SETA DIREITA
  pinMode(3, OUTPUT);     // PWM 1
  pinMode(4, OUTPUT);     // SETA ESQUERDA
  pinMode(5, OUTPUT);     // PWM 2
  pinMode(6, OUTPUT);     // PWM 3
  pinMode(7, OUTPUT);     // FAROL
  pinMode(9, OUTPUT);     // PWM 4
  //pinMode(A2, OUTPUT);  // SEM USO
  //pinMode(A3, OUTPUT);  // SEM USO
  pinMode(A4, OUTPUT);    // HORN
  pinMode(A5, OUTPUT);    // REARLIGHT
  pinMode(A6, OUTPUT);    // FAROL DE MILHA
  pinMode(A7, OUTPUT);    // LUZ DE FREIO
   
  //Canal do receptor
  int CHANNEL = 100;
  
  // Config PWM
  SERVO_STEERING.attach(STEERING_PIN);
  
  radio.begin();                            
  radio.setAutoAck(false);                 
  radio.setChannel(CHANNEL);                   
  radio.setDataRate(RF24_250KBPS);         
  radio.setPALevel(RF24_PA_HIGH);         
  radio.powerUp();                          
  radio.openReadingPipe(1, 0xE8E8F0F0E1LL); 
  radio.startListening();                  

beep();
}

void loop() {
  delay(15); 

  if (radio.available()) { 
    radio.read( &Controle, sizeof(Controle) );  

    //BOTÃO 0 - Sirene
    if (Controle.Botao_0 == LOW) { 
      if (AuxBotao0 == LOW) {     
          AuxBotao0 = HIGH;         
          Botao0 = ! Botao0;       
          Serial.print("   Botao 0 Pressionado: ");
          Serial.println(Botao0);     
          digitalWrite(RIGHTSIGNAL_PIN, LOW);  
          digitalWrite(LEFTSIGNAL_PIN, LOW);  
       }
    }
    else { 
      AuxBotao0 = LOW; 
    }
    if (Botao0 == HIGH) { 
      Botao1 = LOW;  
      Botao2 = LOW; 
      if (millis() -  PiscaAlerta > 500) {  
        digitalWrite(RIGHTSIGNAL_PIN, !digitalRead(RIGHTSIGNAL_PIN));
        digitalWrite(LEFTSIGNAL_PIN, !digitalRead(LEFTSIGNAL_PIN)); 
        tone(SPEAKER_PIN, 100, 50);
        //PiscaAlerta = millis();
        //Sirene(); 
        SireneRapida();
      }
    }


    //BOTÃO 1 - Seta para direita 
    if (Controle.Botao_1 == LOW) {
      if (AuxBotao1 == LOW) {
        AuxBotao1 = HIGH;
        Botao1 = ! Botao1;
        Serial.print("   Botao 1 Pressionado: ");
        Serial.println(Botao1);
        digitalWrite(RIGHTSIGNAL_PIN, LOW);  
        digitalWrite(LEFTSIGNAL_PIN, LOW);  
      }
    } else {
      AuxBotao1 = LOW;
    }

    if (Botao1 == HIGH) {
      Botao2 = LOW;   
      
      if (millis() -  SetaDireita > 500) {
        digitalWrite(RIGHTSIGNAL_PIN, !digitalRead(RIGHTSIGNAL_PIN));
        tone(SPEAKER_PIN, 80, 50);
        SetaDireita = millis();
      }
      //Sirene(); 
    }


    //BOTÃO 2 - Farol de Milha
    if (Controle.Botao_2 == LOW) {
      if (AuxBotao2 == LOW) {
        AuxBotao2 = HIGH;
        Botao2 = ! Botao2;
        Serial.print("    Botao 2 Pressionado: ");
        Serial.println(Botao2);
        digitalWrite(MILELIGHT_PIN, LOW);       
      }
    } else {
      AuxBotao2 = LOW;
    }

    if (Botao2 == HIGH) {
      //tone(SPEAKERPIN, 10, 200);
      digitalWrite(MILELIGHT_PIN, HIGH);       
    }

    //BOTÃO 3 - Farol baixo, alto e lanternas
    if (Controle.Botao_3 == LOW) {
      if (AuxBotao3 == LOW) {
        AuxBotao3 = HIGH;
        Botao3 = ! Botao3;
        Serial.print("    Botao 3 Pressionado: ");
        Serial.println(Botao3);
        digitalWrite(HEADLIGHT_PIN, LOW);       
       // digitalWrite(REARLIGHT_PIN, LOW);       
      }
    } else {
      AuxBotao3 = LOW;
    }

    if (Botao3 == HIGH) {
      //tone(SPEAKERPIN, 10, 200);
      digitalWrite(HEADLIGHT_PIN, HIGH);       
     // digitalWrite(REARLIGHT_PIN, HIGH);      
    }
 // Fim Botoes
 
 // Inicio dos controles PWM
 
 // PWM da Direção
    int st_posicao = map(Controle.Joystick_BX, 20, 120, 0, 180); 
    SERVO_STEERING.write(st_posicao);
    if (st_posicao < 90 or st_posicao > 96){
    Serial.print("Direcao_controle:     "); 
    Serial.println(Controle.Joystick_BX); 

    Serial.print("Direcao_servo:     "); 
    Serial.println(st_posicao); 
    }
 // Fim PWM Direção

    cont = 0;
  }
  else { 
    cont ++;
    if (cont > 10) { 
      cont = 11;
      Serial.println("Sem Sinal do Radio: Verifique se o transmissor esta ligado");  
      if (millis() - amostra_tempo > 5000) {
      tone(SPEAKER_PIN, 700, 100);
        delay(100);
      tone(SPEAKER_PIN, 1300, 100);
        amostra_tempo = millis();
      }
    }
  }
} // end loop


void Sirene() {
  int frequencia = 0;
  digitalWrite(RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(LEFTSIGNAL_PIN, HIGH);
  
  for (frequencia = 80; frequencia < 2200; frequencia += 1) {
  tone(SPEAKER_PIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL_PIN, LOW);
  digitalWrite(LEFTSIGNAL_PIN, LOW);
  
  for (frequencia = 2200 ; frequencia > 200; frequencia -= 1) {
  tone(SPEAKER_PIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(LEFTSIGNAL_PIN, HIGH);
}



void SireneRapida() {
  int frequencia = 0;
  digitalWrite(RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(LEFTSIGNAL_PIN, HIGH);

  for (frequencia = 150; frequencia < 1800; frequencia += 40) {
  tone(SPEAKER_PIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL_PIN, LOW);
  digitalWrite(LEFTSIGNAL_PIN, LOW);

  for (frequencia = 1800 ; frequencia > 150; frequencia -= 40) {
  tone(SPEAKER_PIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(LEFTSIGNAL_PIN, HIGH);
}

void beep()
{
      int k = random(1000,2000);
    for (int i = 0; i <=  random(100,2000); i++){
        
        tone(SPEAKER_PIN, k+(-i*2));          
        delay(random(.9,2));             
    } 
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(SPEAKER_PIN, k + (i * 10));          
        delay(random(.9,2));             
    }
    tone (SPEAKER_PIN,300,100); 
}

//END
