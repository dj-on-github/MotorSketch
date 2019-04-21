

#define VENDOR_ID               0x16C0
#define PRODUCT_ID              0x0480
#define RAWHID_USAGE_PAGE       0xFFAB  // recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE            0x0200  // recommended: 0x0100 to 0xFFFF

#define RAWHID_TX_SIZE          64      // transmit packet size
#define RAWHID_TX_INTERVAL      2       // max # of ms between transmit packets
#define RAWHID_RX_SIZE          64      // receive packet size
#define RAWHID_RX_INTERVAL      8       // max # of ms between receive packets

#define ROTARY_ENCODER_TICKS 800
#define STEPPER_MOTOR_TICKS 800

int rotary_encoder_pin_a_r = 20;
int rotary_encoder_pin_b_r = 10;
int rotary_encoder_pin_a_f = 19;
int rotary_encoder_pin_b_f = 9;

volatile int rotary_encoder_pos = 0;
volatile int angle_offset;
volatile int br_count=0;
volatile int ar_count=0;
volatile int bf_count=0;
volatile int af_count=0;
volatile int ar_error_count=0;
volatile int af_error_count=0;
volatile int br_error_count=0;
volatile int bf_error_count=0;
volatile int encoder_state;

int pul_pin = 23;
int dir_pin = 22;
int ena_pin = 21;
int led_pin = 13;

double qerr;

#define CW 0
#define CCW 1

// Quadrature state changes
int states[4] = {0,1,3,2};

/* Old Tables - Too slow.
int useconds[47] = {1000,980,960,940,920,900,880,860,840,820,
                        800,780,760,740,720,700,680,660,640,620,
                        600,580,560,540,520,500,480,460,440,420,
                        400,380,360,340,320,300,280,260,240,220,
                        200,180,160,140,120,100,80};
                        
int steps[47] =        {100,102,104,106,109,111,114,116,119,122,
                        125,128,132,135,139,143,147,152,156,161,
                        167,172,179,185,192,200,208,217,227,238,
                        250,263,278,294,312,333,357,385,417,455,
                        500,556,625,714,833,1000,1250};

int lengths[47] =      {100, 202, 306, 412, 521, 632, 746, 862,
                        981, 1103, 1228, 1356, 1488, 1623, 1762,
                        1905, 2052, 2204, 2360, 2521, 2688, 2860,
                        3039, 3224, 3416, 3616, 3824, 4041,
                        4268, 4506, 4756, 5019, 5297, 5591,
                        5903, 6236, 6593, 6978, 7395, 7850,
                        8350, 8906, 9531, 10245, 11078, 12078,
                        13328};

int ramp_elements = 46;
int ramp_steps = 12078;
*/

// useconds - table of microseconds per pulse on ramp up.
// steps - table of how many pulsed to issue at each usecond rate
// lengths - cumulative number of steps, so we can know how far to go.

int useconds[47] = {1000,980,960,940,920,900,880,860,840,820,800,780,760,740,
                    720,700,680,660,640,620,600,580,560,540,520,500,480,460,
                    440,420,400,380,360,340,320,300,280,260,240,220,200,180,
                    160,140,120,100,80};

int steps[47] = {25,26,26,27,27,28,28,29,30,30,31,32,33,34,35,36,37,38,39,40,
                 42,43,45,46,48,50,52,54,57,60,62,66,69,74,78,83,89,96,104,114,
                 125,139,156,179,208,250,312};

int lengths[47] = {25,51,77,104,131,159,187,216,246,276,307,339,372,406,441,
                   477,514,552,591,631,673,716,761,807,855,905,957,1011,1068,
                   1128,1190,1256,1325,1399,1477,1560,1649,1745,1849,1963,2088,
                   2227,2383,2562,2770,3020,3332};

int ramp_elements = 46;
int ramp_steps = 3020;



void pul_high() {
   pinMode(pul_pin, INPUT);
}
void pul_low() {
   pinMode(pul_pin, OUTPUT);
   digitalWrite(pul_pin,LOW);
}


