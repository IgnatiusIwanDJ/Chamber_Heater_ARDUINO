// Untuk Solenoid Three Way LOW ke bawah (pemanas), HIGH (ke atas)
#define CLK 9
#define DBIT 10 // so
#define CS 13

#include <SoftwareSerial.h>

//Ultrasonic Proximity variable
int counter=0;
int final_value;
float nilai_akhir;

//Apung-apung Sensor
int warning_level_sensor=12;
//Thermocouple variable

// Variabel void loop
int suhu_pembacaan;
int ketinggian_air;
int V1=2;
int V2=4;
int V3=7;
int V4=8;
int P1=11;
int P2=1;
int Speed1=5;
int Speed2=6;
int H=3;
int heater_pwm;
int max_water_level=14;
int min_water_level=12;
int lowest_temperature_level=26;
int highest_temperature_level=36;
int min_stage_4=10;

int v = 0;
float Ctemp, Ftemp;

int spiRead()   {
  int value = 0;
  digitalWrite(CS,LOW);
  delay(2);
  digitalWrite(CS,HIGH);
  delay(220);

  /* Read the chip and return the raw temperature value */
  /* Bring CS pin low to allow us to read the data from
   the conversion process */

  digitalWrite(CS,LOW);
  /* Cycle the clock for dummy bit 15 */
  digitalWrite(CLK,HIGH);
  delay(1);
  digitalWrite(CLK,LOW);

  /*
   Read bits 14-3 from MAX6675 for the Temp. Loop for each bit reading
   the value and storing the final value in 'temp'
   */

  for (int i=14; i>=0; i--) {
    digitalWrite(CLK,HIGH);
    value += digitalRead(DBIT) << i;
    digitalWrite(CLK,LOW);
  }
  // check bit D2 if HIGH no sensor
  if ((value & 0x04) == 0x04) return -1;

  // shift right three places
  return value >> 3;
}

//Jaga ketinggian air
void level_water(){
  warning_level_sensor=digitalRead(12);
   if (ketinggian_air>max_water_level){
     digitalWrite(V1,LOW);
      // Drainase jalan
    }
   if (warning_level_sensor==1){
    digitalWrite(V1,LOW);
   digitalWrite(V2,LOW);
   digitalWrite(V3,LOW);
   digitalWrite(V4,LOW);
   digitalWrite(P1,LOW);
   digitalWrite(P2,LOW);
     // Drainase jalan
   }
}
//Fungsi pembacaan Proximity
int nilai_proximity(){
  float sensorValue;
  counter=0;
  while (counter<20){
  sensorValue+= analogRead(A0);
  counter+=1;
  delay(10);  
 }
  sensorValue=sensorValue/20;;
  nilai_akhir=0.02*sensorValue+1.955;
  final_value=int(nilai_akhir);
  final_value=22-final_value;
  delay(10);  
  return final_value;
}

//Fungsi Pembacaan Thermocouple
int nilai_suhu(){
  int suhu_final;
  v = spiRead();
  if (v == -1)   {
    Serial.print("No sensor \n");
  }
  else   {
    suhu_final =  v * 0.25;
     suhu_final= 1.1865* suhu_final+6.3269;
    //suhu_final=1.9548*suhu_final-19.84;
    //suhu_final=1*suhu_final-0.0006;
  }
  delay(100);
    return suhu_final;
    
}

void initial_solenoid_setup(){
  //Inisialisasi Awal
  digitalWrite(V1,LOW);
  digitalWrite(V2,LOW);
  digitalWrite(V3,LOW);
  digitalWrite(V4,LOW);
}

void stage_1_solenoid_setup(){
   digitalWrite(V1,HIGH);
   digitalWrite(V2,LOW);
   digitalWrite(V3,LOW);
   digitalWrite(V4,LOW);
   digitalWrite(P1,LOW);
   digitalWrite(P2,LOW);
   digitalWrite(H,LOW);
}

void stage_3_solenoid_setup(){
  digitalWrite(V1,LOW);
  digitalWrite(V2,LOW);
  digitalWrite(V3,HIGH);
  digitalWrite(V4,LOW);
  digitalWrite(P2,LOW);
}

