
#include <Wire.h>
#include <VirtualWire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

int ledPin=13;//led on pin 13 is ON except when transmitter is parallel to the ground

void setup() 
{
  vw_setup(2000);//Bits per second
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);//Initialise the serial connection debugging
  Wire.begin();
  setupMPU();
}



void loop() 
{
  recordAccelRegisters();
  int xval = gForceX;
  int yval = gForceY;
  
  if ((xval>370 && xval<385) && (yval>360 && yval<380)) //stationary or stop(transmitter parallel to ground)
  {
    digitalWrite(ledPin,LOW);
    send("s");
  } 
  else 
  { 
    if ((xval>395 && xval<416) && (yval>360 && yval<380)) //forward(transmitter tilted forward)
    {
      digitalWrite(ledPin,HIGH);
      send("f");
      
    }
    if ((xval>345 && xval<365) && (yval>365 && yval<381)) //backward(transmitter tilted backward)
    {
      digitalWrite(ledPin,HIGH);
      send("a");
      
    }
    if ((xval>370 && xval<380) && (yval>335 && yval<355)) //left(transmitter tilted to left)
    {
      digitalWrite(ledPin,HIGH);
      send("l");
     }
     if ((xval>370 && xval<390) && (yval>385 && yval<395))//right(transmitter tilted to right)
    {
      digitalWrite(ledPin,HIGH);
      send("r");
      
    }
  }
  
}


void send(char *message)//send function definition
{
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx(); // Wait until the whole message is gone
}

void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  

  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}




