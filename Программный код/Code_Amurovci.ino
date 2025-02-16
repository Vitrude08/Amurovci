#include "RotaryEncoder.h" 
RotaryEncoder encoder(A2, A3);
#include <Servo.h>
Servo myServo;
#define CW   1900
#define STOP 1450
#define CCW  1000
#define DIST  10
#define CM  1
#define INC 0
int shim;
int Trig_pin = 2;
int Echo_pin = 3;
int mld = 12; // направление левого мотора
int mlsp = 10; // скорость левого мотора
int mrd = 13; // направление правого мотора
int mrsp = 11; // скорость правого мотора
int ls = 6; // левый датчик линии
int cs = 7; // центральный датчик линии
int rs = 8; // правый датчик линии
int c = 0; // отсчёт тупиков
int v[3]; // массив под значения датчиков линии,слева направо
int dist; // значение датчика расстояния
int Color;
int pos = 0;  // задаем начальное положение энкодера
int newPos;
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8 // датчик цвета
int zv; // значение датчика цвета
void setup() {
  // put your setup code here, to run once:
  pinMode(mld, OUTPUT);
  pinMode(mlsp, OUTPUT);
  pinMode(mrd, OUTPUT);
  pinMode(mrsp, OUTPUT);
  pinMode(ls, INPUT);
  pinMode(cs, INPUT);
  pinMode(rs, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  pinMode(sensorOut, INPUT);
  myServo.writeMicroseconds(STOP);
  delay(1500);
}

void loop() {
  // put your main code here, to run repeatedly:
  v[0] = digitalRead(ls);
  v[1] = digitalRead(cs);
  v[2] = digitalRead(rs);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Color = pulseIn(sensorOut, LOW);
  encoder.tick();
  newPos = encoder.getPosition();
  //dist = ultrasonic.Ranging(CM); //считывание значений с датчиков
  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  dist = pulseIn(Echo_pin, HIGH);
  dist = dist / 58;
  if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 < 1) { //белый,чёрный,чёрный => поворот направо
    povorot(1); 
  }

  else if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //белый,чёрный,белый => ровный участок
    pryamo();
  }
  else if (v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //чёрный,чёрный,белый => поворот налево
    povorot(0);
  }
  if (v[0] * 2 > 1 && v[1] * 2 > 1 && v[2] * 2 > 1) { // белый,белый,белый => тупик => разворот
    stop();
    razvorot();
  }
  if (v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 < 1) { //чёрный,чёрный,чёрный => старт/финиш
    stop();
    if (dist < DIST) { // старт => взять цилиндр
      vzyat();
      c=1;
    }
    else { // финиш => поставить цилиндр
      myServo.writeMicroseconds(CCW);
      delay(500);
    }
  }
  if (dist < DIST) { //видит цилиндр
    stop();
    if (100 > Color > 50) { // красная
      obezd();
    }
    if (50 > Color > 0) { //синяя
      if (c==1){ // если брали зелёный цилиндр
        razvorot();
        while(not(v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 < 1)){ // движение до перекрёстка
          if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 < 1) { //белый,чёрный,чёрный => поворот направо
            povorot(1);
          }
          else if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //белый,чёрный,белый => ровный участок
            pryamo();
          }
          else if (v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //чёрный,чёрный,белый => поворот налево
            povorot(0);
          }
        }
        naz();
        myServo.writeMicroseconds(CCW);
        delay(500); // ставим зелёный цилиндр
        naz();
        razvorot();
        while(dist < DIST){ // двигаемся до синего цилиндра
          if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 < 1) { //белый,чёрный,чёрный => поворот направо
            povorot(1);
          }
          else if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //белый,чёрный,белый => ровный участок
            pryamo();
          }
          else if (v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //чёрный,чёрный,белый => поворот налево
            povorot(0);
          }
        }
        ubrat();
        razvorot();
        while(dist < DIST){ // двигаемся до зелёного цилиндра
          if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 < 1) { //белый,чёрный,чёрный => поворот направо
            povorot(1);
          }
          else if (v[0] * 2 > 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //белый,чёрный,белый => ровный участок
            pryamo();
          }
          else if (v[0] * 2 < 1 && v[1] * 2 < 1 && v[2] * 2 > 1) { //чёрный,чёрный,белый => поворот налево
            povorot(0);
          }
          vzyat();
          razvorot();
        }
      }
      else{ // если не брали зелёный цилиндр
        ubrat();
      }
    }
  }
  delay(50);
}
void pr() { //движение прямо для объезда и взятия цилиндра
  shim=1;
  digitalWrite(mld, 0);
  analogWrite(mlsp, 255);
  delay(200);
  digitalWrite(mrd, 0);
  analogWrite(mrsp, 255);
  delay(200);
  if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
}
void naz() { //движение назад для постановки цилиндра
  digitalWrite(mld, 1);
  analogWrite(mlsp, 255);
  delay(100);
  digitalWrite(mrd, 1);
  analogWrite(mrsp, 255);
  delay(100);
}
void levo() { //поворот налево для объезда
  digitalWrite(mld, 1);
  analogWrite(mlsp, 255);
  delay(100);
  digitalWrite(mrd, 0);
  analogWrite(mrsp, 255);
  delay(100);
  if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
}
void pravo() { //поворот направо для объезда
  digitalWrite(mld, 0);
  analogWrite(mlsp, 255);
  delay(100);
  digitalWrite(mrd, 1);
  analogWrite(mrsp, 255);
  delay(100);
  if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
}
void obezd() { //объезд
  delay(500);
  pravo();
  pr();
  levo();
  levo();
  pr();
  pravo();
}

