/*
 * RoboSnoopy - The FluxGarage RoboEyes library, revived as a pixel-art Snoopy
 *
 * Draws a fully animated pixel-art Snoopy on OLED displays, based on the 
 * Adafruit GFX library's graphics primitives. Snoopy reflects many emotions
 * (default, tired, angry, happy, confused, surprised, sad, love, dead, yay)
 * and every part of his body moves: ears, eyes, brows, pupils, nose, mouth,
 * arms, tail, feet and the whole character wanders around the screen.
 *
 * Copyright (C) 2024-2026 Dennis Hoelscher (original RoboEyes)
 * Robo Snoopy edition - your name here
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef _FLUXGARAGE_ROBOEYES_H
#define _FLUXGARAGE_ROBOEYES_H


// Display colors
uint8_t BGCOLOR = 0;    // background and overlays
uint8_t MAINCOLOR = 1;  // drawings

// For mood type switch
#define DEFAULT     0
#define TIRED       1
#define ANGRY       2
#define HAPPY       3
#define CONFUSED    4
#define SURPRISED   5
#define SAD         6
#define LOVE        7
#define DEAD        8
#define YAY         9

// For turning things on or off
#define ON  1
#define OFF 0

// Snoopy screen margins (distance from screen edges to the head-center anchor point)
#define SN_XMARGIN 16   // min/max head-center x distance from left/right screen edges
#define SN_YMARGIN 12   // min head-center y distance from top of the screen
#define SN_YBOTTOM 38   // head-center y distance from the bottom (feet clearance)

// For switch "predefined positions"
#define N  1   // north, top center
#define NE 2   // north-east, top right
#define E  3   // east, middle right
#define SE 4   // south-east, bottom right
#define S  5   // south, bottom center
#define SW 6   // south-west, bottom left
#define W  7   // west, middle left
#define NW 8   // north-west, top left
// for middle center set "DEFAULT"


// Constructor: takes a reference to the active Adafruit display object (e.g., Adafruit_SSD1306)
// Eg: RoboEyes<Adafruit_SSD1306> snoopy(display);
template<typename AdafruitDisplay>
class RoboEyes
{
public:

// Reference to Adafruit display object
AdafruitDisplay *display;

// For general setup - screen size and max. frame rate
int screenWidth = 128; // OLED display width, in pixels
int screenHeight = 64; // OLED display height, in pixels
int frameInterval = 20; // default value for 50 frames per second (1000/50 = 20 milliseconds)
unsigned long fpsTimer = 0; // for timing the frames per second

// For controlling mood types and expressions (kept for compatibility)
bool tired = 0;
bool angry = 0;
bool happy = 0;
bool curious = 0;  // kept for compatibility, has no effect on Snoopy
bool cyclops = 0;  // kept for compatibility, Snoopy is always a single character
bool sweat = 0;
bool eyeL_open = 1; // left eye opened or closed?
bool eyeR_open = 1; // right eye opened or closed?
int moodCurrent = DEFAULT;


//*********************************************************************************************
//  Snoopy Position (the anchor point is the center of Snoopy's head)
//*********************************************************************************************

int eyeLxDefault = 64;   // centered head position (default screen 128x64)
int eyeLyDefault = 27;
int eyeLx = eyeLxDefault;     // current anchor x
int eyeLy = eyeLyDefault;     // current anchor y
int eyeLxNext = eyeLx;        // target anchor x
int eyeLyNext = eyeLy;        // target anchor y


//*********************************************************************************************
//  Emotion drivers - target values come from setMood(), current values are tweened each frame
//*********************************************************************************************

int t_ear = 0;    // ear lift: -1 droop, 0 neutral, +1 raised
int c_ear = 0;
int t_brow = 0;   // brow: 0 none, -2 angry, -1 surprised, +1 happy, +2 sad, +3 confused
int c_brow = 0;
int t_mouth = 0;  // mouth: 0 neutral, 1 smile, 2 open laugh, -1 small frown, -2 big frown
int c_mouth = 0;
int t_arm = 0;    // arms: 0 down, 1 up, 2 wave, 3 scratch, 4 fists
int c_arm = 0;
int t_energy = 0; // energy: 0 calm, 1 happy bounce, 3 yay jump
int c_energy = 0;
int t_spE = 0;    // special eyes: 0 normal, 1 hearts, 2 big surprise, 3 crossed (X), 4 squint
int t_spM = 0;    // special mouth: 0 normal, 1 yawn, 2 surprise "o", 3 dead wavy + tongue
int specialEyes = 0;
int specialMouth = 0;
float eyeOpenCur = 1.0f;    // how open the eyes are: 1.0 open, 0.0 closed
float eyeOpenTarget = 1.0f;

// Mood parameter lookup tables, indexed by mood constant (0..9)
int moodEar[10]   = {0,-1, 1, 1, 0, 1,-1, 1,-1, 1};
int moodBrow[10]  = {0, 0,-2, 1, 3,-1, 2, 1, 0,-1};
int moodMouth[10] = {0,-1,-2, 2, 1, 1,-2, 2,-1, 2};
int moodArm[10]   = {0, 0, 4, 1, 3, 1, 0, 2, 0, 1};
int moodEnergy[10]= {0, 0, 0, 1, 0, 1, 0, 1, 0, 3};
int moodSpE[10]   = {0, 0, 0, 0, 4, 2, 0, 1, 3, 0};
int moodSpM[10]   = {0, 1, 0, 0, 0, 2, 0, 0, 3, 0};
float moodEye[10] = {1.0f,0.4f,1.0f,1.0f,1.0f,1.0f,0.8f,1.0f,0.0f,1.0f};


//*********************************************************************************************
//  Macro Animations
//*********************************************************************************************

// Animation - horizontal flicker/shiver
bool hFlicker = 0;
bool hFlickerAlternate = 0;
byte hFlickerAmplitude = 2;

// Animation - vertical flicker/shiver
bool vFlicker = 0;
bool vFlickerAlternate = 0;
byte vFlickerAmplitude = 2;

// Animation - auto blinking
bool autoblinker = 0;                 // activate auto blink animation
int blinkInterval = 1;                // basic interval between each blink in full seconds
int blinkIntervalVariation = 4;       // interval variation range in full seconds
unsigned long blinktimer = 0;

// Animation - idle mode: Snoopy wanders to random screen positions
bool idle = 0;
int idleInterval = 1;                 // basic interval between each repositioning in full seconds
int idleIntervalVariation = 3;        // interval variation range in full seconds
unsigned long idleAnimationTimer = 0;

// Animation - Snoopy confused: shaking left and right
bool confused = 0;
unsigned long confusedAnimationTimer = 0;
int confusedAnimationDuration = 500;
bool confusedToggle = 1;

// Animation - Snoopy laughing: shaking up and down
bool laugh = 0;
unsigned long laughAnimationTimer = 0;
int laughAnimationDuration = 500;
bool laughToggle = 1;

// Sweat drop 1
int sweat1XPosInitial = 2;
int sweat1XPos;
float sweat1YPos = 2;
int sweat1YPosMax;
float sweat1Height = 2;
float sweat1Width = 1;

// Sweat drop 2
int sweat2XPosInitial = 2;
int sweat2XPos;
float sweat2YPos = 2;
int sweat2YPosMax;
float sweat2Height = 2;
float sweat2Width = 1;

// Sweat drop 3
int sweat3XPosInitial = 2;
int sweat3XPos;
float sweat3YPos = 2;
int sweat3YPosMax;
float sweat3Height = 2;
float sweat3Width = 1;


//*********************************************************************************************
//  GENERAL METHODS
//*********************************************************************************************

RoboEyes(AdafruitDisplay &disp) : display(&disp) {};

// Startup RoboEyes with defined screen-width, screen-height and max. frames per second
void begin(int width, int height, byte frameRate) {
	screenWidth = width;
	screenHeight = height;
  display->clearDisplay();
  display->display();
  // Start centered with open, neutral eyes
  eyeLxDefault = screenWidth/2;
  eyeLyDefault = (screenHeight/2)-5;
  if(eyeLyDefault > getScreenConstraint_Y()) eyeLyDefault = getScreenConstraint_Y();
  eyeLx = eyeLxDefault;
  eyeLy = eyeLyDefault;
  eyeLxNext = eyeLx;
  eyeLyNext = eyeLy;
  eyeOpenCur = 1.0f;
  eyeOpenTarget = 1.0f;
  // Reset emotion drivers
  t_ear=c_ear=0; t_brow=c_brow=0; t_mouth=c_mouth=0; t_arm=c_arm=0;
  t_energy=c_energy=0; t_spE=specialEyes=0; t_spM=specialMouth=0;
  moodCurrent = DEFAULT;
  fpsTimer = 0;
  blinktimer = 0;
  idleAnimationTimer = 0;
  setFramerate(frameRate);
}

void update(){
  // Limit drawing updates to defined max framerate
  if(millis()-fpsTimer >= frameInterval){
    drawEyes();
    fpsTimer = millis();
  }
}


//*********************************************************************************************
//  SETTERS METHODS
//*********************************************************************************************

// Calculate frame interval based on defined frameRate
void setFramerate(byte fps){
  frameInterval = 1000/fps;
}

// Set color values
void setDisplayColors(uint8_t background, uint8_t main) {
  BGCOLOR = background; // background and overlays, choose 0 for monochrome displays and 0x00 for grayscale displays such as SSD1322
  MAINCOLOR = main;     // drawings, choose 1 for monochrome displays and 0x0F for grayscale displays such as SSD1322 (0x0F = maximum brightness)
}

// Kept for backwards compatibility - Snoopy ignores eye-geometry settings
void setWidth(byte leftEye, byte rightEye) {}
void setHeight(byte leftEye, byte rightEye) {}
void setBorderradius(byte leftEye, byte rightEye) {}
void setSpacebetween(int space) {}
void setCuriosity(bool curiousBit) { curious = curiousBit; }
void setCyclops(bool cyclopsBit) { cyclops = cyclopsBit; }

// Set mood expression (changes ears, eyes, brows, mouth, arms, energy, specials)
void setMood(unsigned char mood)
{
  if(mood > YAY) mood = DEFAULT;
  moodCurrent = mood;
  t_ear = moodEar[mood];
  t_brow = moodBrow[mood];
  t_mouth = moodMouth[mood];
  t_arm = moodArm[mood];
  t_energy = moodEnergy[mood];
  t_spE = moodSpE[mood];
  t_spM = moodSpM[mood];
  eyeOpenTarget = moodEye[mood];
  tired = (mood==TIRED);
  angry = (mood==ANGRY);
  happy = (mood==HAPPY || mood==YAY || mood==LOVE);
}

void setEmotion(unsigned char emotion){ setMood(emotion); } // alias for setMood()

// Set predefined position
void setPosition(unsigned char position)
  {
    switch (position)
    {
    case N:
      eyeLxNext = screenWidth/2;
      eyeLyNext = SN_YMARGIN+1;
      break;
    case NE:
      eyeLxNext = getScreenConstraint_X()-4;
      eyeLyNext = SN_YMARGIN+2;
      break;
    case E:
      eyeLxNext = getScreenConstraint_X();
      eyeLyNext = (screenHeight/2)-5;
      break;
    case SE:
      eyeLxNext = getScreenConstraint_X()-4;
      eyeLyNext = getScreenConstraint_Y();
      break;
    case S:
      eyeLxNext = screenWidth/2;
      eyeLyNext = getScreenConstraint_Y();
      break;
    case SW:
      eyeLxNext = SN_XMARGIN+4;
      eyeLyNext = getScreenConstraint_Y();
      break;
    case W:
      eyeLxNext = SN_XMARGIN;
      eyeLyNext = (screenHeight/2)-5;
      break;
    case NW:
      eyeLxNext = SN_XMARGIN+4;
      eyeLyNext = SN_YMARGIN+2;
      break;
    default:
      eyeLxNext = screenWidth/2;
      eyeLyNext = (screenHeight/2)-5;
      break;
    }
  }

// Set automated blinking, minimal blink interval in full seconds and variation range in full seconds
void setAutoblinker(bool active, int interval, int variation){
  autoblinker = active;
  blinkInterval = interval;
  blinkIntervalVariation = variation;
}
void setAutoblinker(bool active){
  autoblinker = active;
}

// Set idle mode - Snoopy wandering to random screen positions
void setIdleMode(bool active, int interval, int variation){
  idle = active;
  idleInterval = interval;
  idleIntervalVariation = variation;
}
void setIdleMode(bool active) {
  idle = active;
}

// Set horizontal flickering (displacing Snoopy left/right)
void setHFlicker (bool flickerBit, byte Amplitude) {
  hFlicker = flickerBit;
  hFlickerAmplitude = Amplitude;
}
void setHFlicker (bool flickerBit) {
  hFlicker = flickerBit;
}

// Set vertical flickering (displacing Snoopy up/down)
void setVFlicker (bool flickerBit, byte Amplitude) {
  vFlicker = flickerBit;
  vFlickerAmplitude = Amplitude;
}
void setVFlicker (bool flickerBit) {
  vFlicker = flickerBit;
}

void setSweat (bool sweatBit) {
  sweat = sweatBit;
}


//*********************************************************************************************
//  GETTERS METHODS
//*********************************************************************************************

// Returns the max anchor x position for Snoopy (ears stay on screen)
int getScreenConstraint_X(){
  return screenWidth-SN_XMARGIN;
}

// Returns the max anchor y position for Snoopy (feet stay on screen)
int getScreenConstraint_Y(){
  return screenHeight-SN_YBOTTOM;
}


//*********************************************************************************************
//  BASIC ANIMATION METHODS
//*********************************************************************************************

// Close both eyes
void close() {
	eyeOpenTarget = 0;
  eyeL_open = 0;
	eyeR_open = 0;
}

// Open both eyes (returns to the openness defined by the current mood)
void open() {
  eyeOpenTarget = moodEye[moodCurrent];
  eyeL_open = 1;
	eyeR_open = 1;
}

// Trigger blink animation
void blink() {
  close();
  open();
}

// Close/Open single eye control (kept for compatibility - Snoopy blinks both eyes)
void close(bool left, bool right) { close(); }
void open(bool left, bool right) { open(); }
void blink(bool left, bool right) { blink(); }


//*********************************************************************************************
//  MACRO ANIMATION METHODS
//*********************************************************************************************

// Play confused animation - one shot animation of Snoopy shaking left and right
void anim_confused() {
	confused = 1;
}

// Play laugh animation - one shot animation of Snoopy shaking up and down
void anim_laugh() {
  laugh = 1;
}


//*********************************************************************************************
//  PRE-CALCULATIONS AND ACTUAL DRAWINGS
//*********************************************************************************************

// Interpolate an int halfway towards target, always making progress
int tweenInt(int current, int target){
  int next = (current + target)/2;
  if(next == current && next != target){ next += (target>current)?1:-1; }
  return next;
}

// Draw a small pixel-art heart
void drawHeart(int hx, int hy){
  display->fillCircle(hx-1, hy-1, 2, BGCOLOR);
  display->fillCircle(hx+1, hy-1, 2, BGCOLOR);
  display->fillTriangle(hx-3, hy-1, hx+3, hy-1, hx, hy+2, BGCOLOR);
}

// Draw pixel-art Snoopy with head center at (cx, cy).
// Every body part is computed from the tweened emotion driver values:
// ears, eyes, brows, pupils, nose, mouth, body, arms, legs, feet and tail.
void drawSnoopy(int cx, int cy){

  // Pupils look into the direction Snoopy is walking to
  int px = (eyeLxNext > screenWidth*0.7f) ? 1 : ((eyeLxNext < screenWidth*0.3f) ? -1 : 0);
  int py = (eyeLyNext < eyeLyDefault-4) ? -1 : ((eyeLyNext > eyeLyDefault+4) ? 1 : 0);

  // Head tilt (confused: tilt to the right, dead: droop to the left)
  int fTilt = (specialEyes==4) ? 1 : 0;

  //************************************* TAIL *************************************
  float wagDiv = (c_energy>0 || c_mouth>=2) ? 90.0f : 160.0f;
  int wag = (int)(sin(millis()/wagDiv)*3.0f);
  display->fillRect(cx+10, cy+18, 2, 2, MAINCOLOR);
  display->fillRect(cx+12+wag, cy+16, 2, 2, MAINCOLOR);
  display->fillRect(cx+14+wag, cy+14, 2, 2, MAINCOLOR);

  //************************************* HEAD *************************************
  display->fillRoundRect(cx-12, cy-9, 25, 26, 11, MAINCOLOR);

  //************************************* EARS *************************************
  // Confused: left ear raised, right ear drooping
  int earLiftL = c_ear + ((specialEyes==4) ? 1 : 0);
  int earLiftR = c_ear;
  // Angry/confused: ears stiffen outwards
  int esL = (c_brow==-2 || c_brow==3) ? -1 : 0;
  int esR = (c_brow==-2 || c_brow==3) ?  1 : 0;
  // Left ear
  display->fillRoundRect(cx-15+esL, cy-8+earLiftL, 6, 17, 3, MAINCOLOR);
  display->fillRoundRect(cx-14+esL, cy-7+earLiftL, 4, 15, 2, BGCOLOR);
  // Right ear
  display->fillRoundRect(cx+9+esR,  cy-8+earLiftR, 6, 17, 3, MAINCOLOR);
  display->fillRoundRect(cx+10+esR, cy-7+earLiftR, 4, 15, 2, BGCOLOR);

  //************************************* EYES *************************************
  if(specialEyes == 1){
    // LOVED: heart eyes
    drawHeart(cx-4, cy-1);
    drawHeart(cx+4, cy-1);
  }
  else if(specialEyes == 3){
    // DEAD: crossed X eyes
    display->drawLine(cx-6, cy-4, cx-2, cy+0, BGCOLOR);
    display->drawLine(cx-6, cy+0, cx-2, cy-4, BGCOLOR);
    display->drawLine(cx+2, cy-4, cx+6, cy+0, BGCOLOR);
    display->drawLine(cx+2, cy+0, cx+6, cy-4, BGCOLOR);
  }
  else if(eyeOpenCur < 0.12f){
    // BLINKED/CLOSED: soft curved eyelid lines
    display->drawLine(cx-6, cy, cx-3, cy-1, BGCOLOR);
    display->drawLine(cx-3, cy-1, cx-1, cy, BGCOLOR);
    display->drawLine(cx+1, cy, cx+4, cy-1, BGCOLOR);
    display->drawLine(cx+4, cy-1, cx+6, cy, BGCOLOR);
  }
  else {
    int eyeW = (specialEyes==2) ? 5 : 4;              // surprise = big round eyes
    int eyeH = (specialEyes==2) ? 6 : (1 + (int)(eyeOpenCur*4.0f)); // 1..5
    int yTop = cy - (eyeH/2);
    if(specialEyes == 4){
      // CONFUSED: squinty left eye
      display->fillRoundRect(cx-6+px, yTop, 4, (eyeH>3)?eyeH-2:1, 2, BGCOLOR);
      display->fillRoundRect(cx+2+px, yTop, eyeW, eyeH, 2, BGCOLOR);
    } else if(specialEyes == 2){
      display->fillCircle(cx-4+px, cy, 3, BGCOLOR);
      display->fillCircle(cx+4+px, cy, 3, BGCOLOR);
    } else {
      display->fillRoundRect(cx-6+px, yTop, eyeW, eyeH, 2, BGCOLOR);
      display->fillRoundRect(cx+2+px, yTop, eyeW, eyeH, 2, BGCOLOR);
    }
  }

  //************************************* BROWS *************************************
  switch(c_brow){
    case -2: // ANGRY: inner ends pointing down
      display->drawLine(cx-9, cy-7, cx-4, cy-9, BGCOLOR);
      display->drawLine(cx+4, cy-9, cx+9, cy-7, BGCOLOR);
      break;
    case -1: // SURPRISED: both raised
      display->drawLine(cx-8, cy-10, cx-3, cy-11, BGCOLOR);
      display->drawLine(cx+3, cy-11, cx+8, cy-10, BGCOLOR);
      break;
    case 1: // HAPPY: outer ends up
      display->drawLine(cx-8, cy-6, cx-3, cy-8, BGCOLOR);
      display->drawLine(cx+3, cy-8, cx+8, cy-6, BGCOLOR);
      break;
    case 2: // SAD: inner ends pointing up
      display->drawLine(cx-9, cy-8, cx-4, cy-11, BGCOLOR);
      display->drawLine(cx+4, cy-11, cx+9, cy-8, BGCOLOR);
      break;
    case 3: // CONFUSED: one high, one low
      display->drawLine(cx-8, cy-10, cx-3, cy-9, BGCOLOR);
      display->drawLine(cx+3, cy-7, cx+8, cy-9, BGCOLOR);
      break;
  }

  //************************************* NOSE *************************************
  display->fillRoundRect(cx-3+fTilt, cy+2, 7, 6, 3, BGCOLOR);

  //************************************* MOUTH *************************************
  if(specialMouth == 1){
    // Yawn
    display->fillRoundRect(cx-2+fTilt, cy+10, 5, 5, 2, BGCOLOR);
  }
  else if(specialMouth == 2){
    // SURPRISED "o"
    display->fillCircle(cx+fTilt, cy+11, 2, BGCOLOR);
  }
  else if(specialMouth == 3){
    // DEAD: wavy mouth + tongue
    display->drawLine(cx-4+fTilt, cy+12, cx-1+fTilt, cy+11, BGCOLOR);
    display->drawLine(cx-1+fTilt, cy+11, cx+1+fTilt, cy+13, BGCOLOR);
    display->drawLine(cx+1+fTilt, cy+13, cx+4+fTilt, cy+11, BGCOLOR);
    display->fillRect(cx-3+fTilt, cy+14, 6, 2, MAINCOLOR); // tongue
  }
  else {
    switch(c_mouth){
      case 2: // big open laugh + tongue
        display->fillRoundRect(cx-5+fTilt, cy+9, 11, 6, 3, BGCOLOR);
        display->fillRect(cx-2+fTilt, cy+13, 5, 2, MAINCOLOR);
        break;
      case 1: // smile (corners up)
        display->drawLine(cx-3+fTilt, cy+11, cx+fTilt, cy+13, BGCOLOR);
        display->drawLine(cx+fTilt, cy+13, cx+3+fTilt, cy+11, BGCOLOR);
        break;
      case -1: // small frown (corners down)
        display->drawLine(cx-3+fTilt, cy+13, cx+fTilt, cy+11, BGCOLOR);
        display->drawLine(cx+fTilt, cy+11, cx+3+fTilt, cy+13, BGCOLOR);
        break;
      case -2: // big frown
        display->drawLine(cx-4+fTilt, cy+14, cx-1+fTilt, cy+11, BGCOLOR);
        display->drawLine(cx-1+fTilt, cy+11, cx+2+fTilt, cy+11, BGCOLOR);
        display->drawLine(cx+2+fTilt, cy+11, cx+4+fTilt, cy+14, BGCOLOR);
        break;
      default: // neutral: soft tiny smile
        display->drawLine(cx-2+fTilt, cy+11, cx+fTilt, cy+12, BGCOLOR);
        display->drawLine(cx+fTilt, cy+12, cx+2+fTilt, cy+11, BGCOLOR);
        break;
    }
  }

  //************************************* BODY *************************************
  display->fillRoundRect(cx-9, cy+16, 18, 12, 4, MAINCOLOR);

  //************************************* ARMS *************************************
  if(c_arm == 1){
    // Both arms raised
    display->fillCircle(cx-13, cy-7, 3, MAINCOLOR);
    display->fillCircle(cx+13, cy-7, 3, MAINCOLOR);
  }
  else if(c_arm == 2){
    // Waving right arm, left arm down (love)
    int wy = (int)(sin(millis()/140.0f)*3.0f);
    display->fillCircle(cx+13, cy-8+wy, 3, MAINCOLOR);
    display->fillRoundRect(cx-12, cy+17, 3, 8, 1, MAINCOLOR);
  }
  else if(c_arm == 3){
    // Scratching head with right arm (confused)
    display->fillCircle(cx+13+fTilt, cy-2-fTilt, 3, MAINCOLOR);
    display->fillRoundRect(cx-12, cy+17, 3, 8, 1, MAINCOLOR);
  }
  else if(c_arm == 4){
    // Angry fists
    display->fillCircle(cx-11, cy+22, 3, MAINCOLOR);
    display->fillCircle(cx+11, cy+22, 3, MAINCOLOR);
  }
  else {
    // Arms hanging down
    display->fillRoundRect(cx-12, cy+17, 3, 8, 1, MAINCOLOR);
    display->fillRoundRect(cx+9, cy+17, 3, 8, 1, MAINCOLOR);
  }

  //************************************* LEGS *************************************
  display->fillRect(cx-7, cy+28, 5, 4, MAINCOLOR);
  display->fillRect(cx+2, cy+28, 5, 4, MAINCOLOR);

  //************************************* FEET *************************************
  display->fillRoundRect(cx-8, cy+32, 7, 4, 2, MAINCOLOR);
  display->fillRoundRect(cx+1, cy+32, 7, 4, 2, MAINCOLOR);
  // ankle separation
  display->fillRect(cx-7, cy+32, 5, 1, BGCOLOR);
  display->fillRect(cx+2, cy+32, 5, 1, BGCOLOR);

} // end of drawSnoopy


// Draw animated sweat drops in the upper screen area
void drawSweatDrops(){
  if(sweat){
    // Sweat drop 1 -> left corner
    if(sweat1YPos <= sweat1YPosMax){sweat1YPos+=0.5;}
    else {sweat1XPosInitial = random(30); sweat1YPos = 2; sweat1YPosMax = (random(10)+10); sweat1Width = 1; sweat1Height = 2;}
    if(sweat1YPos <= sweat1YPosMax/2){sweat1Width+=0.5; sweat1Height+=0.5;}
    else {sweat1Width-=0.1; sweat1Height-=0.5;}
    sweat1XPos = sweat1XPosInitial-(sweat1Width/2);
    display->fillRoundRect(sweat1XPos, sweat1YPos, sweat1Width, sweat1Height, 3, MAINCOLOR);

    // Sweat drop 2 -> center area
    if(sweat2YPos <= sweat2YPosMax){sweat2YPos+=0.5;}
    else {sweat2XPosInitial = random((screenWidth-60))+30; sweat2YPos = 2; sweat2YPosMax = (random(10)+10); sweat2Width = 1; sweat2Height = 2;}
    if(sweat2YPos <= sweat2YPosMax/2){sweat2Width+=0.5; sweat2Height+=0.5;}
    else {sweat2Width-=0.1; sweat2Height-=0.5;}
    sweat2XPos = sweat2XPosInitial-(sweat2Width/2);
    display->fillRoundRect(sweat2XPos, sweat2YPos, sweat2Width, sweat2Height, 3, MAINCOLOR);

    // Sweat drop 3 -> right corner
    if(sweat3YPos <= sweat3YPosMax){sweat3YPos+=0.5;}
    else {sweat3XPosInitial = (screenWidth-30)+(random(30)); sweat3YPos = 2; sweat3YPosMax = (random(10)+10); sweat3Width = 1; sweat3Height = 2;}
    if(sweat3YPos <= sweat3YPosMax/2){sweat3Width+=0.5; sweat3Height+=0.5;}
    else {sweat3Width-=0.1; sweat3Height-=0.5;}
    sweat3XPos = sweat3XPosInitial-(sweat3Width/2);
    display->fillRoundRect(sweat3XPos, sweat3YPos, sweat3Width, sweat3Height, 3, MAINCOLOR);
  }
} // end of drawSweatDrops


void drawEyes(){

  //// PRE-CALCULATIONS - TWEENINGS ////

  // Clamp target positions so Snoopy can never walk off screen
  if(eyeLxNext < SN_XMARGIN) eyeLxNext = SN_XMARGIN;
  if(eyeLxNext > getScreenConstraint_X()) eyeLxNext = getScreenConstraint_X();
  if(eyeLyNext < SN_YMARGIN) eyeLyNext = SN_YMARGIN;
  if(eyeLyNext > getScreenConstraint_Y()) eyeLyNext = getScreenConstraint_Y();

  // Smoothly move Snoopy towards his target position
  eyeLx = (eyeLx + eyeLxNext)/2;
  eyeLy = (eyeLy + eyeLyNext)/2;
  if(eyeLx < SN_XMARGIN) eyeLx = SN_XMARGIN;
  if(eyeLx > getScreenConstraint_X()) eyeLx = getScreenConstraint_X();
  if(eyeLy < SN_YMARGIN) eyeLy = SN_YMARGIN;
  if(eyeLy > getScreenConstraint_Y()) eyeLy = getScreenConstraint_Y();

  // Smoothly morph emotion drivers towards their targets
  c_ear    = tweenInt(c_ear, t_ear);
  c_brow   = tweenInt(c_brow, t_brow);
  c_mouth  = tweenInt(c_mouth, t_mouth);
  c_arm    = tweenInt(c_arm, t_arm);
  c_energy = tweenInt(c_energy, t_energy);
  specialEyes  = t_spE;
  specialMouth = t_spM;
  eyeOpenCur = (eyeOpenCur + eyeOpenTarget)*0.5f;

  // Constant gentle breathing so Snoopy always feels alive
  int breathe = (int)(sin(millis()/400.0f)*1.0f);
  // Mood-driven bounce (happy 1px, yay 3px)
  int bob = (int)(sin(millis()/150.0f)*(float)c_energy);

  int sx = eyeLx;
  int sy = eyeLy - breathe - bob;

  //// APPLYING MACRO ANIMATIONS ////

	if(autoblinker){
		if(millis() >= blinktimer){
		  blink();
		  blinktimer = millis()+(blinkInterval*1000)+(random(blinkIntervalVariation)*1000);
		}
	}

  // Laughing - Snoopy shakes up and down
  if(laugh){
    if(laughToggle){
      setVFlicker(1, 2);
      laughAnimationTimer = millis();
      laughToggle = 0;
    } else if(millis() >= laughAnimationTimer+laughAnimationDuration){
      setVFlicker(0, 0);
      laughToggle = 1;
      laugh=0;
    }
  }

  // Confused - Snoopy shakes left and right
  if(confused){
    if(confusedToggle){
      setHFlicker(1, 2);
      confusedAnimationTimer = millis();
      confusedToggle = 0;
    } else if(millis() >= confusedAnimationTimer+confusedAnimationDuration){
      setHFlicker(0, 0);
      confusedToggle = 1;
      confused=0;
    }
  }

  // Idle - Snoopy wanders to random positions on screen
  if(idle){
    if(millis() >= idleAnimationTimer){
      eyeLxNext = random(SN_XMARGIN, getScreenConstraint_X()+1);
      eyeLyNext = random(SN_YMARGIN, getScreenConstraint_Y()+1);
      idleAnimationTimer = millis()+(idleInterval*1000)+(random(idleIntervalVariation)*1000);
    }
  }

  // Adding offset for horizontal flickering/shivering
  if(hFlicker){
    if(hFlickerAlternate) { sx += hFlickerAmplitude; }
    else { sx -= hFlickerAmplitude; }
    hFlickerAlternate = !hFlickerAlternate;
  }

  // Adding offset for vertical flickering/shivering
  if(vFlicker){
    if(vFlickerAlternate) { sy += vFlickerAmplitude; }
    else { sy -= vFlickerAmplitude; }
    vFlickerAlternate = !vFlickerAlternate;
  }

  //// ACTUAL DRAWINGS ////

  display->clearDisplay(); // start with a blank screen

  // Draw pixel-art Snoopy (dead: slumps down, tilted to the side)
  if(specialEyes == 3){ drawSnoopy(sx-2, sy+2); }
  else { drawSnoopy(sx, sy); }

  // Draw animated sweat drops on top
  drawSweatDrops();

  display->display(); // show drawings on display

} // end of drawEyes method


}; // end of class RoboEyes


#endif