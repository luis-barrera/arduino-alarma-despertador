#include <Arduino.h>
#include <Ds1302.h>  // Librería de RTC (Módulo que guarda la fecha y hora)
#include <PCM.h>

// Definición de las ondas para cada nota en el buzzer
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// change this to make the song slower or faster
int tempo = 200;
// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {
  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4,
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
};
// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
// Calcular la duración de una nota en ms
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

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
  pinMode(buttonPin, INPUT);
}

void loop(){
  // Checar si el botón a sido pulsado
  if (digitalRead(buttonPin) == HIGH){
    /* Serial.println("Button"); */
    playSong();
  }

  // Obtener la fecha del RTC
  rtc.getDateTime(&now);

  // Checar alarmas
  for(int i = 0; i < lengAlarmas; i++){
    if(now.hour == Alarmas[i][0] &&
       now.minute == Alarmas[i][1]){
      /* Serial.println("Ring"); */
      ring();
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
  delay(400);

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
      if (digitalRead(buttonPin) == HIGH){
        noTone(buzzerPin);
        delay(60000);
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

void playSong(){
  delay(400);

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzerPin, melody[thisNote], noteDuration * 0.9);

    // Checar si el botón a sido pulsado
    if (digitalRead(buttonPin) == HIGH){
      /* Serial.println("Song Out"); */
      noTone(buzzerPin);
      delay(400);
      return;
    }

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzerPin);
  }
}
