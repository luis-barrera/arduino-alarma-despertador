#include <Arduino.h>
#include <Ds1302.h>  // Librería de RTC (Módulo que guarda la fecha y hora)

const int buzzerPin = 8;  // Pin del Buzzer (que hace el sonido)
const int buttonPin = 9;  // Pin del Botón

// Máximo de minutos que puede sonar la alarma
const int MAX_MINUTES = 5;

// Arreglo de alarmas
const int Alarmas[][2] = {
    {5, 30},
    {5, 45},
    {6, 0},
    {6, 30},
    {7, 0},
};

// Creamos un objeto para manipular el RTC
Ds1302 rtc(4, 2, 3);

// Mostrar los días de la semana con su nombre
const static char* WeekDays[] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"};
// Valor de una función sinosoidal
float sinVal;
// Valor del tono para el Buzzer
int toneVal;
// Estado del botón
int buttonState = 0;
// Iterator de las alarmas
int alarmaCounter = 0;
// Tamaño del arreglo
int lengAlarmas = sizeof(Alarmas)/sizeof(Alarmas[0]);
// Variable de la fecha
Ds1302::DateTime now;

void setup(){
  Serial.begin(9600);

  // Iniciamos el RTC
  rtc.init();
  // Habilitamos el pin del Buzzer
  pinMode(buzzerPin, OUTPUT);
  // Habilitamos el pin del Botón
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop(){
  // Obtener la fecha del RTC
  rtc.getDateTime(&now);

  // Checar alarmas
  for(int i = 0; i < lengAlarmas; i++){
    if(now.hour == Alarmas[i][0] &&
       now.minute == Alarmas[i][1]){
      Serial.println("Ring");
      ring();
      noTone(buzzerPin);
      delay(40000);
      alarmaCounter = alarmaCounter % lengAlarmas;
    }
  }

  // Imprimir en el Monitor las fecha y hora
  /* static uint8_t last_second = 0; */
  /* if (last_second != now.second){ */
  /*   last_second = now.second; */

  /*   Serial.print("20"); */
  /*   Serial.print(now.year);    // 00-99 */
  /*   Serial.print('-'); */
  /*   if (now.month < 10) Serial.print('0'); */
  /*   Serial.print(now.month);   // 01-12 */
  /*   Serial.print('-'); */
  /*   if (now.day < 10) Serial.print('0'); */
  /*   Serial.print(now.day);     // 01-31 */
  /*   Serial.print(' '); */
  /*   Serial.print(WeekDays[now.dow - 1]); // 1-7 */
  /*   Serial.print(' '); */
  /*   if (now.hour < 10) Serial.print('0'); */
  /*   Serial.print(now.hour);    // 00-23 */
  /*   Serial.print(':'); */
  /*   if (now.minute < 10) Serial.print('0'); */
  /*   Serial.print(now.minute);  // 00-59 */
  /*   Serial.print(':'); */
  /*   if (now.second < 10) Serial.print('0'); */
  /*   Serial.print(now.second);  // 00-59 */
  /*   Serial.println(); */
  /* } */

  delay(100);
}

void ring(){
  // Diferencia entre los minutos actuales y de encendido de la alarma
  int diffMinutes;

  // Obtener la fecha del RTC
  rtc.getDateTime(&now);

  // Minuto en que inició la alarma
  uint8_t initMinute = now.minute;

  // Repetir infinitamente el sonido
  while(true){
    // Generar las ondas del sonido en el Buzzer
    for(int x=0; x<180; x++){
      sinVal = (sin(x * (3.1412 / 180)));
      toneVal = 1000 + (int(sinVal * 1000));

      // Activar el Buzzer con el tono
      tone(buzzerPin, toneVal);

      // Checar si el botón a sido pulsado
      buttonState = digitalRead(buttonPin);
      if (buttonState == LOW){
        return;
      }
    }

    // Obtener la fecha del RTC
    rtc.getDateTime(&now);

    // Detenemos la alarma si lleva MAX_MINUTES activa
    diffMinutes = now.minute - initMinute;
    if (diffMinutes >= MAX_MINUTES || diffMinutes < 0){
      return;
    }
  }
}
