#include <Wire.h>
#include <Adafruit_AS7341.h>
#include <AccelStepper.h>
//#include <WiFi.h>
//#include <HTTPClient.h>

#define dirPin 3
#define stepPin 2
#define enablePin 4

#define endStopPin 9

const char* nome = "TP-Link_7201"; /* NOME DA REDE DE WIFI*/
const char* senha = "83345173"; /* SENHA DO WIFI*/

String ID_script = "AKfycbyL-8qUhGZtr0MjQl7bOo-Sv6ppyEliFHagdl5tvsPRrUUAizqqjDLpybR4gs6vu3XZ"; /*IDENTIFICAÇÃO DO SCRIPT DA PLANILHA*/


Adafruit_AS7341 as7341;

String inputString = "";      
bool stringComplete = false;  

int count;

bool flag = false;

char command; 

AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

void setup() {
  // initialize serial:
  Serial.begin(115200);

  if (!as7341.begin()){
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_4X);/* ALTERAR GANHO */
  as7341.enableLED(true); /* LIGAR(true) DESLIGAR(false) O LED */
  as7341.setLEDCurrent(10); /* CORRENTE/INTENSIDADE DO LED */

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  /*WiFi.begin(nome, senha);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("CONECTANDO A REDE...");
  }
  Serial.println("CONECTADO A REDE COM SUCESSO");*/

  pinMode(endStopPin, INPUT_PULLUP);
  pinMode(enablePin, OUTPUT);
  
  // Habilita o motor
  digitalWrite(enablePin, LOW);
  
  // Define a velocidade do motor
  stepper.setMaxSpeed(1000);

    // Move o motor até que o interruptor de fim de curso seja acionado
  while (digitalRead(endStopPin) == HIGH) {
    stepper.moveTo(-5000);
    stepper.run();
  }
  
  // Reseta a posição do motor
  stepper.setCurrentPosition(0); 
}

