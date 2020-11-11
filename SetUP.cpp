#include <SoftwareSerial.h> //블루투스 라이브러리
char btInVal; //블루투스 입력값
SoftwareSerial BTSerial(14, 15); //TX:2, RX:3

#include <Arduino_APDS9960.h> //RGB컬러센서

#include "Arduino.h"
#include "SetUp.h" //함수 선언 되어 있어서

#define CAR_AUTO_Drive 0
#define CAR_BACK 2

int laSen1 = 5; //우측 적외선 센서
int laSen2 = 4; //좌측 적외선 센서
int laSen3 = 3; //좌측 적외선 센서
int laSen4 = 2; //좌측 적외선 센서
int laSen5 = A3; //center 적외선 센서
int laValue1 = 0;
int laValue2 = 0;
int laValue3 = 0;
int laValue4 = 0;
int laValue5 = 0;

int raySen1 = A2; //원거리 적외선 센서
int rayValue;

int moSpeed1 = 10; //모터  속도 센서
int moSpeed2 = 11; //모터 속도 센서
int moValue1 = 0; //모터 속도 값
int moValue2 = 0; //모터 속도 값

int trigger = 12;  //초음파 발사
int echo = 13; //초음파 감지
int distance; //초음파 거리

int c_num = 0;

void carRight();
void carRightS();
void carLeft();
void carLeftS();
void carStop();
void carStraight();
void carStraightS();
void carBack();
void serialPrint();

void SetUp_a() {
  Serial.begin(9600);
  BTSerial.begin(9600);  //블루투스 시리얼 개방

  pinMode(6, OUTPUT); //우측 모터 출력 전진
  pinMode(7, OUTPUT); //우측 모터 출력 후진
  pinMode(8, OUTPUT); //좌측 모터 출력 전진
  pinMode(9, OUTPUT); //좌측 모터 출력 후진

  pinMode(laSen1, INPUT); //우측 적외선 센서
  pinMode(laSen2, INPUT); //좌측 적외선 센서
  pinMode(laSen3, INPUT); //좌측 적외선 센서
  pinMode(laSen4, INPUT); //좌측 적외선 센서
  pinMode(laSen5, INPUT); //center 적외선 센서

  pinMode(raySen1, INPUT); //원거리용 적외선 센서

  pinMode(moSpeed1, OUTPUT); //모터 속도 센서
  pinMode(moSpeed2, OUTPUT); //모터 속도 센서

  pinMode(trigger, OUTPUT);  //초음파 발사
  pinMode(echo, INPUT); //초음파 감지

    while (!Serial); //RGB컬러센서

  if (!APDS.begin()) {
    Serial.println("APDS9960센서 오류!");
    while (1);
  }
}



