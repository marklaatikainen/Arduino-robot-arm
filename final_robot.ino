/*
I have used some parts from 2007 Tod E. Kurt, http://todbot.com/blog/ code, last edited by Mark Tashiro.

pulseServo function is made by Tero Karvinen, terokarvinen.com / botbook.com



Mark Laatikainen, Haaga-Helia 06/2017

*/



#include <Wire.h>

// smoothing settings for servo 1 and 2.
// read 20 inputs and calculate average between them
const int numReadings = 20;
const int numReadingsservo2 = 20;

// servo 1
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int averagex = 0;                // the average

// servo 2
int readingsservo2[numReadingsservo2];      // the readings from the analog input
int readIndexservo2 = 0;              // the index of the current reading
int totalservo2 = 0;                  // the running total
int averagey = 0;                // the average

// variable for nunchuck's C-button
int c_button = 0;

// set pins for servos
int servo1Pin = 4;
int servo2Pin = 5;
int servo3Pin = 6;
int servo4Pin = 7;

// variables for storing values for smoothing
int newval1, oldval1;
int newval2, oldval2;


// move servo function
void pulseServo(int servoPin, int pulseLenUs)
{
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseLenUs);
  digitalWrite(servoPin, LOW);
  delay(10);
}

// variable for storing data from nunchuck
static uint8_t nunchuck_buf[6];


void setup()
{
  Serial.begin(19200);
  
  pinMode(servo1Pin, OUTPUT);
  pinMode(servo2Pin, OUTPUT);
  pinMode(servo3Pin, OUTPUT);
  pinMode(servo4Pin, OUTPUT);



for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
for (int thisReadingservo2 = 0; thisReadingservo2 < numReadingsservo2; thisReadingservo2++) {
    readingsservo2[thisReadingservo2] = 0;
  }

// emulate power and ground from A2 and A3 pins for nunchuck adapter
  nunchuck_setpowerpins();
  
  nunchuck_init();

}

void loop()
{
  nunchuck_get_data();

// map nunchuk data to a servo data point
  int x_axis = map(nunchuck_buf[0], 32, 231, 600, 2000);    // servo 4
  int y_axis = map(nunchuck_buf[1], 32, 231, 560, 2100);    // servo 3

  newval1 = map(nunchuck_buf[2], 215, 75, 800, 2100); // servo 1
  newval2 = map(nunchuck_buf[3], 75, 215, 800, 2100); // servo 2

  int z_button = 0;
if ((nunchuck_buf[5] >> 0) & 1)
    z_button = 1;


  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = newval1;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
  averagex = total / numReadings;
  pulseServo(servo1Pin, averagex);


  totalservo2 = totalservo2 - readingsservo2[readIndexservo2];
  // read from the sensor:
  readingsservo2[readIndexservo2] = newval2;
  // add the reading to the total:
  totalservo2 = totalservo2 + readingsservo2[readIndexservo2];
  // advance to the next position in the array:
  readIndexservo2 = readIndexservo2 + 1;

  // if we're at the end of the array...
  if (readIndexservo2 >= numReadingsservo2) {
    // ...wrap around to the beginning:
    readIndexservo2 = 0;
  }

  // calculate the average:
  averagey = totalservo2 / numReadingsservo2;


  pulseServo(servo2Pin, averagey);
  pulseServo(servo3Pin, y_axis);

if (z_button == 1) {
  pulseServo(servo4Pin, 2100);
  delay(5); 
}
else {
  pulseServo(servo4Pin, 560);
  delay(5);
  }
}

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize        
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
void nunchuck_init()
{ 
  Wire.begin();                      // join i2c bus as master
  Wire.beginTransmission(0x52);     // transmit to device 0x52
  Wire.write(0x40);            // sends memory address
  Wire.write(0x00);            // sends sent a zero.  
  Wire.endTransmission();     // stop transmitting
}

// Send a request for data to the nunchuck
// was "send_zero()"
void nunchuck_send_request()
{
  Wire.beginTransmission(0x52);     // transmit to device 0x52
  Wire.write(0x00);            // sends one byte
  Wire.endTransmission();     // stop transmitting
}

// Receive data back from the nunchuck, 
int nunchuck_get_data()
{
    int cnt=0;
    Wire.requestFrom (0x52, 6);     // request data from nunchuck
    while (Wire.available ()) {
      // receive byte as an integer
      nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.read());
      cnt++;
    }
    nunchuck_send_request();  // send request for next data payload
    // If we recieved the 6 bytes, then go print them
    if (cnt >= 5) {
     return 1;   // success
    }
    return 0; //failure
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}
