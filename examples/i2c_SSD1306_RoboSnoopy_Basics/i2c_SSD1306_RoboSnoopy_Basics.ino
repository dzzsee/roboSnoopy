//***********************************************************************************************
//  This example shows the basic methods of the RoboSnoopy library.
//  A pixel-art Snoopy wanders around the screen, cycles through its moods
//  and reacts to one-shot animations.
//
//  Hardware: an Arduino Nano R3 (or any compatible board), an I2C OLED display
//  with an SSD1306 / SSD1309 chip, and some jumper wires.
//
//  Based on the FluxGarage RoboEyes library examples by Dennis Hoelscher.
//
//***********************************************************************************************

#include <Adafruit_SSD1306.h>
#include <RoboSnoopy.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// create a RoboSnoopy instance using an Adafruit_SSD1306 display driver
RoboSnoopy<Adafruit_SSD1306> snoopy(display);


void setup() {
  Serial.begin(9600);

  // Startup OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C or 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Startup Snoopy
  snoopy.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60); // screen-width, screen-height, max framerate

  // Let Snoopy stroll around on its own
  snoopy.setIdleMode(ON, 2, 2); // bool active, int interval, int variation -> walk to a new random position every few seconds
  snoopy.setAutoblinker(ON, 4, 3); // blink every few seconds, with a random variation

  // Snoopy's walking speed in pixels per second
  snoopy.setSpeed(50);
}


void loop() {
  snoopy.update(); // update Snoopy drawings (framerate limited)

  // Cycle through the moods every 4 seconds
  static unsigned long moodTimer = millis();
  if (millis() >= moodTimer + 4000) {
    moodTimer = millis();
    static byte nextMood = HAPPY;
    snoopy.setMood(nextMood);
    nextMood++;
    if (nextMood > CURIOUS) { nextMood = DEFAULT; }

    if (snoopy.mood == HAPPY)    { snoopy.anim_laugh();     } // hop up and down
    if (snoopy.mood == CURIOUS)  { snoopy.anim_confused();  } // shake left and right
  }

  // Play a one-shot animation from time to time
  static unsigned long animTimer = millis();
  if (millis() >= animTimer + 12000) {
    animTimer = millis();
    snoopy.anim_confused();
  }

  // Put Snoopy to sleep at night: hold this pin LOW or comment the two lines
  if (digitalRead(A0) == LOW) { snoopy.close(); }
  else { snoopy.open(); }
}