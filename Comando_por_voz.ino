#include <SoftwareSerial.h>
#include "SimpleVR.h"



#define tempo 10
int frequencia = 0;
int Pinofalante = 10;



int melodia[] = {660,660,660,510,660,770,380,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,500,500,500,580,660,500,430,380,500,500,500,500,580,660,870,760,500,500,500,500,580,660,500,430,380,660,660,660,510,660,770,380};
int duracaodasnotas[] = {100,100,100,100,100,100,100,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,60,80,60,80,80,80,80,80,80,60,80,60,80,80,80,80,80,60,80,60,80,80,80,80,80,80,100,100,100,100,100,100,100};
/**        
 * Connection
 * Arduino    VoiceRecognitionModule
 * 2   ------->     TX
 * 3   ------->     RX
 */
VR myVR(2,3);    // 2:RX 3:TX, OS PINOS FICAM A SUA ESCOLHA.
uint8_t buf[64];
int led = 8;
int portSpeak(10);
int ROT = 12;
/**
  0: Esperando a ativação do sistema
  1: Esperando o comando
*/
uint8_t sta;

/** indexaçao dos valores para a variavel voice */
#define SISTEMA          (0x02)
#define LIGAR            (0x03) 
#define DESLIGAR         (0x04)
#define INICIAR          (0x05)
#define FINALIZAR        (0x06)
#define ACENDER          (0x11)
#define APAGAR           (0x12)
#define MUSICA           (0X13)
#define LIGAR            (0x15)
#define ROTEADOR         (0x16)
#define INTERNET         (0x17)
#define SIRENE           (0x18)

unsigned long start_time;

void setup(void)
{
  myVR.begin(9600);
  
  /** initialize */
  Serial.begin(115200);
  Serial.println(F("Modulo de comando por voz ativado."));
  
  if(myVR.checkVersion(buf) > 0){
    Serial.println(F("Procurando SimpleVR"));
    Serial.print(F("SW VER:"));
    Serial.print(buf[0], DEC); 
    Serial.print('.');Serial.print(buf[1], DEC); 
    Serial.print('.');Serial.println(buf[2], DEC); 
    Serial.print(F("HW VER:"));
    Serial.print(buf[3], DEC); 
    Serial.print('.');Serial.println(buf[4], DEC); 
  }else{
    Serial.println(F("SimpleVR nao foi encontrado , por favor reinicialize o arduino e verifique a sua conexao."));
    while(1);
  }
  
  if(myVR.setGroup(1) < 0){
    Serial.println(F("Erro de cominicacao, por favor reinicialize o arduino e verifique a sua conexao."));
    while(1);
  }
  
  if(myVR.setEnable(true) < 0){
    Serial.println(F("Erro de cominicacao, por favor reinicialize o arduino e verifique a sua conexao."));
    while(1);
  }
  
  if(myVR.checkSystemSettings(buf) < 0){
    Serial.println(F("Erro de cominicacao, por favor reinicialize o arduino e verifique a sua conexao."));
    while(1);
  }else{
    if(buf[0]){
      Serial.println(F("SimpleVR esta habilitado."));
    }else{
      Serial.println(F("SimpleVR is desabilitado."));
    }
    Serial.print(F("Grupo"));
    Serial.print(buf[1], DEC); Serial.println(F("Esta selecionado."));
    if(buf[2] != 0xFF){
      Serial.print(F("O valor limite e: ")); Serial.println(buf[2], DEC);
      Serial.println("Por favor espere 5 segundos, depois de o seu comando.");
      Serial.println();
    }
  }
  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  pinMode(portSpeak, OUTPUT);
  digitalWrite(portSpeak, LOW);
  pinMode(ROT, OUTPUT);
  digitalWrite(ROT, HIGH);
  
  /** Inicializando a variavel de controle do sistema */
  sta = 0;
  start_time = millis();
}

void loop(void)
{
  int ret;
  uint16_t voice=0;
  ret = myVR.recognize(buf, 50);
  if(ret > 0){
    voice += buf[0];
    voice <<=8;
    voice += buf[1];
    switch(voice){
      case SISTEMA:
        if(sta == 0){
          sta = 1;
          tone(10, 300, 200);
          start_time = millis();
          Serial.println(F("Sistema ativado, aguardando o comando."));  
        }        
        break;
        
      case LIGAR:
        if(sta == 1){
          sta = 0;
          tone(10, 200, 500);
          delay(300);
          tone(10, 500, 500);
          delay(300);
          Serial.println(F("Led ligado, chame sistema novamente e de seu comando."));
          digitalWrite(led, HIGH);
        }
        break;

        
       case DESLIGAR:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("Led desligado, chame sistema novamente e de seu comando."));
          digitalWrite(led, LOW);
        }
        break;

        
        case MUSICA:
          if(sta==1){
            sta = 0;
            Serial.println(F("musica do mario tocando"));
          //for para tocar as 156 notas começando no 0 ate 156 ++ incrementado
             for (int nota = 0; nota < 156; nota++)
             {
              int duracaodanota = duracaodasnotas[nota];
              tone(portSpeak, melodia[nota],duracaodanota);
          //pausa depois das notas
              int pausadepoisdasnotas[] ={150,300,300,100,300,550,575,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,150,300,350,150,350,150,300,150,600,150,300,350,150,150,550,325,600,150,300,350,150,350,150,300,150,600,150,300,300,100,300,550,575};
              delay(pausadepoisdasnotas[nota]);}
        
              noTone(portSpeak);
        }
        break;

        
       case INICIAR:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("Led INICIADO, chame sistema novamente e de seu comando."));
          digitalWrite(led, HIGH);
        }
          break;

          
       case FINALIZAR:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("Led FINALIZADO, chame sistema novamente e de seu comando."));
          digitalWrite(led, LOW);
        }
        break;
        
       case ACENDER:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("Led ACESO, chame sistema novamente e de seu comando."));
          digitalWrite(led, HIGH);
        }
        break;

        
       case APAGAR:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("Led APAGADO, chame sistema novamente e de seu comando."));
          digitalWrite(led, LOW);
        }
        break;
        

                                        
       case ROTEADOR:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("REINICIANDO O ROTEADOR."));
          digitalWrite(ROT, LOW);
          delay(5000);
          digitalWrite(ROT, HIGH);
        }
        break;



       case INTERNET:
        if(sta == 1){
          sta = 0;
          tone(10, 500, 500);
          delay(300);
          tone(10, 200, 500);
          delay(300);
          Serial.println(F("reiniciando a internet"));
          digitalWrite(ROT, LOW);
          delay(5000);
          digitalWrite(ROT, HIGH);
        }
        break;



       case SIRENE:
        if(sta == 1){
         sta = 0;
          for (frequencia = 150; frequencia < 1800; frequencia += 1) 
          {
            tone(portSpeak, frequencia, tempo); 
            delay(1);
          }
          for (frequencia = 1800; frequencia > 150; frequencia -= 1) 
          {
            tone(portSpeak, frequencia, tempo); 
            delay(1);
          }
         
        }
        break;

          
          
        
    }
  }
  
  /** wait action command for 4 seconds */
  if(sta == 1){
    if(millis()-start_time > 4000){
      sta = 0;
      Serial.println(F("Tempo para comando encerrado, chame sistema novamente."));  
    }
  }
}