void pryamo() { // движение прямо
  digitalWrite(mld, 0);
  analogWrite(mlsp, 255);
  digitalWrite(mrd, 0);
  analogWrite(mrsp, 255);
  if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
}
void nazad() { // движение назад
  digitalWrite(mld, 0);
  analogWrite(mlsp, 255);
  digitalWrite(mrd, 0);
  analogWrite(mrsp, 255);
}
void vzyat() { // взятие цилиндра
  myServo.writeMicroseconds(CCW);
  delay(500);
  pr();
  myServo.writeMicroseconds(CW);
  delay(700);
}
void postavit() { // постановка цилиндра
  naz();
  myServo.writeMicroseconds(CCW);
  delay(500);
  while(v[1]*2>1){
    nazad();
  }
  myServo.writeMicroseconds(CW);
  delay(700);
}
void ubrat() { // перестановка цилиндра
  delay(500);
  vzyat();
  pravo();
  pr();
  postavit();
  pravo();
}
void razvorot() { // разворот в зависимости от того, на чём стоял робот
  delay(500);
  if (v[1] * 2 > 1) { // стоял на белом
    while (v[1] * 2 > 1) { // поворот до тех пор, пока робот не встанет на линию
      digitalWrite(mld, 1);
      analogWrite(mlsp, 255);
      digitalWrite(mrd, 0);
      analogWrite(mrsp, 255);
      if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
    }
  }
  else { // стоял на чёрном
    while (v[1] * 2 < 1) { // поворот до тех пор, пока робот не выйдет с чёрной площадки
      digitalWrite(mld, 1);
      analogWrite(mlsp, 255);
      digitalWrite(mrd, 0);
      analogWrite(mrsp, 255);
      if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
    }
    while (v[1] * 2 > 1) { // поворот до тех пор, пока робот не встанет на линию
      digitalWrite(mld, 1);
      analogWrite(mlsp, 255);
      digitalWrite(mrd, 0);
      analogWrite(mrsp, 255);
      if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
    }
  }
}


void povorot(int p) { //поворот в направлении
  pryamo(); // едет прямо пока не выйдет за чёрную линию
  if (v[0] < 700 && v[1] < 700 && v[2] < 700) {
    if (!p) { // поворот налево
      while (v[1] < 700) {
        digitalWrite(mld, 1);
        analogWrite(mlsp, 255);
        digitalWrite(mrd, 0);
        analogWrite(mrsp, 255);
        if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
      }
    }
    else { // поворот направо
      while (v[1] < 700) {
        digitalWrite(mld, 0);
        analogWrite(mlsp, 255);
        digitalWrite(mrd, 1);
        analogWrite(mrsp, 255);
        if (pos==newPos) {
    naz();
    }
    else{
      pos=newPos;
    }
      }
    }
  }
}
void stop(){ //остановка робота
  digitalWrite(mld, 0);
  analogWrite(mlsp, 0);
  digitalWrite(mrd, 0);
  analogWrite(mrsp, 0);
}