#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define LED_PIN 6     //light strip connect to Pin D6
#define BUTTON_PIN 5  // Button connected to Pin D5
#define NUMPIXELS 12  //number of Led on the light strip

#define NUMSETTINGS 10
#define SETTING_ADDRESS 0

#include <FlashStorage.h>
FlashStorage(SETTING_STORAGE, int);



Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
//create a new light strip object to define the data pattern

#define DELAYVAL 500  //time (in milliseconds) to pause between pixels

int light_setting = 8;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();  //intialize NeoPixel strip object
  pixels.setBrightness(20);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  light_setting = SETTING_STORAGE.read();
}

/**
 * Increment light setting and perform mod.
 */
void update_light_setting() {
  light_setting = (light_setting + 1) % NUMSETTINGS;
  SETTING_STORAGE.write(light_setting);
  Serial.println(light_setting);
}

/**
 * Return true if button was clicked, and wait a delay 
 * for debounce.
 */
bool button_was_clicked() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
    return true;
  }
  return false;
}

/**
 * Wait for n miliseconds, returning true if the button was clicked
 */
bool button_wait(int n, int t_step = 1) {
  for (int i = 0; i < n; i++) {
    if (button_was_clicked()) {
      return true;
    }
    delay(t_step);
  }
  return false;
}

void rainbow(int wait) {
  while (true) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      if (button_was_clicked()) {
        update_light_setting();
        return;
      }

      for (int i = 0; i < pixels.numPixels(); i++) {
        int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      }
      pixels.show();
      delay(wait);
    }
  }
}

int transformSmallerRange(int min_out, int max_out, int min_in, int max_in, int inputVal) {
  int range_out = max_out - min_out;
  int midpt = (max_in + min_in) / 2;

  if (inputVal < midpt) {
    return min_out + (inputVal / midpt ) * range_out;
  }
  else {
    return min_out + ( 1 - ((inputVal - midpt) / midpt) ) * range_out;
  }
}

// void gold_cycle(int wait) {
//   while (true) {
//     for (long firstPixelHue = 0; firstPixelHue <  65536; firstPixelHue += 256) {
//       if (button_was_clicked()) {
//         update_light_setting();
//         return;
//       }

//       for (int i = 0; i < pixels.numPixels(); i++) {
//         int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
//         int pixelHueTrans = transformSmallerRange(0, 65536 / 12, 0, 65536, pixelHue);
//         pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHueTrans)));
//       }
//       pixels.show();
//       delay(wait);
//     }
//   }
// }

bool fade_on(int pix, int r, int g, int b, int start_brightness, int end_brightness) {
  for (float br = start_brightness; br <= end_brightness; br++) {
    // float brightness = start_brightness + (end_brightness - start_brightness) * b / 1000;
    // pixels.setPixelColor(pix, pixels.Color(r / 256 * br, g / 256 * br, b / 256 * br));
    pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
    pixels.show();
    if (button_wait(1)) {
      return false;
    }
  }
  return true;
}

bool fade_off(int pix, int r, int g, int b, int start_brightness, int end_brightness, int off_brightness) {
  for (float br = start_brightness; br >= end_brightness; br--) {
    // float brightness = start_brightness + (end_brightness - start_brightness) * b / 1000;
    // pixels.setPixelColor(pix, pixels.Color(r / 256 * br, g / 256 * br, b / 256 * br));
    pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
    pixels.show();
    if (button_wait(1)) {
      return false;
    }
  }
  float br = float(off_brightness);
  pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
  pixels.show();
  return true;
}

void gold_random(int wait) {
  int max_brightness = 150;
  int base_r = 255;
  int base_g = 70;
  int base_b = 0;
  pixels.fill((0, 0, 0));
  pixels.show();
  
  uint32_t base_color = pixels.Color(255, 70, 0);
  int min_br = 50;
  uint32_t off = pixels.Color(base_r * (min_br/256), base_g * (min_br/256), base_b * (min_br/256));

  bool end = false; 

  while (not end) {
    int pix = random(NUMPIXELS);
    uint32_t current_color = pixels.getPixelColor(pix);
    if (current_color == off) {
      if (not fade_on(pix, base_r, base_g, base_b, 100, 255)) {
        break;
      };
    } else {
      if (not fade_off(pix, base_r, base_g, base_b, 255, 100, min_br)) {
        break;
      }
    }

    pixels.show();
    if (button_wait(wait + random(wait / 2, wait / 2))) {
        end = true;
        break;
      }
  }
  update_light_setting();
}