void loop_a() {
  if (Serial.available()) {
    //ray=Serial.read();
  }
  //Serial.println("Adoino2 적외선 센서");
  // Serial.println(ray);


  if (APDS.colorAvailable()) { //RGB컬러센서
    int RGB[3] = {0};
    APDS.readColor(RGB[0], RGB[1], RGB[2]);
    Serial.print("R=");
    Serial.println(RGB[0]);
    Serial.print("G=");
    Serial.println(RGB[1]);
    Serial.print("B=");
    Serial.println(RGB[2]);
    Serial.println("----------------");
    //delay(500);
  }


  rayValue = digitalRead(raySen1); //우측 적외선 센서 입력값 읽음

  if (BTSerial.available()) {   // 값이 있으면
    btInVal = BTSerial.read();
  }

  laValue1 = digitalRead(laSen1); //우측 적외선 센서 입력값 읽음
  laValue2 = digitalRead(laSen2); //좌측 적외선 센서 입력값 읽음
  laValue3 = digitalRead(laSen3); //좌측 적외선 센서 입력값 읽음
  laValue4 = digitalRead(laSen4); //좌측 적외선 센서 입력값 읽음
  laValue5 = digitalRead(laSen5); //좌측 적외선 센서 입력값 읽음
  rayValue = digitalRead(raySen1); //원거리용 적외선 센서 입력값 읽음


  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  distance = pulseIn(echo, HIGH) / 58; //초음파 소리 내기

  distance = map(distance, 0, 20, 0, 255);
  Serial.println("Distance=" + String(distance));

  if (btInVal == 'f') { //Line Track 전진
    analogWrite(moSpeed1, 160);
    analogWrite(moSpeed2, 160);
    //Serial.println("수동 전진");
    Serial.write('g');

    carStraight();
  }

  else if (btInVal == 'r') { //Line Track 우회전
    //Serial.println("수동 우회전");
    analogWrite(moSpeed1, 180); //좌측바퀴
    analogWrite(moSpeed2, 100);
    carRight();
  }

  else if (btInVal == 'l') { //Line Track 좌회전
    analogWrite(moSpeed1, 100); //좌측바퀴
    analogWrite(moSpeed2, 180);
    carLeftS();
    //Serial.println("수동 좌회전");
    carLeft();
  }

  else if (btInVal == 'b') { //Line Track 후전
    analogWrite(moSpeed1, 180); //좌측바퀴
    analogWrite(moSpeed2, 180);
    //Serial.println("수동 후전");
    Serial.write('b');
    carBack();
  }

  else if (btInVal == 's') { //Line Track 정지
    //Serial.println("수동 정지");
    carStop();
  }
  //화물적재 정차
  else if (laValue1 == 1 && laValue4 == 1 && laValue5 == 1)
  { //조건이 1이면 서행
    carStop();
  } // 화물적재 정차 end



  //-----------------------------------=]--------------------------------------------------
  else if (btInVal == '0') { //Line Track 정지
    //Serial.println("Line Track 모터 정지");
    carStop();
    Serial.write('s');
  }

  else if (btInVal == '1') { //Line Track 출발
    //Serial.println("Line Track 출발");
    Serial.write('g');

      if (distance < 150) { //180mm 이하시 정지
    //Serial.println("자동모드 충돌방지에 의한 모터 정지");
    carStop();
      }

   else if (rayValue == 0) {
      // Max 우회전
      if (laValue1 == 1) {
        if (rayValue != 1) {
          analogWrite(moSpeed1, 170); //좌측바퀴 230
          analogWrite(moSpeed2, 140); //190
          carRight();
          //Serial.println("Max 우회전");
        }
        else if (rayValue == 1) {
          return;
        }
      }


      //Max 좌회전
      else if (laValue4 == 1) { //우측으로 쏠리면 좌회전~~
        if (rayValue != 1) {
          analogWrite(moSpeed1, 140); //좌측바퀴 190
          analogWrite(moSpeed2, 170); //230
          carLeft();
          //Serial.println("Max 좌회전");
        }
        else if (rayValue == 1) {
          return;
        }
      }


      // 약 우회전
      else if (laValue2 == 1)
      {

        analogWrite(moSpeed1, 150); //좌측바퀴 150-->255
        analogWrite(moSpeed2, 135); //우측바퀴 //120-->150
        carStraight();
        //Serial.println("약 우회전");
      }

      //약 좌회전
      else if (laValue3 == 1) { //우측으로 쏠리면 좌회전~~
        analogWrite(moSpeed1, 130); //좌측바퀴  250-->255
        analogWrite(moSpeed2, 140); //130-->150
        carStraight();
        //Serial.println("약 좌회전");

      }


      //직진
      else if ((laValue5 == 1)
               || (laValue2 == 1 && laValue5 == 1)
               || (laValue3 == 1 && laValue5 == 1)
               || (laValue2 == 1 && laValue3 == 1 && laValue5 == 1))
      { //조건이 1이면 전진
        analogWrite(moSpeed1, 135); //130
        analogWrite(moSpeed2, 125); //130
        carStraight();
        //Serial.println("5번 없는 직진");
      }
      /*
            //후진
        else if (laValue1 == 0 && laValue2 == 0
               && laValue3 == 0 && laValue4 == 0 && laValue5 == 0 && rayValue == 0) { //조건이 0이면 후진

        analogWrite(moSpeed1, 170); //150
        analogWrite(moSpeed2, 170);
        carBack();
        }
      */
    }// if (rayValue == 0) 끝


    //-----------------------------------------------------------------------------------------------


    else if (rayValue == 1) { //고속 직진

      if ((laValue3 == 0 && laValue5 == 1 && laValue2 == 0 && rayValue == 1)
          || (laValue1 == 0 && laValue2 == 1 && laValue5 == 1 && laValue3 == 0 && laValue4 == 0)
          || (laValue2 == 0 && laValue3 == 1 && laValue4 == 0 && laValue5 == 1)
          || (laValue1 == 0 && laValue2 == 1 && laValue3 == 1 && laValue4 == 0 && laValue5 == 1)) { //조건이 1이면 전진
        analogWrite(moSpeed1, 180); //230
        analogWrite(moSpeed2, 160); //200
        carStraight();
        //Serial.println("고속모두 직진");
        Serial.write('m');
      } //if end

      //약 직진
      else if ((laValue1 == 0 && laValue2 == 0 && laValue3 == 0 && laValue4 == 0 && laValue5 == 0)
               || (laValue1 == 1 && laValue2 == 0)
               || (laValue1 == 0 && laValue2 == 1 && laValue3 == 0)
               || (laValue3 == 1)
               || (laValue4 == 1)) { //조건이 1이면 전진
        analogWrite(moSpeed1, 160); //170
        analogWrite(moSpeed2, 140); //150
        carStraight();
        //Serial.println("고속모드 약직진");
      }

    } // else if (rayValue == 1) { //고속 직진 "끝"
 
  } // else if (btInVal == '1') { //Line Track 출발 ""끝"""




} // loop end


