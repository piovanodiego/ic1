/*
   Informe de Trabajo Final
   Ingeniería en Computación 1 (IC - UNRAF)
   Año 2024
   Desarrollo de un dispositivo experimental basado en Arduino
   para explorar y comprender el movimiento circular uniforme
   y sus conceptos asociados.
   Bataglia, Lucas Xavier, Piovano, Diego Ramón
*/

// Definición de pines y variables
const int sensorPin = 2;       // Pin digital conectado al sensor Hall
const int potangPin = A0;      // Pin analógico conectado al potenciómetro
int salida_PWM = 3;             // Declaramos el pin 3 como salida PWM
String readString;              // Creamos una variable de tipo string
volatile int vueltas = 0;       // Contador de vueltas
unsigned long tiempoAnterior = 0; // Tiempo en milisegundos desde el último cálculo de RPM
float rpm = 0;
float velocidadAngular = 0;
bool estadoAnterior = LOW;      // Estado anterior del sensor Hall
int valorpotang = 0;

// Librerías necesarias
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Definición de la pantalla OLED
#define ANCHO 128
#define ALTO 64
#define OLED_RESET 4
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);

// Variables para cálculos físicos
float radio = 0.05;
float velocidad_lineal;
float aceleracion_centripeta;

// Configuración inicial
void setup() {
  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(sensorPin, INPUT);
  pinMode(salida_PWM, OUTPUT);
  digitalWrite(sensorPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(sensorPin), contarVueltas, CHANGE);
  Serial.begin(9600);
}

// Bucle principal
void loop() {
  // Lectura de comandos por el puerto serial
  char dato = Serial.read();
  switch (dato) {
    case 'A':
      velocidad();
      break;
  }

  // Cálculo de RPM y velocidad angular
  unsigned long tiempoActual = millis();
  unsigned long tiempoTranscurrido = tiempoActual - tiempoAnterior;

  if (tiempoTranscurrido >= 1500) {
    rpm = (vueltas * 60000.0) / tiempoTranscurrido;
    velocidadAngular = rpm * (PI / 30);

    vueltas = 0;
    tiempoAnterior = tiempoActual;
  }

  // Cálculo de velocidad lineal y aceleración centrípeta
  double omega = (rpm * 2 * PI) / 60.0;
  velocidad_lineal = radio * omega;
  aceleracion_centripeta = radio * (omega * omega);

  // Mostrar datos en la pantalla OLED
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(2);
  oled.print("IC-1 UNRaf");
  oled.setCursor(0, 20);
  oled.setTextSize(1);
  oled.print(rpm);
  oled.println(" RPM");

  oled.print("Vel.A: ");
  oled.print(velocidadAngular);
  oled.println(" rad/s");

  oled.print("vel. lineal: ");
  oled.print(velocidad_lineal, 2);
  oled.println(" m/s");

  oled.print("Ac. w: ");
  oled.print(aceleracion_centripeta, 2);
  oled.println(" m/s2");

  oled.display();
}

// Función para contar vueltas del sensor Hall
void contarVueltas() {
  bool estadoActual = digitalRead(sensorPin);

  if (estadoActual != estadoAnterior) {
    if (estadoActual == HIGH) {
      vueltas++;
    }
    estadoAnterior = estadoActual;
  }
}

// Función para ajustar la velocidad del motor
void velocidad() {
  delay(15);
  while (Serial.available()) {
    char C = Serial.read();  // Almacenamos la lectura serial en la variable C
    readString += C;         // Sumamos la cadena de caracteres + la variable C
  }
  if (readString.length() > 0) {
    analogWrite(salida_PWM, (readString.toInt())); // Convierte la cadena a entero y ajusta la velocidad
    readString = "";
  }
}