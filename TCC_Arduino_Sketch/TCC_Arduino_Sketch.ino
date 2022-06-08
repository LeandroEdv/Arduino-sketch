// ----------- INICIALIZAÇÃO DO SENSOR ULTRASÔNICO ----------

#include "Ultrasonic.h" // BIBLIOTECA
#define pin_trigger 11
#define pin_echo 10
Ultrasonic ultrasonic(pin_trigger, pin_echo); //INICIALIZA OS PINOS DOS SENSORES

//---------------- INICIALIZAÇÃO RTC -------------------------

#include <Wire.h> //BIBLIOTECA
#include "RTClib.h" //BIBLIOTECA
RTC_DS3231 rtc; //OBJETO DO TIPO RTC
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

//---------------- INICIALIZAÇÃO DO SENSOR DE TEMPERATURA --------

#include <OneWire.h>
#include <DallasTemperature.h>
#define DS18B20 12 //DEFINE O PINO DIGITAL UTILIZADO PELO SENSOR
OneWire ourWire(DS18B20); //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&ourWire); //BIBLIOTECA DallasTemperature UTILIZA A OneWire

// -------------- INICIALIZAÇÃO DO SENSOR LDR -------------------

#define sen_ldr A1

//------------- INICIALIZA OS PIN-----------------------------

#define refrig 8
#define bomba 7
#define luminaria 6
#define auxiliar 5

#define led_nivel_alto 2
#define led_nivel_baixo 3
#define led_nivel_critico 4

//-------------- VARIÁVEIS -----------------------------------

int tempo_bomba_ligada = 30;  // QUANTO TEMPO A BOMBA PERMANECE LIGADA 
int tempo_bomba_desligada = 30; // TEMPO EM SEGUNDOS DA BOMBA EM ESPERA (intervalos).
int tempo_leitura_ultra = 15; // *** NÃO PODE SER MENOR QUE 15s ***
int tempo_leitura_temperatura = 10;
int tempo_leitura_lumi = 9; 
int nivel_alto = 10; // NIVEL ALTO (cm)
int nivel_baixo = 20;  // NIVEL BAIXO (cm)

// ------------- Variaveis De controle ----------------------

int distancia;
static bool flag_bomba_ligada;
static bool flag_luminaria;
float temperatura_media;
float tempo_adicional; //<------------ TESTANDO ! Não implementado 

// --------------------- REFERENCIAS DE TEMPO ---------------

DateTime now;
DateTime ref_tempo_liga;
DateTime ref_tempo_desliga;
DateTime ref_tempo_ultra;
DateTime ref_tempo_temperatura;
DateTime ref_tempo_lumi;

//======================= FUNÇÕES SETUP/LOOP =======================

void setup() {

  start();
  strt_rtc();
  digitalWrite(bomba, HIGH);
  digitalWrite(refrig, HIGH);
  digitalWrite(luminaria, HIGH);
  digitalWrite(auxiliar, HIGH);

}
void loop() {  

  now = rtc.now();
  leitura_temperatura ();
  check_ultra_sen();
  acionamento_bomba();
  comportamento_luminaria();
}
//======================= INICIALIZAÇÂO =======================

void start(){
  
  Serial.begin(9600);
  DateTime ref_tempo_liga = rtc.now();
  DateTime ref_tempo_ultra = rtc.now();
  DateTime ref_tempo_temperatura = rtc.now();
  DateTime ref_tempo_lumi = rtc.now(); 
  
 // ------------------- DEFINIÇOES DOS PIN --------------------
 
  pinMode(bomba, OUTPUT);
  pinMode(luminaria, OUTPUT);
  pinMode(refrig, OUTPUT);
  pinMode(auxiliar, OUTPUT);
  
  pinMode(led_nivel_alto, OUTPUT);
  pinMode(led_nivel_baixo, OUTPUT);
  pinMode(led_nivel_critico, OUTPUT);
  pinMode(sen_ldr, INPUT_PULLUP);

}
  
  // =================== INICIALIZAÇÃO RTC ==============================================
  
  void strt_rtc(){
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     //rtc.adjust(DateTime(2022, 5, 30, 10, 44, 0));
  }
}
    
// ========================== VERIFICAÇÃO DE NÍVEL =======================================

  void ultrasonic_sen(){
    
    digitalWrite(pin_trigger, LOW);
    delayMicroseconds(2); 
    digitalWrite(pin_trigger, HIGH); 
    delayMicroseconds(10);  
    digitalWrite(pin_trigger, LOW); 
    distancia = (ultrasonic.Ranging(CM));
    delayMicroseconds(10);
  }

  void check_ultra_sen(){
    
    if (now.unixtime() >= (ref_tempo_ultra.unixtime() + tempo_leitura_ultra)){

      ultrasonic_sen();   
      Serial.println("lendo o nível da água ");
      
      if (distancia <= nivel_alto){
        
        // NIVEL ALTO  
        digitalWrite(led_nivel_alto, HIGH);
        digitalWrite(led_nivel_baixo, LOW);
        digitalWrite(led_nivel_critico, LOW);
        }
      if (distancia > nivel_alto && distancia < nivel_baixo){
        
          // NIVEL BAIXO
          digitalWrite(led_nivel_alto, LOW);
          digitalWrite(led_nivel_baixo, HIGH);
          digitalWrite(led_nivel_critico, LOW);
        }
      if (distancia > nivel_baixo){
        
        // NIVEL CRITICO
        flag_bomba_ligada = false; 
        digitalWrite(led_nivel_alto, LOW);
        digitalWrite(led_nivel_baixo, LOW);
        digitalWrite(led_nivel_critico, HIGH);
          }
          ref_tempo_ultra = now;
        }
      } 
