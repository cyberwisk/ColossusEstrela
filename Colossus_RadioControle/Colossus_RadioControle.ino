/**************************************************************************                  
 Radio Controle Receptor - Colossus da Estrela - NRF24L01
 Aurelio Monteiro Avanzi
 22/02/2023
***************************************************************************/

#include <RF24.h>

RF24 radio(9, 10);

struct MeuControleRemoto  {  
  boolean Botao_0;
  boolean Botao_1;
  boolean Botao_2;
  boolean Botao_3;
  boolean Botao_4;
  byte Joystick_AY;
  byte Joystick_BX;
 };

MeuControleRemoto ControleRemoto;
#define SPEAKERPIN 4
#int CHANNEL = 100;

void setup() {

  Serial.begin(115200);

  delay(100);

  Serial.println("*** Radio Controle - Colossus da Estrela - NRF24L01 ***");   

  radio.begin();
  radio.setAutoAck(false);
  radio.setChannel(CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.powerUp();
  radio.openWritingPipe(0xE8E8F0F0E1LL);

  delay(100);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  beep();
}

void loop() {

  readVcc(); 

  //Botões
  ControleRemoto.Botao_0 = digitalRead(2);
  ControleRemoto.Botao_1 = digitalRead(3);
  ControleRemoto.Botao_2 = digitalRead(4);
  ControleRemoto.Botao_3 = digitalRead(5);
  
  //JOYSTICKS
  ControleRemoto.Joystick_AY = map (analogRead(A1), 0, 1023, 0, 255);  
  ControleRemoto.Joystick_BX = map (analogRead(A0), 0, 1023, 0, 255);  

  // Transmitindo dados

  for (int i = 0; i < 10; i++) {
    radio.write( &ControleRemoto, sizeof(ControleRemoto) );
  }

  Serial.print("       Joystick_AY: ");
  Serial.println(ControleRemoto.Joystick_AY);      

  Serial.print("       Joystick_BX: ");
  Serial.println(ControleRemoto.Joystick_BX);     

  Serial.print("           Botao_0: ");
  Serial.println(ControleRemoto.Botao_0);      

  Serial.print("           Botao_1: ");
  Serial.println(ControleRemoto.Botao_1);      

  Serial.print("           Botao_2: ");
  Serial.println(ControleRemoto.Botao_2);     

  Serial.print("           Botao_3: ");
  Serial.println(ControleRemoto.Botao_3);      
}

void readVcc()
{
  // Ler referência interna 1.1V
  // Ajusta a referência ao Vcc e a medição de referência interna para 1.1V
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Aguarde
  ADCSRA |= _BV(ADSC); // Inicia a conversão
  while (bit_is_set(ADCSRA, ADSC)); // Medindo
  uint8_t low = ADCL; // Vai ler ADCL primeiro - Então trava ADCH
  uint8_t high = ADCH; // Desbloqueia
  int result = (high << 8) | low;
  result = 1125300L / result; // Calcular Vcc em milivolts; 1125300 = 1.1*1023*1000

  Serial.print("           Bateria: ");
  Serial.println(result);    
  
  if (result < 3100) { // Aciona o buzzer se a tensão da bateria estiver menor que 3,1 volts (para indicar que a bateria esta fraca)
    tone(SPEAKERPIN, 900, 100);
  }
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


// FIM
