//***********************************************************************************************
//  This example shows how to create Snoopy animation sequences without the use of delay();
//
//  Hardware: You'll need a breadboard, an arduino nano r3, an I2C oled display with 1306
//  or 1309 chip and some jumper wires.
//
//  Published in September 2024 by Dennis Hoelscher, FluxGarage (original RoboEyes)
//  Robo Snoopy edition
//
//***********************************************************************************************


#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <FluxGarage_RoboEyes.h>
// create a RoboEyes instance using an Adafruit_SSD1306 display driver
RoboEyes<Adafruit_SSD1306> roboEyes(display);


// EVENT TIMER
unsigned long eventTimer; // will save the timestamps
bool event1wasPlayed = 0; // flag variables
bool event2wasPlayed = 0;
bool event3wasPlayed = 0;
bool event4wasPlayed = 0;
bool event5wasPlayed = 0;
bool event6wasPlayed = 0;
bool event7wasPlayed = 0;


void setup() {
  // OLED Display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C or 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Startup RoboSnoopy
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100); // screen-width, screen-height, max framerate - 60-100fps are good for smooth animations
  roboEyes.setPosition(DEFAULT); // Snoopy starts in the middle of the screen
  roboEyes.close(); // start with closed eyes

  eventTimer = millis(); // start event timer from here

} // end of setup


void loop() {
 roboEyes.update(); // update Snoopy drawings

  // LOOPED ANIMATION SEQUENCE
  // Do once after defined number of milliseconds
  if(millis() >= eventTimer+1500 && event1wasPlayed == 0){
    event1wasPlayed = 1;
    roboEyes.open(); // Snoopy opens his eyes
    roboEyes.setMood(DEFAULT);
  }
  // Snoopy gets surprised and jumps up
  if(millis() >= eventTimer+3000 && event2wasPlayed == 0){
    event2wasPlayed = 1;
    roboEyes.setMood(SURPRISED);
    roboEyes.setPosition(N);
  }
  // Snoopy is happy and laughs
  if(millis() >= eventTimer+4500 && event3wasPlayed == 0){
    event3wasPlayed = 1;
    roboEyes.setMood(HAPPY);
    roboEyes.anim_laugh();
    roboEyes.setPosition(DEFAULT);
  }
  // Snoopy gets confused and starts scratching his head
  if(millis() >= eventTimer+6000 && event4wasPlayed == 0){
    event4wasPlayed = 1;
    roboEyes.setMood(CONFUSED);
    roboEyes.anim_confused();
  }
  // Snoopy gets angry
  if(millis() >= eventTimer+7500 && event5wasPlayed == 0){
    event5wasPlayed = 1;
    roboEyes.setMood(ANGRY);
    roboEyes.setPosition(S);
  }
  // Snoopy gets tired
  if(millis() >= eventTimer+9000 && event6wasPlayed == 0){
    event6wasPlayed = 1;
    roboEyes.setMood(TIRED);
  }
  // Snoopy falls in love
  if(millis() >= eventTimer+10500 && event7wasPlayed == 0){
    event7wasPlayed = 1;
    roboEyes.setMood(LOVE);
    roboEyes.setPosition(DEFAULT);
  }
  // Reset timer and flags to restart the whole animation sequence
  if(millis() >= eventTimer+12000){
    roboEyes.close(); // Snoopy goes to sleep
    roboEyes.setMood(DEFAULT);
    // Reset the timer and the event flags to restart the whole "complex animation loop"
    eventTimer = millis();
    event1wasPlayed = 0;
    event2wasPlayed = 0;
    event3wasPlayed = 0;
    event4wasPlayed = 0;
    event5wasPlayed = 0;
    event6wasPlayed = 0;
    event7wasPlayed = 0;
  }
  // END OF LOOPED ANIMATION SEQUENCE

} // end of main loop