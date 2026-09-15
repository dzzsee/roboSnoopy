/*
 * RoboSnoopy for OLED Displays V 1.0.0
 * A pixel-art Snoopy mascot that reflects emotions, walks around the screen
 * and reacts to mood changes, based on the FluxGarage RoboEyes library by
 * Dennis Hoelscher (www.fluxgarage.com).
 *
 * The sprite art is stored in RoboSnoopy_sprites.h (generated from
 * tools/art/) and drawn with the Adafruit GFX library. Black features
 * (ears, nose, eyes, mouth) are "punched" onto the white fur with the
 * background color, exactly like the eyelids in the original RoboEyes.
 *
 * Snoopy is a trademark of Peanuts Worldwide; this library is for personal
 * and educational use only. Do not use it commercially.
 */

#ifndef _ROBSNOOPY_H
#define _ROBSNOOPY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>

#include "RoboSnoopy_sprites.h"


// Mood switches (kept compatible with the RoboEyes library values)
#ifndef DEFAULT
#define DEFAULT 0
#endif
#ifndef TIRED
#define TIRED 1
#endif
#ifndef ANGRY
#define ANGRY 2
#endif
#ifndef HAPPY
#define HAPPY 3
#endif
#define SURPRISED 4
#define CURIOUS 5

// For turning things on or off
#ifndef ON
#define ON 1
#endif
#ifndef OFF
#define OFF 0
#endif

// For predefined target positions
#ifndef N
#define N  1 // north, top center
#endif
#ifndef NE
#define NE 2 // north-east, top right
#endif
#ifndef E
#define E  3 // east, middle right
#endif
#ifndef SE
#define SE 4 // south-east, bottom right
#endif
#ifndef S
#define S  5 // south, bottom center
#endif
#ifndef SW
#define SW 6 // south-west, bottom left
#endif
#ifndef W
#define W  7 // west, middle left
#endif
#ifndef NW
#define NW 8 // north-west, top left
#endif


//*********************************************************************************************
//  RoboSnoopy - pixel-art Snoopy mascot
//*********************************************************************************************

template<typename AdafruitDisplay>
class RoboSnoopy
{
private:

  // Small sine-style lookup table (16 entries) for idle bob and happy hops
  static const int8_t bobTable[16];

  // Origin of the blit actually drawn this frame (body + face stay aligned
  // even while the sprite bobs or shivers)
  int blitX = 0;
  int blitY = 0;

  // Map a local art coordinate to its on-screen position, honoring mirroring
  int flipX(int localX) {
    if (mirror) { return blitX + (SN_SPRITE_WIDTH - 1 - localX) * scale; }
    return blitX + localX * scale;
  }
  int flipY(int localY) {
    return blitY + localY * scale;
  }

  // Draw one sprite frame at (dx, dy), honoring mirroring and scale.
  // Mirrored frames are pre-generated in flash (AVR-safe: drawBitmap reads
  // through pgm_read_byte), so walking left just swaps to *_mirror sprites.
  void drawBlit(int dx, int dy, byte frame) {
    const uint8_t *bmp;
    if (frame) {
      bmp = mirror ? snoopy_frame2_mirror : snoopy_frame2;
    } else {
      bmp = mirror ? snoopy_frame1_mirror : snoopy_frame1;
    }
    if (scale == 1) {
      display->drawBitmap(dx, dy, bmp, SN_SPRITE_WIDTH, SN_SPRITE_HEIGHT, mainColor);
    } else {
      int bytesPerRow = (SN_SPRITE_WIDTH + 7) / 8;
      for (int py = 0; py < SN_SPRITE_HEIGHT; py++) {
        for (int px = 0; px < SN_SPRITE_WIDTH; px++) {
          if (pgm_read_byte(bmp + py * bytesPerRow + (px >> 3)) & (0x80 >> (px & 7))) {
            display->fillRect(dx + px * scale, dy + py * scale, scale, scale, mainColor);
          }
        }
      }
    }
  }

