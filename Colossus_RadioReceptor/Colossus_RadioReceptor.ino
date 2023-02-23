/**************************************************************************                  
 Radio Controle Receptor - Colossus da Estrela - NRF24L01
 Aurelio Monteiro Avanzi
 22/02/2023
***************************************************************************/

#include <RF24.h>
#include <Servo.h>

uint32_t PiscaAlerta, SetaDireita, SetaEsquerda, amostra_tempo; 

Servo myservo;

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
  Serial.println("*** Colossus Estrela - NRF24L01+ ***"); 

  // Definição dos pinos 
  #define SPEAKERPIN A4
  #define GEAR 6
  #define QUATTRO 9
  #define STEERING 3
  #define HEADLIGHT 7
  #define REARLIGHT A5
  #define MILELIGHT A2
  #define BREAKLIGHT A3
  #define RIGHTSIGNAL 2
  #define LEFTSIGNAL 4
    
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
  
  myservo.attach(3);
  
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

    //BOTÃO 0
    if (Controle.Botao_0 == LOW) { 
      if (AuxBotao0 == LOW) {     
        AuxBotao0 = HIGH;         
        Botao0 = ! Botao0;       
        Serial.print("   Botao 0 Pressionado: ");
        Serial.println(Botao0);     
        digitalWrite(2, LOW);  
        digitalWrite(4, LOW);  
      }
    } else { 
      AuxBotao0 = LOW; 
    }

    if (Botao0 == HIGH) { 
      Botao1 = LOW;  
      Botao2 = LOW; 
      if (millis() -  PiscaAlerta > 500) {  
        digitalWrite(2, !digitalRead(2));
        digitalWrite(4, !digitalRead(4)); 
        tone(SPEAKERPIN, 100, 50);
        //PiscaAlerta = millis();
        //Sirene(); 
        SireneRapida();

      }
    }


    // TRATANDO DADOS RECEBIDOS DO BOTÃO 1
    // USADO PARA SETA PARA DIREITA

    if (Controle.Botao_1 == LOW) {
      if (AuxBotao1 == LOW) {
        AuxBotao1 = HIGH;
        Botao1 = ! Botao1;
        Serial.print("   Botao 1 Pressionado: ");
        Serial.println(Botao1);
        digitalWrite(2, LOW);  
        digitalWrite(4, LOW);  
      }
    } else {
      AuxBotao1 = LOW;
    }

    if (Botao1 == HIGH) {
      Botao2 = LOW;   
      
      if (millis() -  SetaDireita > 500) {
        digitalWrite(2, !digitalRead(2));
      tone(SPEAKERPIN, 80, 50);
        SetaDireita = millis();
      }
      
      //Sirene(); 
    }


    //BOTÃO 2
    if (Controle.Botao_2 == LOW) {
      if (AuxBotao2 == LOW) {
        AuxBotao2 = HIGH;
        Botao2 = ! Botao2;
        Serial.print("       Botao 2 Pressionado: ");
        Serial.println(Botao2);
        digitalWrite(MILELIGHT, LOW);       
      }
    } else {
      AuxBotao2 = LOW;
    }

    if (Botao2 == HIGH) {
      //tone(SPEAKERPIN, 10, 200);
      digitalWrite(MILELIGHT, HIGH);       
    }

    //BOTÃO 3 - Farol
    if (Controle.Botao_3 == LOW) {
      if (AuxBotao3 == LOW) {
        AuxBotao3 = HIGH;
        Botao3 = ! Botao3;
        Serial.print("       Botao 3 Pressionado: ");
        Serial.println(Botao3);
        digitalWrite(HEADLIGHT, LOW);       
        digitalWrite(REARLIGHT, LOW);       
      }
    } else {
      AuxBotao3 = LOW;
    }

    if (Botao3 == HIGH) {
      //tone(SPEAKERPIN, 10, 200);
      digitalWrite(HEADLIGHT, HIGH);       
      digitalWrite(REARLIGHT, HIGH);      
    }
 // Fim Botoes
 
    int aux = 0; 

    // CONTROLE DE DIREÇÃO - controla a posição Do braço do servo motor de acordo com os movimentos no joystick no controle remoto
    aux = map(Controle.Joystick_BX, 0, 255, 40, 140); 
    myservo.write(aux); 

    Serial.print("Direcao:     "); 
    Serial.println(aux); 


    // Tratando valores recebidos dos joysticks para controle de frente ré e aceleração do motor
    // Os valores recebidos do joystick vão de 0 a 255, os ifs abaixo dividem este valor para controlar frente ré e parado.

    if (Controle.Joystick_AY >= 130) {
      aux = map(Controle.Joystick_BX, 130, 255, 20, 255); 
//      analogWrite(5, aux);  
//      analogWrite(6, LOW);     

     // Serial.print("Frente: "); 
     // Serial.println(aux);  

//      digitalWrite(3, digitalRead(2));   

    }
    else if (Controle.Joystick_AY <= 125) { 
      aux = map(Controle.Joystick_AY, 125, 0, 20, 255);
 //     analogWrite(5, LOW);  
 //     analogWrite(6, aux);  

      //Serial.print("Re:     "); 
      //Serial.println(aux);  

 //     digitalWrite(3, HIGH);              

    }
    else {
     
 //     analogWrite(5, LOW);   
 //     analogWrite(6, LOW);   

      //Serial.println("PARADO "); 

//      digitalWrite(3, digitalRead(2)); 

    }

    cont = 0;
  }
  else { 
    cont ++;
    if (cont > 10) { 
      cont = 11;
      Serial.println("Sem Sinal do Radio: Verifique se o transmissor esta ligado");  
 //     digitalWrite(5, LOW);  
 //     digitalWrite(6, LOW);  

      if (millis() - amostra_tempo > 5000) {
      tone(SPEAKERPIN, 700, 100);
        delay(100);
      tone(SPEAKERPIN, 1300, 100);
        amostra_tempo = millis();
      }
    }
  }
} // end loop


void Sirene() {
  int frequencia = 0;
  digitalWrite(RIGHTSIGNAL, HIGH);
  digitalWrite(LEFTSIGNAL, HIGH);
  
  for (frequencia = 80; frequencia < 2200; frequencia += 1) {
  tone(SPEAKERPIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL, LOW);
  digitalWrite(LEFTSIGNAL, LOW);
  
  for (frequencia = 2200 ; frequencia > 200; frequencia -= 1) {
  tone(SPEAKERPIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL, HIGH);
  digitalWrite(LEFTSIGNAL, HIGH);
}



void SireneRapida() {
  int frequencia = 0;
  digitalWrite(RIGHTSIGNAL, HIGH);
  digitalWrite(LEFTSIGNAL, HIGH);

  for (frequencia = 150; frequencia < 1800; frequencia += 40) {
  tone(SPEAKERPIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL, LOW);
  digitalWrite(LEFTSIGNAL, LOW);

  for (frequencia = 1800 ; frequencia > 150; frequencia -= 40) {
  tone(SPEAKERPIN, frequencia, 10);
    delay(1);
  }
  digitalWrite(RIGHTSIGNAL, HIGH);
  digitalWrite(LEFTSIGNAL, HIGH);
}

void beep()
{
      int k = random(1000,2000);
    for (int i = 0; i <=  random(100,2000); i++){
        
        tone(SPEAKERPIN, k+(-i*2));          
        delay(random(.9,2));             
    } 
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(SPEAKERPIN, k + (i * 10));          
        delay(random(.9,2));             
    }
    tone (SPEAKERPIN,300,100); 
}

//END
