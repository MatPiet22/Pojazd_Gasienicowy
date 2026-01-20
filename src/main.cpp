#include <Arduino.h>
#include "BluetoothSerial.h"
#include <ESP32Servo.h>
BluetoothSerial SerialBT;
Servo headServo;
#define M1_A 18
#define M1_B 19
#define M2_A 4
#define M2_B 5
#define SERVO_PIN 23
#define TRIG_PIN 21
#define ECHO_PIN 22

int current_speed = 125;
float distance = 999;
char command = ' ';
int speed = 175;
float measureDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0)
    return 999;

  return duration * 0.0343 / 2 + 1;
}
void stopAll()
{
  ledcWrite(8, 0);
  ledcWrite(9, 0);
  ledcWrite(10, 0);
  ledcWrite(11, 0);
}
void forward()
{
  ledcWrite(8, current_speed);
  ledcWrite(9, 0);
  ledcWrite(10, current_speed);
  ledcWrite(11, 0);
}
void backward()
{
  if (current_speed < 175)
  {
    speed = 175;
  }
  else
  {
    speed=current_speed;
  }
  ledcWrite(8, 0);
  ledcWrite(9, speed);
  ledcWrite(10, 0);
  ledcWrite(11, speed);
}
void left()
{
  if (current_speed < 175)
  {
    speed = 175;
  }
  else
  {
    speed=current_speed;
  }
  ledcWrite(8, 0);
  ledcWrite(9, speed);
  ledcWrite(10, speed);
  ledcWrite(11, 0);
}
void right()
{
  if (current_speed < 175)
  {
    speed = 175;
  }
  else
  {
    speed=current_speed;
  }
  ledcWrite(8, speed);
  ledcWrite(9, 0);
  ledcWrite(10, 0);
  ledcWrite(11, speed);
}
void setup()
{

  pinMode(M1_A, OUTPUT);
  digitalWrite(M1_A, LOW);
  pinMode(M1_B, OUTPUT);
  digitalWrite(M1_B, LOW);
  pinMode(M2_A, OUTPUT);
  digitalWrite(M2_A, LOW);
  pinMode(M2_B, OUTPUT);
  digitalWrite(M2_B, LOW);

  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  ESP32PWM::allocateTimer(3);
  headServo.setPeriodHertz(50);
  headServo.attach(SERVO_PIN, 500, 2400);
  headServo.write(90);

  ledcSetup(8, 1000, 8);
  ledcSetup(9, 1000, 8);
  ledcSetup(10, 1000, 8);
  ledcSetup(11, 1000, 8);

  ledcWrite(8, 0);
  ledcWrite(9, 0);
  ledcWrite(10, 0);
  ledcWrite(11, 0);

  ledcAttachPin(M1_A, 8);
  ledcAttachPin(M1_B, 9);
  ledcAttachPin(M2_A, 10);
  ledcAttachPin(M2_B, 11);
  SerialBT.begin("ESP32-Robot");

  headServo.write(60);
  stopAll();
}
void loop()
{
  if (SerialBT.available())
  {
    char c = SerialBT.read();
    if (c != '\n' && c != '\r')
    {
      command = c;
    }
  }
  distance = measureDistance();
  if (distance <= 25 && command == 'w')
  {
    stopAll();
    SerialBT.println("PRZESZKODA! Skanowanie...");
    headServo.write(60);
    delay(400);
    float distRight = measureDistance();
    SerialBT.print("Prawo: ");
    SerialBT.println(distRight);
    headServo.write(120);
    delay(500);
    float distLeft = measureDistance();
    SerialBT.print("Lewo: ");
    SerialBT.println(distLeft);
    headServo.write(90);
    delay(200);
    if (distRight > distLeft)
    {
      SerialBT.println("Wybrano: PRAWO");
      right();
      delay(500);
    }
    else
    {
      SerialBT.println("Wybrano: LEWO");
      left();
      delay(500);
    }
    forward();
    command = 'w';
  }
  switch (command)
  {
  case 'w':
    forward();
    break;
  case 's':
    backward();
    break;
  case 'a':
    left();
    break;
  case 'd':
    right();
    break;
  case ' ':
    stopAll();
    break;
  case 'z':
    headServo.write(10);
    break;
  case 'x':
    headServo.write(90);
    break;
  case 'c':
    headServo.write(170);
    break;
  case '+':
    current_speed += 25;
    if (current_speed > 255)
      current_speed = 255;
    SerialBT.print("Predkosc: ");
    SerialBT.println(current_speed);
    command = 'x';
    break;
  case '-':
    current_speed -= 25;
    if (current_speed < 0)
      current_speed = 0;
    SerialBT.print("Predkosc: ");
    SerialBT.println(current_speed);
    command = 'x';
    break;
  }
  delay(50);
}