//함수----------------------------------------------------

void carRight() {
  digitalWrite(8, HIGH);//좌측 바퀴 전진
  digitalWrite(9, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);//우측 바퀴 후진

}

void carRightS() {
  if (c_num < 1) {
    digitalWrite(8, HIGH); //좌측 바퀴 전진
    digitalWrite(9, LOW); //좌측 --
    digitalWrite(6, HIGH); //우측 바퀴 전진
    digitalWrite(7, LOW); //우측 --
    c_num++;
  }
  else
  {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    c_num = 0;
  }

}


void carLeft() {
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH); //좌측 바퀴 후진
  digitalWrite(6, HIGH); //우측 바퀴 전진
  digitalWrite(7, LOW);
}

void carLeftS() {
  if (c_num < 1) {
    digitalWrite(8, HIGH); //좌측 바퀴 전진
    digitalWrite(9, LOW); //좌측바퀴
    digitalWrite(6, HIGH); //우측 바퀴 전진
    digitalWrite(7, LOW);  //우측 바퀴
    c_num++;
  }
  else
  {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    c_num = 0;
  }
}

void carStop() {
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  Serial.println("정지 정상 작동중");
}


void carStraight() {

  digitalWrite(6, HIGH); //우측바퀴 전진
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH); //좌측바퀴 전진
  digitalWrite(9, LOW);
  // Serial.println("전진 정상 작동중");

}
void carStraightS() {
  if (c_num < 2) {
    digitalWrite(6, HIGH); //우측바퀴 전진
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH); //좌측바퀴 전진
    digitalWrite(9, LOW);
    //Serial.println("전진 정상 작동중");
    c_num++;
  }
  else
  {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    c_num = 0;
  }
}


void carBack() {
  if (c_num < 4) {
    digitalWrite(6, LOW); //우측바퀴 전진
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW); //좌측바퀴 전진
    digitalWrite(9, HIGH);
    c_num++;
  }
  else
  {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    c_num = 0;
  }
}
/*
  void serialPrint() {
  Serial.print("원거리용 적외선 센서:");
  Serial.println(String(rayValue));
  Serial.println("laValue1:" + String(laValue1));
  Serial.println("laValue2:" + String(laValue2));
  Serial.println("laValue3:" + String(laValue3));
  Serial.println("laValue4:" + String(laValue4));
  Serial.println("laValue5:" + String(laValue5));

  Serial.print("블루투스 입력 값:: ");
  Serial.println(btInVal);
  }
*/