  // Punch a single pixel-sized feature (black) at local art coordinates
  void punch(int localX, int localY) {
    display->fillRect(flipX(localX), flipY(localY), scale, scale, bgColor);
  }

  // Punch a horizontal dash (used for closed / tired eyes)
  void punchDash(int localX, int localY, int len) {
    display->fillRect(flipX(localX), flipY(localY), len * scale, scale, bgColor);
  }

  // Face region anchors (local art coordinates)
  int eyeY()     { return 12; }
  int eye1X()    { return 34; }
  int eye2X()    { return 38; }
  int browY()    { return 10; }
  int mouthX()   { return 43; }
  int mouthY()   { return 20; }

  // Draw eyes + mouth per mood, as black features on the white fur
  void drawFaceMood() {
    if (blinkClosed || sleep) {
      // Closed eyes (asleep or blinking) - two short dashes
      punchDash(eye1X() - 1, eyeY() + 1, 3);
      punchDash(eye2X() - 1, eyeY() + 1, 3);
      return;
    }
    switch (mood) {
      case HAPPY:
        // "^^" squinting happy eyes
        punch(eye1X() - 1, eyeY() + 1);
        punch(eye1X(),     eyeY()    );
        punch(eye1X() + 1, eyeY() + 1);
        punch(eye2X() - 1, eyeY() + 1);
        punch(eye2X(),     eyeY()    );
        punch(eye2X() + 1, eyeY() + 1);
        // open smile
        punch(mouthX() - 2, mouthY() - 1);
        punch(mouthX() - 1, mouthY()    );
        punch(mouthX(),     mouthY() + 1);
        punch(mouthX() + 1, mouthY()    );
        punch(mouthX() + 2, mouthY() - 1);
        break;
      case ANGRY:
        // slanted eyebrows + slitted eyes + frown
        punch(eye1X() - 1, eyeY() - 2);
        punch(eye1X(),     eyeY() - 2);
        punch(eye2X(),     eyeY() - 2);
        punch(eye2X() + 1, eyeY() - 2);
        punch(eye1X(),     eyeY() + 1);   // narrowed eyes
        punch(eye2X(),     eyeY() + 1);
        punch(mouthX() - 1, mouthY() + 1);
        punch(mouthX(),     mouthY() + 2);
        punch(mouthX() + 1, mouthY() + 1);
        break;
      case TIRED:
        // droopy half-closed eyes
        punchDash(eye1X() - 1, eyeY(),     3);
        punchDash(eye2X() - 1, eyeY(),     3);
        punch(eye1X() - 1,     eyeY() + 2);
        punch(eye2X() - 1,     eyeY() + 2);
        break;
      case SURPRISED:
        punch(eye1X() - 1, eyeY() - 1); punch(eye1X(), eyeY() - 1);
        punch(eye1X() - 1, eyeY()    ); punch(eye1X(), eyeY()    );
        punch(eye2X() - 1, eyeY() - 1); punch(eye2X(), eyeY() - 1);
        punch(eye2X() - 1, eyeY()    ); punch(eye2X(), eyeY()    );
        punch(mouthX() - 1, mouthY() - 1); punch(mouthX(), mouthY() - 1);
        punch(mouthX() - 1, mouthY()    ); punch(mouthX(), mouthY()    );
        break;
      case CURIOUS:
        // normal dots + one raised eyebrow, goes with slow approach
        punch(eye1X(), eyeY());
        punch(eye2X(), eyeY());
        punchDash(eye2X() - 1, eyeY() - 2, 3);
        punch(mouthX() - 1, mouthY() - 1);
        punch(mouthX(),     mouthY()    );
        punch(mouthX() + 1, mouthY() - 1);
        break;
      default: // DEFAULT - two neutral dot eyes
        punch(eye1X(), eyeY());
        punch(eye2X(), eyeY());
        break;
    }
  }

