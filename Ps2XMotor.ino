#include <Servo.h>
#include <PS2X_lib.h>  //for v1.6


/******************************************************************
 * set pins connected to PS2 controller:
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT 8   //14    This pin will need a pull up resistor
#define PS2_CMD 11  //15
#define PS2_SEL 10  //16
#define PS2_CLK 12  //17

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures false
//#define rumble      true
#define rumble false

PS2X ps2x;  // create PS2 Controller Class
Servo esc1, esc2, esc3, esc4;

//mills
unsigned long stms1, stms2, stms3;
unsigned long crms1, crms2, crms3;

int error = 0;
byte type = 0;
byte vibrate = 0;

int ps2kr;
int ps2kn;
int mapkr;
int mapkn;

int speed = 30;
int stskn = 0;
int stskr = 0;
int mskn = 30;
int mskr = 30;

int ms1 = 50;
int ms2 = 70;

void setup() {

  Serial.begin(9600);

  //motor
  esc1.attach(6);
  esc1.write(20);
  esc2.attach(5);
  esc2.write(20);
  esc3.attach(3);
  esc3.write(20);
  delay(3000);

  //mills
  stms1 = millis();
  stms2 = millis();
  stms3 = millis();

  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it

  //CHANGES for v1.6 HERE!!! ***********PAY ATTENTION**********

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  } else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  //  Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
  //Motor
}

void loop() {
  if (error == 1)  //skip loop if no controller found
    return;

  if (type == 2) {                      //Guitar Hero Controller
    ps2x.read_gamepad();                //read controller
  } else {                              //DualShock Controller
    ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button

    //ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1); //print stick values if either is TRUE
    //Serial.print("Stick Values:");
    //Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX

    crms2 = millis();
    crms3 = millis();

    //Joystick Kiri
    ps2kr = ps2x.Analog(PSS_LY);
    if (ps2kr >= 132) {
      ps2kr = 132;
    }
    mapkr = map(ps2kr, 132, 0, 30, 85);

    //Joystick Kanan
    ps2kn = ps2x.Analog(PSS_RY);
    if (ps2kn >= 123) {
      ps2kn = 123;
    }
    //2 Controller
    if (mapkr >= mskr && mapkn >= mskn) {
      mapkr = mskr;
      mapkn = mskn;
      if (stskr == 0) {
        if (crms2 - stms2 >= 1000) {
          mskr = 60;
          stskr = 1;
          stms2 = crms2;
        }
      }
      if (stskn == 0) {
        if (crms3 - stms3 >= 1000) {
          mskn = 60;
          stskn = 1;
          stms3 = crms3;
        }
      }
      if (stskr == 1) {
        if (crms2 - stms2 >= 1000) {
          mskr = 75;
          stskr = 2;
          stms2 = crms2;
        }
      }
      if (stskn == 1) {
        if (crms3 - stms3 >= 1000) {
          mskn = 75;
          stskn = 2;
          stms3 = crms3;
        }
      }
      if (stskr == 2) {
        if (crms2 - stms2 >= 1000) {
          mskr = 85;
          stskr = 3;
          stms2 = crms2;
        }
      }
      if (stskn == 2) {
        if (crms3 - stms3 >= 1000) {
          mskn = 85;
          stskn = 3;
          stms3 = crms3;
        }
      }
    }
    if (mapkr <= 35) {
      stskr = 0;
      mskr = mskr;
    }
    if (mapkn <= 35) {
      stskn = 0;
      mskn = mskn;
    }

    //Joystik Kiri
    if (mapkr >= mskr) {
      mapkr = mskr;
      if (stskr == 0) {
        if (crms2 - stms2 >= 1000) {
          mskr = 60;
          stskr = 1;
          stms2 = crms2;
        }
      }
      if (stskr == 1) {
        if (crms2 - stms2 >= 1000) {
          mskr = 75;
          stskr = 2;
          stms2 = crms2;
        }
      }
      if (stskr == 2) {
        if (crms2 - stms2 >= 1000) {
          mskr = 85;
          stskr = 3;
          stms2 = crms2;
        }
      }
    }
    // else{
    //   stskr = 0;
    //   mskr = 40;
    // }
    Serial.print(mapkr);
    esc1.write(mapkr);

    //Joystik Kanan
    mapkn = map(ps2kn, 123, 0, 30, 85);
    if (mapkn >= mskn) {
      mapkn = mskn;
      if (stskn == 0) {
        if (crms3 - stms3 >= 1000) {
          mskn = 60;
          stskn = 1;
          stms3 = crms3;
        }
      }
      if (stskn == 1) {
        if (crms3 - stms3 >= 1000) {
          mskn = 75;
          stskn = 2;
          stms3 = crms3;
        }
      }
      if (stskn == 2) {
        if (crms3 - stms3 >= 1000) {
          mskn = 85;
          stskn = 3;
          stms3 = crms3;
        }
      }
    }
    // else{
    //   stskn = 0;
    //   mskn = 40;
    // }
    Serial.print(mapkn);
    esc2.write(mapkn);

    //StartMillis
    crms1 = millis();
    if (crms1 - stms1 >= 100) {
      //Kotak
      if (ps2x.Button(PSB_CROSS)) {
        if (speed == 0) {
          speed = 30;
        } else {
          speed += 1;
          if (speed >= 85) {
            speed = 85;
          }
        }
      }
      //Bulat
      if (ps2x.Button(PSB_CIRCLE)) {
        speed -= 1;
        if (speed <= 30) {
          speed = 30;
        }
      }
      if (ps2x.Button(PSB_START)) {
        speed = 0;
      }
      esc3.write(speed);
      Serial.println(speed);
      stms1 = crms1;
    }
  }
  delay(50);
}