// ========================= CALCULO DE TEMPO ENTRE ACINAMENTOS DA BOMBA ==============================

  int tempo_para_ligar(){

    if (estado() == 1){
      
    int result;
    result = (tempo_bomba_desligada /4) + tempo_bomba_desligada;
    return result;
    }else {
      return tempo_bomba_desligada;
      }
  }

//  ======================== COMANDOS DA BOMBA =========================================================
  
  void comportamento_bomba(){
    
    static bool flag_sen;
        
    if (now.unixtime() >= (ref_tempo_liga.unixtime() + tempo_para_ligar()) && flag_bomba_ligada == false ){
      
      if (flag_sen == false){ 
      ultrasonic_sen(); // <------- VERIFICAR O SENSOR ULTRASÔNICO ANTES DE LIGAR A BOMBA ----<<<<
       
      Serial.println("Leitura de verificação da bomba");
       flag_sen = true;
      }
      
     if (distancia <= nivel_baixo){
      
      Serial.println(" Bomba ligada !");
      flag_bomba_ligada = true; // <----- ESSE FLAG FAZ A BOMBA LIGAR! ---<<<<<
      ref_tempo_desliga = now;
      flag_sen = false; // <---- ESSE FLAG INDICA UMA LEITURA DO SENSOR ---<<<<
        }
      }
      
    if (now.unixtime() >= (ref_tempo_desliga.unixtime() + tempo_bomba_ligada) && flag_bomba_ligada == true){
      
      Serial.println("Bomba desligada !");
      ref_tempo_liga = now;
      flag_bomba_ligada = false;
      }  
  }

  void acionamento_bomba(){ 
  
    comportamento_bomba();
    
     if (flag_bomba_ligada == true){
      //  digitalWrite(bomba, HIGH);
         digitalWrite(bomba, LOW);
    }else {
        // digitalWrite(bomba, LOW);
         digitalWrite(bomba, HIGH);
        }
    }
    
  //======================== REFRIGERAÇÃO DA ÁGUA =============================================

 float sen_temperatura(){
  
  float temp;
  sensors.requestTemperatures();
  return  temp = sensors.getTempCByIndex(0);
  
  }
  
 void leitura_temperatura (){
  
  static int cont;
  float temp;
  float maxtemp;
  float mintemp;
  float result;
  
  if (now.unixtime() >= (ref_tempo_temperatura.unixtime() + tempo_leitura_temperatura)){ 

   
    temp = sen_temperatura();
    
     if (cont == 0){
      maxtemp = temp;  
      mintemp = temp;
      cont += 1;
      }
     if (temp > maxtemp && cont != 0){
      maxtemp = temp;  
      } 
     if (temp < mintemp && cont != 0){
      mintemp = temp;
      }
     if(cont != 0 ){
      float soma;
      
      soma = (temp * 3) + (maxtemp + mintemp);
      result = soma /5 ;
      cont +=1;
     } else {
      result = temp;
      }
      if (cont >= 10){
          cont = 0;
      }
    temperatura_media = result;
      
  Serial.print("Temperatura da agua: "); 
  Serial.print(temperatura_media);
  Serial.println("*C");
  ref_tempo_temperatura = now;
  acionamento_refrig();
    }
  }
  void acionamento_refrig(){
    
    if (temperatura_media >= 27.5 ){
     // digitalWrite(refrig,HIGH);
      digitalWrite(refrig,LOW);
      digitalWrite(auxiliar,LOW);
      }else{
        //digitalWrite(refrig,LOW);
        digitalWrite(refrig,HIGH);
        digitalWrite(auxiliar,HIGH);
        }
    }
//  ============================= CONTROLE DA ILUMINAÇÃO ============================================

int estado(){
   if (now.hour() >= 5 && now.hour() < 17){
       // Serial.print("Dia ");
     return 0;
        }
   if(now.hour() >= 17  && now.hour() < 5){
         // Serial.print("Noite ");
      return 1;
          }
      }
      
float sensor_ldr(){
  
   float valor;
   valor = analogRead(sen_ldr);
   return valor;
  }
  
void comportamento_luminaria(){

static bool luz = false;

if (now.unixtime() >= (ref_tempo_lumi.unixtime() + tempo_leitura_lumi )){ 
 Serial.print ("Lendo Ldr ");
 Serial.println(sensor_ldr());

    if (estado()== 0){

   if (sensor_ldr() < 599.00 ){
    
     // luminosidade alta
    //digitalWrite(luminaria, LOW);
    digitalWrite(luminaria, HIGH);
    Serial.println("Luz desligada");
    } 
   if (sensor_ldr() > 600.00 ){
    
    Serial.println("Luz Ligada !!");
    
      digitalWrite(luminaria, LOW);
     // luminosidade baixa
    } 
      }else {
        
        digitalWrite(luminaria, HIGH);
   Serial.print("Noite");
        
        
        }
    ref_tempo_lumi = now;   
 }
}
