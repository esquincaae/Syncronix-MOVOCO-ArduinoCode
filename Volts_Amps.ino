#include <Wire.h>  
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  
#include <math.h>

// Configura el LCD con el número de la interfaz y los pines de control
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7);  // DIR, E, RW, RS, D4, D5, D6, D7

double sensorValue1 = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;


// Definir el pin al que está conectado el sensor ACS712
#define SENSOR_PIN A0
// Valor de la tensión de referencia para el ADC. Para Arduino UNO es 5V
#define VREF 5.0
// Valor de sensibilidad del sensor ACS712 20A es 100 mV/A, para el de 30A es 66 mV/A
#define SENSITIVITY 0.185

// Número de muestras para el promedio móvil
#define NUM_SAMPLES 10

void setup() {
  Serial.begin(9600);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2); // inicializar el LCD con 16 columnas y 2 filas
}

void loop() {
  //float Irms = 
  calculateIrms(NUM_SAMPLES); // Calcular corriente RMS
  //lcd.setCursor(6, 0);
  //lcd.print(Irms);
  //lcd.print(" A"); // Imprimir el valor de la corriente en la LCD
  //delay(1);
  calculateVolt();
}

void calculateVolt(){
    for ( int i = 0; i < 100; i++ ) {
        sensorValue1 = analogRead(A1); // Lectura del sensor en el pin A0
        if (analogRead(A1) > 511) {
            val[i] = sensorValue1; // Guarda el valor si es mayor que 511
        }
        else {
            val[i] = 0; // En otro caso, guarda 0
        }
        delay(5000);
    }

    max_v = 0;

    for ( int i = 0; i < 100; i++ ){
        if ( val[i] > max_v )
        {
            max_v = val[i]; // Encuentra el valor máximo
        }
        val[i] = 0;
    }

    if (max_v != 0) {
        VmaxD = max_v;
        VeffD = VmaxD / sqrt(2); // Calcula el valor RMS
        Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2; // Conversión a voltaje
    }
    else {
        Veff = 0;
    }

    // Coloca el cursor en la primera columna y segunda fila de la LCD
    lcd.setCursor(0, 2);
    // Imprime el voltaje en la LCD
    lcd.print("Volts: "); 
    lcd.print(Veff); //muestra en LCD
    lcd.print(" V");
    //Serial.print("Volts");
    //Serial.println(Veff); //muestra en Monitor

    VmaxD = 0;

    delay(100);
}

// Función para calcular la corriente RMS
float calculateIrms(int num_samples) {
  float sum = 0;
  for (int i = 0; i < num_samples; i++) {
    float value = analogRead(SENSOR_PIN);
    // Convertir a corriente
    float I = ((VREF / 2.0) - (value / 1024.0) * VREF) / SENSITIVITY;
    sum += I * I; // sumar el cuadrado de la corriente
    delay(5000); // Pequeña pausa para estabilización

  }
  float irms = sqrt(sum / (float)num_samples); // Calcular la raíz cuadrada de la media
  lcd.setCursor(6, 0);
  lcd.print("Amps: ");
  lcd.print(irms);
  lcd.print(" A"); // Imprimir el valor de la corriente en la LCD
}