  // Effective speed of the current mood in pixels per second
  int moodSpeed() {
    switch (mood) {
      case HAPPY:     return walkSpeed * 6 / 4;
      case ANGRY:     return walkSpeed * 3 / 2;
      case TIRED:     return walkSpeed / 2;
      case SURPRISED: return walkSpeed / 3;
      case CURIOUS:   return walkSpeed * 3 / 5;
      default:        return walkSpeed;
    }
  }

public:

  // Reference to the Adafruit display object
  AdafruitDisplay *display;

  // General setup - screen size and max. frame rate
  int screenWidth = 128;
  int screenHeight = 64;
  int frameInterval = 20;             // ms per frame (1000 / fps)
  unsigned long frameTimer = 0;

  // Display colors
  uint8_t bgColor = 0;                // background / punches (0 = black)
  uint8_t mainColor = 1;              // drawings (1 = white)

  // Current mood expression
  byte mood = DEFAULT;

  // Position state: current top-left corner and walking target
  int x = 0, y = 0;
  int tx = 0, ty = 0;
  float fx = 0.0f, fy = 0.0f;        // fractional sub-pixel accumulators
  bool moving = false;
  bool mirror = false;                // true when facing left
  byte walkFrame = 0;

  // Movement settings
  int walkSpeed = 55;                 // pixels per second
  byte scale = 1;                     // pixel scale: 1 or 2

  // Idle mode - Snoopy walks to random positions
  bool idleMode = 0;
  int idleInterval = 2;
  int idleIntervalVariation = 3;
  unsigned long idleTimer = 0;

  // Auto blinking
  bool autoblink = 0;
  int blinkInterval = 3;
  int blinkIntervalVariation = 2;
  unsigned long blinkTimer = 0;
  bool blinkClosed = 0;
  unsigned long blinkHoldUntil = 0;
  bool sleep = 0;                     // eyes closed (open()/close())

  // One-shot animations
  bool laughing = 0;
  unsigned long laughHoldUntil = 0;
  bool laughToggle = 1;
  bool confused = 0;
  unsigned long confuseHoldUntil = 0;
  bool confuseToggle = 1;

  // Shiver / flicker
  bool hShiver = 0;
  byte hShiverAmp = 2;
  bool hShiverAlt = 0;
  bool vShiver = 0;
  byte vShiverAmp = 2;
  bool vShiverAlt = 0;

  // Step animation bookkeeping
  unsigned int stepDist = 0;


  // Constructor: takes a reference to the active Adafruit display object
  // Eg: RoboSnoopy<Adafruit_SSD1306> snoopy(display);
  RoboSnoopy(AdafruitDisplay &disp) : display(&disp) {}


  //*********************************************************************************************
  //  GENERAL METHODS
  //*********************************************************************************************

  // Startup Snoopy with screen-width, screen-height and max. frames per second
  void begin(int width, int height, byte frameRate) {
    screenWidth = width;
    screenHeight = height;
    display->clearDisplay();
    display->display();
    setFramerate(frameRate);
    x = tx = (screenWidth  - SN_SPRITE_WIDTH  * scale) / 2;
    y = ty = (screenHeight - SN_SPRITE_HEIGHT * scale) / 2;
  }

  // Limit drawing updates to the max. framerate defined by begin()
  void update() {
    if (millis() - frameTimer >= (unsigned long)frameInterval) {
      drawSnoopy();
      frameTimer = millis();
    }
  }