void moveStepper(int cells){
  
  int steps = cells * 15;
  
  if (steps != 0) {
  Serial.print("Você digitou: ");
  Serial.println(steps);
  
  // Move o motor para a posição correspondente ao número digitado
  stepper.moveTo(steps);

  // Loop para mover o motor de passo gradualmente para a posição desejada
  while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
}

void doReadings(){
  inputString = inputString.substring(0, inputString.length() - 1);
  int turns = inputString.toInt();
  int count = 1;
  uint16_t storeReadings[turns] [8];
  for(count;count<turns + 1;count++){
    moveStepper(count);
    for(int i=0;i<10;i++){
      if (!as7341.readAllChannels()){
        Serial.println("Error reading all channels!"); 
      }
      storeReadings[count][0] = storeReadings[count][1] + as7341.getChannel(AS7341_CHANNEL_415nm_F1);
      storeReadings[count][1] = storeReadings[count][1] + as7341.getChannel(AS7341_CHANNEL_445nm_F2);
      storeReadings[count][2] = storeReadings[count][2] + as7341.getChannel(AS7341_CHANNEL_480nm_F3);
      storeReadings[count][3] = storeReadings[count][3] + as7341.getChannel(AS7341_CHANNEL_515nm_F4);
      storeReadings[count][4] = storeReadings[count][4] + as7341.getChannel(AS7341_CHANNEL_555nm_F5);
      storeReadings[count][5] = storeReadings[count][5] + as7341.getChannel(AS7341_CHANNEL_590nm_F6);
      storeReadings[count][6] = storeReadings[count][6] + as7341.getChannel(AS7341_CHANNEL_630nm_F7);
      storeReadings[count][7] = storeReadings[count][7] + as7341.getChannel(AS7341_CHANNEL_680nm_F8);
      delay(100);
    }
    storeReadings[count] [0] = storeReadings[count] [0] / 10;
    storeReadings[count] [1] = storeReadings[count] [1] / 10;
    storeReadings[count] [2] = storeReadings[count] [2] / 10;
    storeReadings[count] [3] = storeReadings[count] [3] / 10;
    storeReadings[count] [4] = storeReadings[count] [4] / 10;
    storeReadings[count] [5] = storeReadings[count] [5] / 10;
    storeReadings[count] [6] = storeReadings[count] [6] / 10;
    storeReadings[count] [7] = storeReadings[count] [7] / 10; 

    /*if (WiFi.status() == WL_CONNECTED)
    {
    String urlfinal = "https://script.google.com/macros/s/" + ID_script + "/exec?val1=" + String(storeReadings[count][0]) + "&val2=" + String(storeReadings[count][1]) + "&val3=" + String(storeReadings[count][2]) + "&val4=" + String(storeReadings[count][3]) + "&val5=" + String(storeReadings[count][4]) + "&val6=" + String(storeReadings[count][5]) + "&val7=" + String(storeReadings[count][6]) + "&val8=" + String(storeReadings[count][7]);
    HTTPClient http;
    http.begin(urlfinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.println(" ");
    Serial.println("HTTP status code: ");
    Serial.print(httpCode);
    Serial.println(" ");
    http.end();
    }*/
    delay(300);   
    Serial.print("@");
    Serial.print(storeReadings[count][0]);
    Serial.print("A");
    Serial.print(storeReadings[count][1]);
    Serial.print("B");
    Serial.print(storeReadings[count][2]);
    Serial.print("C");
    Serial.print(storeReadings[count][3]);
    Serial.print("D");
    Serial.print(storeReadings[count][4]);
    Serial.print("E");
    Serial.print(storeReadings[count][5]);
    Serial.print("F");
    Serial.print(storeReadings[count][6]);
    Serial.print("G");
    Serial.print(storeReadings[count][7]);
    Serial.print("H");
    Serial.print("\n");
  }  
}

void setLedCurrent(){
  String LedCurrent = inputString.substring(0, inputString.length() - 1);
  int LedValue = LedCurrent.toInt();
  if(LedValue < 101){
    as7341.setLEDCurrent(LedValue);    
  }
}

void setGain(){
  String strGain = inputString.substring(0, inputString.length() - 1);
  int gainValue = strGain.toInt();

  switch (gainValue){
    
    case 1:
    as7341.setGain(AS7341_GAIN_1X);
    break;

    case 2:
    as7341.setGain(AS7341_GAIN_2X);
    break;
    
    case 4:
    as7341.setGain(AS7341_GAIN_4X);
    break;

    case 8:
    as7341.setGain(AS7341_GAIN_8X);
    break;

    case 16:
    as7341.setGain(AS7341_GAIN_16X);
    break;

    case 32:
    as7341.setGain(AS7341_GAIN_32X);
    break;

    case 64:
    as7341.setGain(AS7341_GAIN_64X);
    break;

    case 128:
    as7341.setGain(AS7341_GAIN_128X);
    break;

    case 256:
    as7341.setGain(AS7341_GAIN_256X);
    break;

    case 512:
    as7341.setGain(AS7341_GAIN_512X);
    break;

    default:
    Serial.println("Invalido");
  }
}


bool chooseTask(){

  if(inputString.equals("reset")){
    Serial.println("Resetando a posição do motor...");
    
    while (digitalRead(endStopPin) == HIGH) {
      stepper.moveTo(-5000);
      stepper.run();
    }
    // Reseta a posição do motor
    stepper.setCurrentPosition(0);
    flag = true;
  }
  else{
    command = inputString.charAt(inputString.length() - 1);

    switch (command){
      
    case '#':
    doReadings();
    flag = true;
    break;

    case '*':
    setLedCurrent();
    flag = true;
    break;

    case 'x':
    setGain();
    flag = true;
    break;

    default:
    flag = false;
    break;    
   }
  }
  return flag;
}

void loop() {
  if (Serial.available() > 0) {
    // Lê uma string até o caractere de nova linha
    inputString = Serial.readStringUntil('\n');
    inputString.trim();  // Remove espaços em branco do início e fim da string

    if(!chooseTask()){
      Serial.println("Invalid command");
    }
    flag = false;
  }
}
