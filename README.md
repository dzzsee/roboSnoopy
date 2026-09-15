# RoboSnoopy Library

A pixel-art **Snoopy mascot** for OLED displays that reflects emotions and walks around the screen. It is built on top of the same ideas and API as the FluxGarage RoboEyes library (Dennis Hoelscher), which is also included in this repository for reference.

The sprite was designed as editable ASCII pixel-art (see `tools/art/`) and is packed into `PROGMEM` bitmaps by `tools/spritegen.py`. Black features of Snoopy (ears, nose, eyes, mouth, eyebrows) are "punched" onto the white fur with the background color - the same trick the RoboEyes library uses for its eyelids. All motion is interpolated and framerate-limited, so the animations stay smooth without `delay()`.

> Snoopy is a trademark of Peanuts Worldwide. This library is intended for **personal and educational** use only - do not use it commercially.

![RoboSnoopy art preview - replace with your own render](./tools/art/snoopy_frame1.txt)

## Installation

1. In the Arduino IDE, navigate to "Sketch > Include Library > Add .ZIP Library" and select this repository's folder (or its zip).
2. Make sure the *Adafruit GFX Library* is installed (Sketch > Include Library > Manage Libraries > search "Adafruit GFX").

## Minimal Example

```cpp
#include <Adafruit_SSD1306.h>
#include <RoboSnoopy.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
RoboSnoopy<Adafruit_SSD1306> snoopy(display);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  snoopy.begin(128, 64, 60);       // width, height, max framerate
  snoopy.setIdleMode(ON, 2, 2);    // wander to random positions
  snoopy.setAutoblinker(ON, 4, 3); // blink once in a while
}

void loop() {
  snoopy.update();                 // draw (framerate limited)
}
```

## Functions

### General
- **begin()** _(screen-width, screen-height, max framerate)_
- **update()** _draws a frame in the main loop, limited by the max framerate from begin()_
- **drawSnoopy()** _same as update(), but without the framerate limitation_
- **setDisplayColors()** _(uint8_t background, uint8_t main)_ -> use 0/1 for monochrome OLEDs (e.g. SSD1306) or 0x00/0x0F for grayscale OLEDs (e.g. SSD1322)

### Movement
- **setPosition()** _set a walking target: N, NE, E, SE, S, SW, W, NW or DEFAULT (screen center). Snoopy walks there toggling its legs and facing the direction of travel_
- **setIdleMode()** _(bool active, int interval, int variation) -> Snoopy walks to a new random position every few seconds_
- **setSpeed()** _(int pixelsPerSecond)_
- **setScale()** _(byte 1 or 2) -> doubles the pixel size (slower drawing, OK for small screens)_
- **getScreenConstraint_X() / getScreenConstraint_Y()** _max coordinates so the whole sprite stays on screen_

### Mood Expressions
Snoopy changes both its face and its speed:
- **setMood()** _DEFAULT, TIRED, ANGRY, HAPPY, SURPRISED, CURIOUS_
  - `HAPPY` -> smiles, hops while walking, a bit faster
  - `TIRED` -> half-closed eyes, slow lazy stroll
  - `ANGRY` -> slanted eyebrows and frown, walks fast
  - `SURPRISED` -> big round eyes, freezes / sneaks slowly
  - `CURIOUS` -> raised eyebrow, slow approach

### Eye Control & One-shot Animations
- **open() / close()** _close() falls asleep (eyes closed), open() wakes up_
- **blink()** _one blink_
- **setAutoblinker()** _(bool active, int interval, int variation) -> random blinking_
- **anim_laugh()** _one-shot: hops and shakes up and down_
- **anim_confused()** _one-shot: shakes left and right_

### Shiver
- **setHFlicker() / setVFlicker()** _(bool, byte amplitude) -> constant horizontal/vertical shivering, same as the flicker of the RoboEyes library_

## Examples
- **i2c_SSD1306_RoboSnoopy_Basics** - wandering Snoopy that cycles through moods and plays one-shot animations on its own.
- **i2c_SSD1306_RoboSnoopy_Sequence** - a scripted show: dance, get tired, fall asleep, wake up angry and run - all without `delay()`.