  // Draw one full Snoopy frame immediately (without the framerate gating)
  void drawSnoopy() {

    //// TIMED BEHAVIOUR ////

    if (autoblink) {
      if (millis() >= blinkTimer) {
        blink();
        blinkTimer = millis() + (unsigned long)blinkInterval * 1000UL +
                     (unsigned long)random(blinkIntervalVariation * 1000 + 1);
      }
    }
    if (blinkClosed && millis() >= blinkHoldUntil && !sleep) {
      blinkClosed = 0;
    }
    if (laughing) {
      if (laughToggle) {
        vShiver = 1; vShiverAmp = 3; vShiverAlt = 0;
        laughHoldUntil = millis() + 600;
        laughToggle = 0;
      } else if (millis() >= laughHoldUntil) {
        vShiver = 0;
        laughToggle = 1;
        laughing = 0;
      }
    }
    if (confused) {
      if (confuseToggle) {
        hShiver = 1; hShiverAmp = 3; hShiverAlt = 0;
        confuseHoldUntil = millis() + 500;
        confuseToggle = 0;
      } else if (millis() >= confuseHoldUntil) {
        hShiver = 0;
        confuseToggle = 1;
        confused = 0;
      }
    }

    //// MOVEMENT ////

    if (idleMode) {
      if (millis() >= idleTimer) {
        tx = random(getScreenConstraint_X() + 1);
        ty = random(getScreenConstraint_Y() + 1);
        idleTimer = millis() + (unsigned long)idleInterval * 1000UL +
                    (unsigned long)random(idleIntervalVariation * 1000 + 1);
      }
    }

    int dx = tx - x;
    int dy = ty - y;
    float dist = (float)sqrt((long)dx * dx + (long)dy * dy);
    if (dist > 0.001f) {
      float step = moodSpeed() * (frameInterval / 1000.0f);
      if (dist <= step) {
        x = tx; y = ty; fx = 0; fy = 0;
        moving = false;
      } else {
        fx += dx / dist * step;               // accumulate sub-pixel motion
        fy += dy / dist * step;
        int ix = (int)fx;
        int iy = (int)fy;
        if (ix) { x += ix; fx -= ix; }
        if (iy) { y += iy; fy -= iy; }
        moving = true;
        stepDist += (unsigned int)(step + 0.5f);
        if (stepDist >= 8) {
          walkFrame ^= 1;
          stepDist = 0;
        }
        if (dx > 0) { mirror = false; }
        else if (dx < 0) { mirror = true; }
      }
    } else {
      moving = false;
    }

    // Keep Snoopy on screen
    int maxX = getScreenConstraint_X();
    int maxY = getScreenConstraint_Y();
    if (x < 0) { x = 0; }
    if (y < 0) { y = 0; }
    if (x > maxX) { x = maxX; }
    if (y > maxY) { y = maxY; }

    //// SMOOTH OFFSETS (bob, shiver, hops) ////

    int drawOffsetX = 0;
    int drawOffsetY = 0;

    if (hShiver) {
      if (hShiverAlt) { drawOffsetX = hShiverAmp; }
      else            { drawOffsetX = -hShiverAmp; }
      hShiverAlt = !hShiverAlt;
    }
    if (vShiver) {
      if (vShiverAlt) { drawOffsetY = vShiverAmp; }
      else            { drawOffsetY = -vShiverAmp; }
      vShiverAlt = !vShiverAlt;
    }

    // Gentle idle bob; a livelier hop when walking happy
    if (!moving || mood == HAPPY) {
      unsigned long stepMs = (mood == HAPPY) ? 30 : 90;
      int bob = (bobTable[(millis() / stepMs) & 15] >> 3) - 1;
      drawOffsetY += bob;
    }

    int sx = x + drawOffsetX;
    int sy = y + drawOffsetY;
    blitX = sx;
    blitY = sy;

    //// DRAWING ////

    display->clearDisplay();
    drawBlit(sx, sy, walkFrame);
    drawFaceMood();
    display->display();
  }


  //*********************************************************************************************
  //  SETTER METHODS
  //*********************************************************************************************

  // Calculate frame interval based on the defined frameRate
  void setFramerate(byte fps) {
    frameInterval = 1000 / fps;
  }

  // Set color values (0/1 for monochrome displays, 0x00/0x0F for grayscale)
  void setDisplayColors(uint8_t background, uint8_t main) {
    bgColor = background;
    mainColor = main;
  }