void stage_2_solenoid_setup(){
  digitalWrite(V1,HIGH);
  digitalWrite(V2,LOW);
  digitalWrite(V3,LOW);
  digitalWrite(V4,LOW);
  digitalWrite(P1,LOW);
  digitalWrite(P2,LOW);
 
}
void stage_warning_solenoid_setup(){
  digitalWrite(V1,LOW);
  digitalWrite(V2,LOW);
  digitalWrite(V3,LOW);
  digitalWrite(V4,LOW);
  digitalWrite(P1,LOW);
  digitalWrite(P2,LOW);
}
void setup() {
  // put your setup code here, to run once:
  counter=0;
  Serial.begin(9600);
  
  //Normally Close (NC)
  //Solenoid valve V1
  pinMode(V1,OUTPUT);
  //Solenoid three-way valve V2
  pinMode(V2,OUTPUT);
  //Solenoid valve V3
  pinMode(V3,OUTPUT);
  //Solenoid valve V4
  pinMode(V4,OUTPUT);

  //Inisialisasi Awal
  initial_solenoid_setup();

  //PWM
  // Pompa 1--> Pompa looping
  pinMode(P1,OUTPUT);
  pinMode(Speed1,OUTPUT);
  // Pompa 2--> Pompa keluar
  pinMode(P2,OUTPUT);
  pinMode(Speed2,OUTPUT);
  // Heater
  pinMode(H,OUTPUT);

  //Inisialisasi Awal
  digitalWrite(P1,LOW);
  digitalWrite(P2,LOW);
  digitalWrite(H,LOW);

  //Ultrasonic
  pinMode(A0,INPUT);
  //Thermocouple
  pinMode(A1,INPUT);
  //Apung-apung
  pinMode(warning_level_sensor,INPUT);

  pinMode(CLK, OUTPUT);
  pinMode(DBIT, INPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(CLK, LOW);
  // Table
  Serial.println("----------------------------------------------------------------");
  Serial.print(" Ketinggian   ");
  Serial.print(" | ");
  Serial.print(" Suhu ");
  Serial.print(" | ");
  Serial.print(" Apung Apung   ");
  Serial.println(" | ");
}





void loop() {
  // Pembacaan suhu
  suhu_pembacaan=nilai_suhu();
  // Pembacaan Ketinggian air
  ketinggian_air= nilai_proximity();
  
  // Menutup air masuk jika warning menyala
  level_water();
  
  // 4 Stages
  // Stage 1 : Air belum masuk, perhitungan suhu ruangan
  // 24,3-26, dimana ketika melewati 8 cm 
  // heater coba dinyalakan PWM dibawah 100
  // delay (5000) untuk pengisian air --> 5 detik
  
  if (suhu_pembacaan <lowest_temperature_level){
    //if (ketinggian_air>min_water_level and ketinggian_air<max_water_level){
    if (ketinggian_air>=max_water_level){
    stage_1_solenoid_setup();
    analogWrite(H,225);
    // Waktu pengisian air
    }
    //else if (ketinggian_air<=min_water_level){
    else if (ketinggian_air<min_water_level){
    stage_1_solenoid_setup();
    }
    else{
    stage_warning_solenoid_setup();
    //analogWrite(H,200);  
    }
  }
  // Stage 2 : Airmasuk, perhitungan suhu ruangan
  // 26 derajat tp belum ke lingkup 34-40, 
  // Heater dinyalakan dengan
  // fungsi PWN=map (value sekarang, range rendah (26), range tinggi (45), nilai PWM tertinggi (255), nilai PWM terendah (0));
  
  else if (suhu_pembacaan>=lowest_temperature_level && suhu_pembacaan<=highest_temperature_level){
  // Stage 3 : AirLOOP, perhitungan suhu ruangan
  // 26-40, dimana air diisi sampai >=12 lalu diloop
  // hingga <40 lalu dikeluarkan
  
     if (ketinggian_air>=max_water_level){
      //delay(1000);
      stage_3_solenoid_setup();
      digitalWrite(P1,HIGH);
      analogWrite(Speed1,100);
      //while (suhu_pembacaan<=highest_temperature_level){
      while (suhu_pembacaan<=40){
        heater_pwm=map(suhu_pembacaan,26,50,255,150);
        analogWrite(H,heater_pwm);
        delay(100);
        suhu_pembacaan=nilai_suhu();
        Serial.print("|");
        Serial.print(ketinggian_air);
        Serial.print("     |      ");
        Serial.print(suhu_pembacaan);
        Serial.print("     |      ");
        Serial.print(warning_level_sensor);
        Serial.println("     |      ");
      }
      // After heating
      digitalWrite(V3,LOW);
      digitalWrite(V4,HIGH);
      digitalWrite(P1,LOW);
      digitalWrite(P2,HIGH);
      analogWrite(Speed2,100);
      delay(1500); // pembuangan air 3 detik
      ketinggian_air= nilai_proximity();
    }
    
    else{
      stage_2_solenoid_setup();
      heater_pwm=map(suhu_pembacaan,26,50,255,150);
      analogWrite(H,heater_pwm);
     // delay(1000);
     
      
    }
     
  }

  else if( suhu_pembacaan>highest_temperature_level){
      
   // Stage 4 : Air mencapai di atas 40, sehingga perlu di 
   // dinginkan
  while (suhu_pembacaan>highest_temperature_level){
  // if (ketinggian_air<max_water_level){
  if( ketinggian_air<min_stage_4){
    //masukkan air dingin
      digitalWrite(V1,HIGH);
      digitalWrite(V2,HIGH);
      digitalWrite(V3,LOW);
      digitalWrite(V4,LOW);
      digitalWrite(P1,LOW);
      digitalWrite(P2,LOW);
      digitalWrite(H,LOW);
  }
  else{
      digitalWrite(V1,LOW);
      digitalWrite(V2,LOW);
      digitalWrite(V3,LOW);
      digitalWrite(V4,HIGH);
      digitalWrite(P1,LOW);
      digitalWrite(P2,LOW);
      digitalWrite(H,LOW);
      delay(5000);
  }
  
   suhu_pembacaan=nilai_suhu();
   ketinggian_air= nilai_proximity();
  Serial.print("|");
  Serial.print(ketinggian_air);
  Serial.print("     |      ");
  Serial.print(suhu_pembacaan);
   Serial.print("     |      ");
  Serial.print(warning_level_sensor);
  Serial.println("     |      ");
  }
  //delay(2000);
  }
  Serial.print("|");
  Serial.print(ketinggian_air);
  Serial.print("     |      ");
  Serial.print(suhu_pembacaan);
   Serial.print("     |      ");
  Serial.print(warning_level_sensor);
  Serial.println("     |      ");
  //delay(1000);


}


