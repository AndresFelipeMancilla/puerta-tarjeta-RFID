#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#define RST_PIN   6     // Cable RST del RFID al pin 6
#define SS_PIN    7     // Cable SDA (SS) del RFID al pin 7
#define LED_VERDE A0    // LED Verde en A0
#define LED_ROJO  A1    // LED Rojo en A1
#define SENSOR_IR 2     // Pin de datos del Infrarrojo
#define PIN_SERVO 4     // Servo cambiado al Pin 4
MFRC522 mfrc522(SS_PIN, RST_PIN);   
Servo miPuerta;         // Objeto para controlar el motor
// UID de tu tarjeta guardado en memoria
byte tarjetaMaestra[] = {0xF2, 0x49, 0x5B, 0x06}; 
void setup() {
  Serial.begin(9600);   
  while (!Serial); 
  SPI.begin();          
  mfrc522.PCD_Init();   
  // Iniciar Servo y mandarlo a la posición 0 (Cerrado) apenas encienda
  miPuerta.attach(PIN_SERVO);
  miPuerta.write(0); 
  // Configurar Pines
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(SENSOR_IR, INPUT);
  // Estado inicial: LEDs apagados
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);
  // Forzar potencia de antena para ayudar al voltaje
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println("=== MAQUETA LISTA: SEGURIDAD TOTAL ===");
}
void loop() {
  // 1. LÓGICA DEL SENSOR INFRARROJO
  if (digitalRead(SENSOR_IR) == LOW) {
    Serial.println("¡Alerta! Sensor Infrarrojo detectó presencia.");
    delay(200); // Pequeña pausa para no saturar el monitor
  }
  // 2. LÓGICA DEL LECTOR RFID
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return; 
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Comprobar si la tarjeta pasada coincide con el UID guardado
  bool accesoConcedido = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != tarjetaMaestra[i]) {
      accesoConcedido = false;
      break;
    }
  }
  // Acciones según la tarjeta
  if (accesoConcedido) {
    Serial.println("ACCESO CONCEDIDO. Abriendo puerta...");
    digitalWrite(LED_VERDE, HIGH);
    miPuerta.write(90);  // Abre la puerta a 90 grados
    delay(3000);         // Se mantiene abierta por 3 segundos
    miPuerta.write(0);   // Vuelve a cerrar a 0 grados
    digitalWrite(LED_VERDE, LOW);
  } else {
    Serial.println("ACCESO DENEGADO. Tarjeta incorrecta.");
    digitalWrite(LED_ROJO, HIGH);
    delay(1500); 
    digitalWrite(LED_ROJO, LOW);
  }
  mfrc522.PICC_HaltA();
}