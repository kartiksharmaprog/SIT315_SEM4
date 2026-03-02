#include <avr/interrupt.h>

// Pin Definitions
#define PIR_PIN 8
#define BUTTON_PIN 9

#define TRIG_PIN 3
#define ECHO_PIN 2

#define LED_PIR 5
#define LED_BUTTON 6
#define LED_DISTANCE 10
#define HEARTBEAT 13

// GLOBAL VARIABLES
volatile bool pirFlag = false;
volatile bool buttonFlag = false;
volatile bool timerFlag = false;

volatile bool pciTriggered = false;
volatile bool extTriggered = false;

volatile unsigned long echoStart = 0;
volatile unsigned long echoEnd = 0;
volatile bool distanceReady = false;

unsigned long uptime = 0;

// PCI SETUP
void setupPCI() {
  PCICR |= (1 << PCIE0);      
  PCMSK0 |= (1 << PCINT0);    
  PCMSK0 |= (1 << PCINT1);    
}

// TIMER SETUP 
void setupTimer1() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;

  OCR1A = 15624;              
  TCCR1B |= (1 << WGM12);     
  TCCR1B |= (1 << CS12) | (1 << CS10); 
  TIMSK1 |= (1 << OCIE1A);    

  sei();
}

// SETUP 
void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_PIR, OUTPUT);
  pinMode(LED_BUTTON, OUTPUT);
  pinMode(LED_DISTANCE, OUTPUT);
  pinMode(HEARTBEAT, OUTPUT);

  setupPCI();
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);
  setupTimer1();

  Serial.println("=== Interrupt Driven System Started ===");
}

// ISR: PCI 
ISR(PCINT0_vect) {
  pirFlag = digitalRead(PIR_PIN);
  buttonFlag = digitalRead(BUTTON_PIN);
  pciTriggered = true;
}

// ISR: ULTRASONIC 
void echoISR() {
  if (digitalRead(ECHO_PIN)) {
    echoStart = micros();
  } else {
    echoEnd = micros();
    distanceReady = true;
    extTriggered = true;
  }
}

// ISR: TIMER 
ISR(TIMER1_COMPA_vect) {
  timerFlag = true;
}

// HELPER 
void triggerUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

// LOOP 
void loop() {

  // PCI EVENT 
  if (pciTriggered) {
    pciTriggered = false;

    if (pirFlag) {
      digitalWrite(LED_PIR, HIGH);
      Serial.println("[PCI] PIR Motion Detected → LED_PIR ON");
    } else {
      digitalWrite(LED_PIR, LOW);
      Serial.println("[PCI] PIR Cleared → LED_PIR OFF");
    }

    if (buttonFlag) {
      digitalWrite(LED_BUTTON, HIGH);
      Serial.println("[PCI] Button Pressed → LED_BUTTON ON");
    } else {
      digitalWrite(LED_BUTTON, LOW);
      Serial.println("[PCI] Button Released → LED_BUTTON OFF");
    }
  }

  // ULTRASONIC EVENT 
  if (distanceReady) {
    distanceReady = false;

    float distance = (echoEnd - echoStart) * 0.034 / 2;

    Serial.print("[EXT INT] Distance Measured: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < 20) {
      digitalWrite(LED_DISTANCE, HIGH);
      Serial.println("[ULTRA] Object Close → LED_DISTANCE ON");
    } else {
      digitalWrite(LED_DISTANCE, LOW);
      Serial.println("[ULTRA] Object Far → LED_DISTANCE OFF");
    }
  }

  // TIMER EVENT 
  if (timerFlag) {
    timerFlag = false;
    uptime++;

    digitalWrite(HEARTBEAT, !digitalRead(HEARTBEAT));

    Serial.print("[TIMER] 1 Second Tick → Uptime: ");
    Serial.println(uptime);

    triggerUltrasonic();
  }
}