## Regenerate the Pixel Art (optional)
```bash
python3 tools/painter.py       # rebuild ASCII art (dev-time tweaking)
python3 tools/spritegen.py tools/art/snoopy_frame1.txt tools/art/snoopy_frame2.txt > /tmp/out.h
python3 tools/render_png.py tools/art/*.txt  # optional preview images
```
Edit the art in `tools/art/`, re-run, and copy the generated arrays into `src/RoboSnoopy_sprites.h`.

---

# FluxGarage RoboEyes (original)

Draws smoothly animated robot eyes on OLED displays, using the Adafruit GFX library. Robot eye shapes are configurable in terms of width, height, border radius and space between. Several different mood expressions (happy, tired, angry, default) and animations (autoblinker, idle, laughing, confused) are available. All state changes have smooth transitions and thus, complex animation sequences are easily feasible.

Developing this library was the first step of a larger project: the creation of my own DIY robot from the design perspective. Check out the [FluxGarage Youtube Channel](https://www.youtube.com/@FluxGarage).

## Watch the Demo and Getting Started Videos on Youtube

[![#1 - Smoothly Animated Robot Eyes on OLED Displays with the Robo Eyes Library](https://img.youtube.com/vi/ibSaDEkfUOI/0.jpg)](https://www.youtube.com/watch?v=ibSaDEkfUOI)
[![#2 - Getting Started With the Free Robo Eyes Arduino Library](https://img.youtube.com/vi/WtLWc5zzrmI/0.jpg)](https://www.youtube.com/watch?v=WtLWc5zzrmI)

## Functions of RoboEyes

### General
- **begin()** _(screen-width, screen-height, max framerate)_
- **update()** _update eyes drawings in the main loop, limited by max framerate as defined in begin()_
- **drawEyes()** _same as update(), but without the framerate limitation_
- **setDisplayColors()** _(uint8_t background, uint8_t main)_
-> background: background and overlays, choose 0 for monochrome displays and 0x00 for grayscale displays such as SSD1322
-> main: drawings, choose 1 for monochrome displays and 0x0F for grayscale displays such as SSD1322 (0x0F = maximum brightness)

### Define Eye Shapes, all values in pixels
- **setWidth()** _(byte leftEye, byte rightEye)_
- **setHeight()** _(byte leftEye, byte rightEye)_
- **setBorderradius()** _(byte leftEye, byte rightEye)_
- **setSpacebetween()** _(int space) -> can also be negative_
- **setCyclops()** _(bool ON/OFF) -> if turned ON, robot has only on eye_

### Define Face Expressions (Mood, Curiosity, Eye-Position, Open/Close)
- **setMood()** _mood expression, can be TIRED, ANGRY, HAPPY, DEFAULT_
- **setPosition()** _cardinal directions, can be N, NE, E, SE, S, SW, W, NW, DEFAULT (default = horizontally and vertically centered)_
- **setCuriosity()** _(bool ON/OFF) -> when turned on, height of the outer eyes increases when moving to the very left or very right_
- **setSweat()** _(bool ON/OFF) -> when turned on, animated sweat drops appear in the upper screen area_
- **open()** _open both eyes -> open(1,0) opens left eye only_
- **close()** _close both eyes -> close(1,0) closes left eye only_

### Set Horizontal and/or Vertical Flicker
Alternately displaces the eyes in the defined amplitude in pixels:
- **setHFlicker()** _(bool ON/OFF, byte amplitude)_
- **setVFlicker()** _(bool ON/OFF, byte amplitude)_

### Play Prebuilt Oneshot Animations
- **anim_confused()** _confused -> eyes shaking left and right_
- **anim_laugh()** _laughing -> eyes shaking up and down_
- **blink()** _close and open both eyes_
- **blink(0,1)** _close and open right eye_

### Macro Animators
Blinks both eyes randomly:
- **setAutoblinker()** _(bool ON/OFF, int interval, int variation)_

Repositions both eyes randomly:
- **setIdleMode()** _(bool ON/OFF, int interval, int variation)_

### Further (Inofficial) Resources by Other Users
- micropython-roboeyes by mchobby: https://github.com/mchobby/micropython-roboeyes
- RoboEyes Micropython Edition by Youssef Tech: https://github.com/yousseftechdev/RoboEyes-Micropython
- RoboEyes for TFT displays by Youssef Tech: https://github.com/yousseftechdev/RoboEyesTFT
- MQTT control system by teletoby-swctv: https://github.com/teletoby-swctv/FluxGarage-RoboEyes-MQTT