#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = A0, TXPin = A1;
static const uint32_t GPSBaud = 9600;
long time = 0;
bool changed = 0;
bool wintertime;
int wintermonths[] = {10,11,12,1,2,3};

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);


void setup() {
cli();//stop interrupts
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
  
  DDRD = B00111100; //sets 2,3,4,5 output
  PORTD = B00000000; //all pins low
  DDRB = B00111111; //sets 8,9,10,11,12,13 output
  PORTB = B00000000; //all pins low

Serial.begin(115200);
// Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
}

void loop() {
   while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))

  if (changed == true) {
    if (time >= 86400){
      time=0;
    }
    if (time%30==0){
        //set_time();
      if (gps.date.isValid()&&gps.time.isValid()) {
        int h = gps.time.hour();
        int m = gps.time.minute();
        int s = gps.time.second();
        int mo = gps.date.month();
        int d = gps.date.day();
        int y = gps.date.year();
        for (int i=0; i < sizeof(wintermonths)/sizeof(int);i++){
          if (wintermonths[i]== mo){
            wintertime = 1;
            break;
          }
          else{
            wintertime = 0;
          }
         }
        //calculate end of wintertime
        if (mo == 3) {
          int winterend = (31 -((((5*y)/4)+4)%7));
          if (d>=winterend){
            wintertime=0;
          }
        }
        if (mo == 10) {
          int winterstart = (31 - ((((5*y)/4)+1)%7));
          if (d<winterstart){
            wintertime=0;
          }
        }
        int tz=0;
        if (wintertime==0){
          tz=7200;
        }
        else {
          tz=3600;
         }
        time = (h*3600L)+(m*60)+s+tz;
        //Serial.print(tz);
        if (time>=86400){
          time = time - 86400;
        }
      }
      else {
        Serial.print("date not available \n" );
       }
    } 

        int h1,rem,m1,s1;
        h1 = time/3600L;
     
  rem = time%3600L;
  m1 = rem/60;
  s1 = rem % 60;
  int timearray[]={(h1/10)%10,h1%10,(m1/10)%10,m1%10,(s1/10)%10,s1%10};
  for (int i=0; i < sizeof(timearray)/sizeof(int);i++){
      PORTB = B00000001<<i; 
      PORTD = ~timearray[i] << 2;
      delay(20);
      PORTB = B00000000;
      delay(20);
  }
      changed=0;
  }
}

SIGNAL(TIMER1_COMPA_vect) {
time++;
changed=1;
}