  // Set mood expression
  void setMood(unsigned char m) {
    switch (m) {
      case TIRED: case ANGRY: case HAPPY: case SURPRISED: case CURIOUS:
        mood = m; break;
      default:
        mood = DEFAULT; break;
    }
  }

  // Set walking speed in pixels per second
  void setSpeed(int pxPerSecond) {
    walkSpeed = pxPerSecond;
  }

  // Set pixel scale: 1 (default) or 2 (doubled pixels, slower drawing)
  void setScale(byte s) {
    scale = (s < 1) ? 1 : (s > 2 ? 2 : s);
    x = tx = (screenWidth  - SN_SPRITE_WIDTH  * scale) / 2;
    y = ty = (screenHeight - SN_SPRITE_HEIGHT * scale) / 2;
  }

  // Walk to a predefined position
  void setPosition(unsigned char position) {
    int cx = getScreenConstraint_X();
    int cy = getScreenConstraint_Y();
    switch (position) {
      case N:  tx = cx / 2;          ty = 0;          break;
      case NE: tx = cx;              ty = 0;          break;
      case E:  tx = cx;              ty = cy / 2;     break;
      case SE: tx = cx;              ty = cy;         break;
      case S:  tx = cx / 2;          ty = cy;         break;
      case SW: tx = 0;               ty = cy;         break;
      case W:  tx = 0;               ty = cy / 2;     break;
      case NW: tx = 0;               ty = 0;          break;
      default: tx = cx / 2;          ty = cy / 2;     break;
    }
  }

  // Idle mode - automated random repositioning
  void setIdleMode(bool active, int interval, int variation) {
    idleMode = active;
    idleInterval = interval;
    idleIntervalVariation = variation;
  }
  void setIdleMode(bool active) {
    idleMode = active;
  }

  // Random blinking
  void setAutoblinker(bool active, int interval, int variation) {
    autoblink = active;
    blinkInterval = interval;
    blinkIntervalVariation = variation;
  }
  void setAutoblinker(bool active) {
    autoblink = active;
  }

  // Horizontal shiver / flicker
  void setHFlicker(bool flickerBit, byte amplitude) {
    hShiver = flickerBit;
    hShiverAmp = amplitude;
  }
  void setHFlicker(bool flickerBit) {
    hShiver = flickerBit;
  }

  // Vertical shiver / flicker
  void setVFlicker(bool flickerBit, byte amplitude) {
    vShiver = flickerBit;
    vShiverAmp = amplitude;
  }
  void setVFlicker(bool flickerBit) {
    vShiver = flickerBit;
  }


  //*********************************************************************************************
  //  GETTER METHODS
  //*********************************************************************************************

  // Max x position so the whole sprite stays on screen
  int getScreenConstraint_X() {
    return screenWidth - SN_SPRITE_WIDTH * scale;
  }

  // Max y position so the whole sprite stays on screen
  int getScreenConstraint_Y() {
    return screenHeight - SN_SPRITE_HEIGHT * scale;
  }


  //*********************************************************************************************
  //  ANIMATION METHODS
  //*********************************************************************************************

  // Close eyes (Snoopy falls asleep)
  void close() {
    sleep = 1;
    blinkClosed = 1;
  }

  // Open eyes
  void open() {
    sleep = 0;
    blinkClosed = 0;
  }

  // Trigger a blink
  void blink() {
    blinkClosed = 1;
    blinkHoldUntil = millis() + 90;
  }

  // One-shot: laughing / hopping up and down
  void anim_laugh() {
    laughing = 1;
  }

  // One-shot: confused, shaking left and right
  void anim_confused() {
    confused = 1;
  }
};


// Static member definitions (template-friendly: one set per instantiation, weak linkage)
template<typename AdafruitDisplay>
const int8_t RoboSnoopy<AdafruitDisplay>::bobTable[16] = {
  0, 6, 12, 16, 19, 21, 22, 23, 23, 22, 21, 19, 16, 12, 6, 0
};


#endif // _ROBSNOOPY_H