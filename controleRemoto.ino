#include <EEPROM.h>

#include <SoftwareSerial.h>
  SoftwareSerial mySerial(10, 11); // RX, TX

//******** Constantes ************
  //const int bSize = 20;//Tamanho do buffer serial  

//*********  Variaveis Globais ********

  //EEPROM ADDRESS
  int addr = 0;
  boolean btConnected = false;
  boolean salvaStatus = false;
  int statusTrava = 0;// 1 é travado, 0 é destravado
  char bufferserial[40];
  int serialCountBuffer = 0;  
  int i=0;
  const int btEnable = 9;       //define pino bluetooth enable
  const int travaOutput = 6;    //define pino saida Trava
  const int destravaOutput = 7; //define pino saida Destrava
  const int RELE = 5;           //define pino saida RELE
  const int btProg = 8;         //define pino saida btProg(bt pin34)
  //int btStatus = 
  const int ledPin = 13;        //Led placa arduino pro mini
  const int REMOTE_CONTROL_TYPE = 100;//Hardware = Remote Control Car
  const int RELAY_TYPE = 101;//Hardware = Rele
  const int RESERVED1 = 102;//Hardware = Reservado1
  const int RESERVED2 = 103;//Hardware = Reservado2
  const int RESERVED3 = 104;//Hardware = Reservado3
  const int RESERVED4 = 105;//Hardware = Reservado4
  const int RESERVED5 = 106;//Hardware = Reservado5
  const int RESERVED6 = 107;//Hardware = Reservado6
  const int RESERVED7 = 108;//Hardware = Reservado7
  const int RESERVED8 = 109;//Hardware = Reservado8
  const int RESERVED9 = 110;//Hardware = Reservado9
  const int RESERVED10 = 111;//Hardware = Reservado10
  int tipoDeHardware = 101;//tipo de hardware default
  
  
void setup() {
  
  /******* Config Pins **********/
  //pinMode(btStatus, INPUT);         // set pin to input
  pinMode(travaOutput, OUTPUT);     //set pin as output
  pinMode(destravaOutput, OUTPUT);  //set pin as output
  pinMode(ledPin, OUTPUT);          //set pin as output
  pinMode(RELE, OUTPUT);            //set pin as output
  pinMode(btEnable, OUTPUT);        //set pin as output
  pinMode(btProg, OUTPUT);          //set pin as output

  /****** Init Pins *********/
  digitalWrite(travaOutput, LOW);
  digitalWrite(destravaOutput, LOW); 
  digitalWrite(btEnable, HIGH);
  digitalWrite(RELE, LOW);
  digitalWrite(btProg, LOW);
  digitalWrite(ledPin, LOW);
  
  Serial.begin(115200);
  Serial.println("TRAVA ELETRICA");

  //Le contHorimetro da EEPROM
  addr = 0;
  statusTrava = (EEPROM.read(addr));
  Serial.print("Status EEPROM ");
  Serial.println(statusTrava);

  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(200);
}

void loop() {

  if(salvaStatus){//Salva status da tranca caso bateria seja desligada
    //incrementa e salva status da trava
    //Serial.println(horimetro);
    EEPROM.write(addr,statusTrava);
    
  }

  while (mySerial.available()) {
      //Le byte da serial Bluetooth
      bufferserial[serialCountBuffer]=mySerial.read();
      //Verifica se o byte é fim de string(\n) ou se estourou o buffer serial   
      if(bufferserial[serialCountBuffer]==10 || serialCountBuffer>40){
        for(i=0;i<serialCountBuffer;i++){
          //Verifica comandos via bluetooth
          //Comando TRAVA
          if(bufferserial[i]=='C' && bufferserial[i+1]=='M' && bufferserial[i+2]=='L'){
             Serial.println("Comando TRAVA");
             pulsoTrava(); 
          //Comando DESTRAVA     
          }else if(bufferserial[i]=='C' && bufferserial[i+1]=='M' && bufferserial[i+2]=='U'){
             Serial.println("Comando DESTRAVA");
             pulsoDestrava(); 
          //Resposta OK do HC-05    
          }else if(bufferserial[i]=='O' && bufferserial[i+1]=='K'){
             Serial.println("ATComando Aceito");
             mySerial.print("ATComando Aceito");      
          }else if(bufferserial[i]=='C' && bufferserial[i+1]=='M' && bufferserial[i+2]=='B'){
             Serial.println("Comando BUZINA");
             comandoBuzina(); 
          //Questiona tipo de hardware   
          }else if(bufferserial[i]=='C' && bufferserial[i+1]=='M' && bufferserial[i+2]=='Q'){
             Serial.println("Comando QUESTIONA");
             comandoQuestiona(); 
          }          
        }
        //Comando troca de nome do módulo bluetooth
        String mBufferserail = bufferserial;
        if(mBufferserail.substring(0,8)=="AT+NAME="){
//          Serial.print("Buffer Serial Sub: "); 
//          Serial.print(mBufferserail.substring(8));
            btProgMode();
            for(i=0;i<serialCountBuffer;i++){
              mySerial.print(bufferserial[i]);  
              //Serial.print(bufferserial[i]); 
            }
          mySerial.println();
          delay(1000);
          btNormalMode();
        }else if(mBufferserail.substring(0,8)=="AT+PSWD="){
          Serial.print("Buffer Serial Sub: "); 
          Serial.print(mBufferserail.substring(8));
            btProgMode();
            for(i=0;i<serialCountBuffer;i++){
              mySerial.print(bufferserial[i]);  
              Serial.print(bufferserial[i]); 
            }
          mySerial.println();
          delay(1000);
          btNormalMode(); 
        }
        serialCountBuffer=0;
        Serial.print("Buffer Serial: "); 
        Serial.print(bufferserial);
      }else{
        serialCountBuffer++;
      }
       //Serial.write(mySerial.read());
    }
    //verifica se chegou dados na serial Arduino
    if (Serial.available()) {
      Serial.print("echo:"); 
      mySerial.write(Serial.read());
    }  
}

void pulsoTrava(){
  digitalWrite(ledPin, HIGH);
  digitalWrite(travaOutput, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  digitalWrite(travaOutput, LOW);
}

void pulsoDestrava(){
  digitalWrite(ledPin, HIGH);
  digitalWrite(destravaOutput, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  digitalWrite(destravaOutput, LOW);
}

void comandoBuzina(){
  digitalWrite(ledPin, HIGH);
  digitalWrite(RELE, HIGH);
  delay(2000);
  digitalWrite(RELE, LOW);
  digitalWrite(ledPin, LOW);
}

void comandoQuestiona(){
  digitalWrite(ledPin, HIGH);
  delay(200);
  addr = 10;
  tipoDeHardware = (EEPROM.read(addr));
  Serial.print("Tipo de Hardware ");
  Serial.println(tipoDeHardware);
  mySerial.print(tipoDeHardware);
  digitalWrite(ledPin, LOW);
}

void btProgMode(){
  digitalWrite(btProg,HIGH);
  delay(1000);
}

void btNormalMode(){
  digitalWrite(btProg,LOW);
  delay(500);
}