void lightning(int wait) {
  int max_brightness = 150;
  // int base_r = 134;
  // int base_g = 59;
  // int base_b = 255; 
  int base_r = 145;
  int base_g = 205;
  int base_b = 255;
  pixels.fill((0, 0, 0));
  pixels.show();
  
  uint32_t base_color = pixels.Color(base_r, base_g, base_b);
  int min_br = 50;
  uint32_t off = pixels.Color(base_r * (min_br/256), base_g * (min_br/256), base_b * (min_br/256));

  bool end = false; 

  int min_wait = wait;
  int max_wait = 5 * wait;
  int active_wait = max_wait;
  int random_multiplier = 100;
  while (not end) {
    int pix = random(NUMPIXELS);
    uint32_t current_color = pixels.getPixelColor(pix);
    if (current_color == off) {
      pixels.setPixelColor(pix, base_color);
      pixels.show();    
    } else {
      pixels.setPixelColor(pix, off);
    }

    pixels.show();
    if (button_wait(active_wait + random(-active_wait /2, active_wait/2))) {
        end = true;
        break;
      }
    
    if (random(random_multiplier) == 1) {
      if (active_wait == max_wait) {
        active_wait = min_wait;
        random_multiplier = 200;
      } else {
        active_wait = max_wait;
        // pixels.fill(off);
        // pixels.show();
        random_multiplier = 40;        
      }
    }
  }
  update_light_setting();
}

void green() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(12, 150, 15));
  }
  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}

void solid_color(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}

void some_solid_color(int r, int g, int b, int idx_list[], int arr_len) {
  pixels.fill((0, 0, 0));

  // for (byte i=0; i < (sizeof(idx_list) / sizeof(idx_list[0])); i++) {
  for (byte i = 0; i < arr_len; i++) {
    int idx = idx_list[i] % NUMPIXELS;
    pixels.setPixelColor(idx, pixels.Color(r, g, b));
  }

  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}

void red_snake() {
  pixels.clear();
  pixels.show();
  int snake_len = 6;
  int snake_timestep = 200;

  int start_pixel = 0;

  while (not button_wait(snake_timestep)) {
    pixels.clear();

    for (int i = 0; i < snake_len; i++) {
      pixels.setPixelColor((start_pixel + i) % NUMPIXELS, pixels.Color(100, 20, 20));
    }

    for (int i = 0; i < snake_len; i++) {
      pixels.setPixelColor((start_pixel + NUMPIXELS / 2 + i) % NUMPIXELS, pixels.Color(50, 15, 50));
    }

    pixels.show();
    start_pixel = (start_pixel + 1) % NUMPIXELS;
  }

  update_light_setting();
}

void all_on() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}


void night_mode() {
  int max_brightness = 150;
  pixels.fill((0, 0, 0));
  pixels.show();

  bool end = false;

  while (not end) {
    // int pix = random(NUMPIXELS);
    int pix1 = 0;
    int pix2 = 0;
    for (int b = 500; b < 1000; b++) {
      float brightness = max_brightness * b / 1000;
      pixels.setPixelColor(pix1, pixels.Color(brightness, 0.5 * brightness, 0.3 * brightness));
      pixels.setPixelColor(pix2, pixels.Color(brightness, 0.5 * brightness, 0.3 * brightness));

      // pixels.setPixelColor(pix, pixels.Color(brightness, 0, 0));
      pixels.show();
      if (button_wait(10)) {
        end = true;
        break;
      }
    }
    if (end) {
      break;
    }
    button_wait(1000);
    for (int b = 999; b >= 500; b--) {
      float brightness = max_brightness * b / 1000;
      pixels.setPixelColor(pix1, pixels.Color(brightness, 0.5 * brightness, 0.3 * brightness));
      pixels.setPixelColor(pix2, pixels.Color(brightness, 0.5 * brightness, 0.3 * brightness));
      // pixels.setPixelColor(pix, pixels.Color(brightness, , 0));
      pixels.show();
      if (button_wait(10)) {
        end = true;
        break;
      }
    }
    pixels.setPixelColor(pix1, pixels.Color(0, 0, 0));
    pixels.setPixelColor(pix2, pixels.Color(0, 0, 0));
    if (end) {
      break;
    }
    button_wait(1000);
  }

  update_light_setting();
}


void off() {
  pixels.clear();
  pixels.show();

  while (not button_was_clicked()) {
    delay(2);
  }

  update_light_setting();
}

int solid_idxs[3] = { 0, 11 };

void loop() {
  switch (light_setting) {
    case 0:
      rainbow(10);
    case 1:
      green();
    case 2:
      solid_color(69, 9, 59);
    case 3:
      solid_color(100, 100, 255);
    case 4:
      red_snake();
    case 5:
      all_on();
    case 6:
      some_solid_color(80, 20, 20, solid_idxs, (sizeof(solid_idxs) / sizeof(int)));
      // solid_color(8, 8, 15);
      // night_mode();
    case 7:
      gold_random(50);
    case 8:
      lightning(50);
    case 9:
      off();
  }

  //  if (button_was_clicked()) {
  //    rainbow(10);
  //  }
  //  pixels.clear(); //Set all pixel colors to 'off'
  // The first NeoPixel in a strand is #0, second is 1, all the way up to the count of pixels minus one.

  //  for(int i=0; i<NUMPIXELS; i++) {
  //    pixels.setPixelColor(i, pixels.Color(10, 50, 0));
  //    pixels.show(); //send the updated pixel colors to the hardware.
  //    delay(DELAYVAL); //pause before next pass through loop
  //  }
  //  for(int i=NUMPIXELS-1; i>=0; i--) {
  //    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  //    pixels.show(); //send the updated pixel colors to the hardware.
  //    delay(DELAYVAL); //pause before next pass through loop
  //  }
}