void dir_high() {
   pinMode(dir_pin, INPUT);
}
void dir_low() {
   digitalWrite(dir_pin,LOW);  
   pinMode(dir_pin, OUTPUT);
}

void ena_high() {
   pinMode(ena_pin, INPUT);
}
void ena_low() {
   digitalWrite(ena_pin,LOW);
   pinMode(ena_pin, OUTPUT);
}



void setup() {
  int a;
  int b;
  //Serial.begin(9600);
  delay(10);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

  ena_low();
  pul_high();
  dir_high();
  enable_dm320t();

  pinMode (rotary_encoder_pin_a_r , INPUT_PULLUP );
  pinMode (rotary_encoder_pin_b_r , INPUT_PULLUP );
  pinMode (rotary_encoder_pin_a_f , INPUT_PULLUP );
  pinMode (rotary_encoder_pin_b_f , INPUT_PULLUP );
  void do_rotary_encoder_a_r();
  void do_rotary_encoder_b_r();
  void do_rotary_encoder_a_f();
  void do_rotary_encoder_b_f();
  attachInterrupt(rotary_encoder_pin_a_r , do_rotary_encoder_a_r , RISING) ;
  attachInterrupt(rotary_encoder_pin_b_r , do_rotary_encoder_b_r , RISING) ;
  attachInterrupt(rotary_encoder_pin_a_f , do_rotary_encoder_a_f , FALLING) ;
  attachInterrupt(rotary_encoder_pin_b_f , do_rotary_encoder_b_f , FALLING) ;

  rotary_encoder_pos = 0;
  br_count=0;
  ar_count=0;
  bf_count=0;
  af_count=0;

  a = digitalRead(rotary_encoder_pin_a_r);
  b = digitalRead(rotary_encoder_pin_b_r);
  
  encoder_state = ((a & (b<<1)) & 0x3);

  //usb_init();
  //while (not(usb_configured())) delay(100);
}

void enable_dm320t() {
    pinMode(ena_pin, OUTPUT);
    digitalWrite(ena_pin, LOW);
    delay(1);
}

void disable_dm320t() {
    pinMode(ena_pin, INPUT);
    delay(1);
}

void delay_us(unsigned long t) {
    unsigned long t1;
    t1 = micros();
    for (;micros() < ((t1+t) & 0xffffffff);) {}
}

void pulses(int n,int us) {
    int i;
    
    for(i=0;i<n;i++) {
        pul_low();
        delay_us(us);
        pul_high();
        delay_us(us);
    }
}

void accel_steps(int dir, int n) {
  //int useconds[47]
  //int steps[47]
  //int lengths[46]
  int i;
  int fastest_steps;
  
  if (dir==CW) dir_low();
  else dir_high();
  
  delay_us(10);
    
  if (n > (2*ramp_steps)) {// Full ramps plus some fastest 
      fastest_steps = n - (2*ramp_steps);
      
      for (i=0;i<ramp_elements;i++) { // Ramp up
          pulses(steps[i],useconds[i]);
      }
      
      pulses(fastest_steps,80); // Fastest run
      
      for (i=0;i<ramp_elements;i++) { // Ramd down
          pulses(steps[ramp_elements-1-i],useconds[ramp_elements-1-i]);
      }             
  }
  
  else if (n == (2*ramp_steps)) {// Full ramps. No Fastest steps
      for (i=0;i<ramp_elements;i++) { // Ramp up
          pulses(steps[i],useconds[i]);
      }
      for (i=0;i<ramp_elements;i++) { // Ramd down
          pulses(steps[45-i],useconds[45-i]);
      }      
  }
  else {  // Only partial ramp up and down.
      // Nothing yet
     pulses(n,STEPPER_MOTOR_TICKS);
  }
    
}

