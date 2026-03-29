#include <Servo.h>

#define TRIGGER_PIN 12 
#define ECHO_PIN 13    
#define BUZZER_PIN 3   
#define SERVO_PIN 9    

Servo doorServo;

// --- ตัวแปรสำหรับระบบ Trigger ---
bool isOpen = false;      // สถานะหน้าต่าง (false=ปิด, true=เปิด)
int swipeCount = 0;       // ตัวนับจำนวนการปัด
int threshold = 10;       // ระยะตรวจจับ (ซม.)

void dingdong() {
  tone(BUZZER_PIN, 523); delay(150);
  tone(BUZZER_PIN, 659); delay(150);
  noTone(BUZZER_PIN);
}

void beep() {
  tone(BUZZER_PIN, 800); delay(100); // เสียงติ๊ดสั้นๆ เวลานับการปัด
  noTone(BUZZER_PIN);
}

void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  doorServo.attach(SERVO_PIN);
  doorServo.write(0); 
}

void loop() {
  long duration, distance;

  // อ่านค่าจาก Ultrasonic
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration / 58.2;

  // ตรวจจับเมื่อมีมือปัดผ่าน (ระยะน้อยกว่า threshold)
  if (distance > 0 && distance <= threshold) {
    
    if (!isOpen) {
      // --- กรณีหน้าต่างปิดอยู่: ปัด 1 ครั้ง เปิดทันที ---
      dingdong();
      doorServo.write(90);
      isOpen = true;
      swipeCount = 0; // รีเซ็ตตัวนับการปัดเพื่อเตรียมปิด
    } 
    else {
      // --- กรณีหน้าต่างเปิดอยู่: ต้องปัดให้ครบ 2 ครั้งถึงจะปิด ---
      swipeCount++; 
      
      if (swipeCount < 2) {
        beep(); // เสียงเตือนว่านับครั้งที่ 1 แล้วนะ
      } 
      else {
        // ครบ 2 ครั้งแล้ว ปิดหน้าต่าง
        dingdong();
        doorServo.write(0);
        isOpen = false;
        swipeCount = 0; // รีเซ็ตตัวนับกลับไปเริ่มใหม่
      }
    }

    // รอจนกว่าจะเอามือออก (ป้องกันการนับซ้ำจากการปัดค้างไว้)
    while (distance <= threshold) {
      digitalWrite(TRIGGER_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
      distance = duration / 58.2;
      delay(50);
    }
    delay(200); // หน่วงเวลาเล็กน้อยเพื่อให้จังหวะการปัดแม่นยำขึ้น
  }

  delay(50);
}