void do_rotary_encoder_a_r() {
  int inc;
  int next_state;

  next_state = encoder_state;
  inc = 0;
  if (encoder_state==0) {
     inc=1;
     next_state=1;
  } else if (encoder_state==2) {
     inc = -1;
     next_state=3;     
  }
  else ar_error_count++;

  ar_count++;

 cli();
 rotary_encoder_pos = (rotary_encoder_pos + inc);
 encoder_state = next_state;
 sei();
}

void do_rotary_encoder_b_r() {
  int inc;
  int next_state;

  inc=0;
  next_state = encoder_state;
  if (encoder_state==1) {
     inc=1;
     next_state=3;
  } else if (encoder_state==0) {
     inc = -1;
     next_state=2;     
  }
    else br_error_count++;

  br_count++;

 cli();
 rotary_encoder_pos = (rotary_encoder_pos + inc);
 encoder_state = next_state;
 sei();
}

void do_rotary_encoder_a_f() {
  int inc;
  int next_state;

  inc=0;
  next_state = encoder_state;
  if (encoder_state==3) {
     inc=1;
     next_state=2;
  } else if (encoder_state==1) {
     inc = -1;
     next_state=0;     
  }
  else af_error_count++;
  af_count++;

 cli();
 rotary_encoder_pos = (rotary_encoder_pos + inc);
 encoder_state = next_state;
 sei();
}

void do_rotary_encoder_b_f() {
  int inc;
  int next_state;

  inc = 0;
  next_state = encoder_state;
  if (encoder_state==3) {
     inc=-1;
     next_state=1;
  } else if (encoder_state==2) {
     inc = 1;
     next_state=0;     
  }
  else bf_error_count++;
  bf_count++;

 cli();
 rotary_encoder_pos = (rotary_encoder_pos + inc);
 encoder_state = next_state;
 sei();
}

void home_position() {
  rotary_encoder_pos = 0;
}

void home() {
  home_position();
}

double angle() {
  int a;
  a = ((((rotary_encoder_pos % ROTARY_ENCODER_TICKS)) * 0.45));
  return a;
}

void doit() {
  int i;
    //for (i=0;i<100;i++) {
    //   printit();
    //}
    //return;
    Serial.println("FULL ROTATE");
    Serial.println(rotary_encoder_pos , DEC);
    home();
    delay(1000);
    printit();
    for (i=0;i<5;i++) {
      accel_steps(CW, 1600);
          printit();
          delay(200);
          ar_count = 0;
          br_count = 0;
          af_count = 0;
          bf_count = 0;
    }

}

void sendout(void *buffer) {
//    int l;
//    l = strlen(buffer);
//    usb_rawhid_send((uint8_t *)buffer, 20);
}

void printit() {
    delay(500);
    Serial.begin(9600);
    delay(500);
    //char buf1[256];
    //char buf2[256];
    //char buf3[256];
    //char buf4[256];
    //char buf5[256];
    Serial.print("POS  :");
    Serial.println(rotary_encoder_pos , DEC);
    //sprintf(buf1, "POS %d/n",rotary_encoder_pos);
    //sendout(buf1);
    
    Serial.print("ANGLE  :");
    Serial.println(angle(), DEC); 
    //sprintf(buf2,"ANGLE %f/n",angle()); 
    //sendout(buf2);
      
    Serial.print("AR  :");
    Serial.print(ar_count,DEC);
    Serial.print("  AF:");
    Serial.println(af_count,DEC);
    //sprintf(buf3,"AR %d  AF %d\n",ar_count, af_count); 
    //sendout(buf3);
    
    Serial.print("BR  :");
    Serial.print(br_count,DEC);
    Serial.print("  BF:");
    Serial.println(bf_count,DEC);
    Serial.println("---------");
    //sprintf(buf4,"BR %d  BF %d\n",br_count, bf_count);
    //sendout(buf4);
    //sprintf(buf5, "-----------\n");
    //sendout(buf5);
    
    Serial.flush();
    Serial.end();
    delay(500);
}

void loop() {
    digitalWrite(led_pin,HIGH);
    doit();
    digitalWrite(led_pin,LOW);
    doit();
    
  
}


