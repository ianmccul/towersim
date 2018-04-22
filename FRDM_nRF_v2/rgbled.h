// -*- C++ -*-
// FRDL-KL25z LED controller functions

// The LED operates in an active-low sense, 1.0 = off, 0.0 = on full.
// This makes it a bit more complicated.  So better to use
// the color control functions here.
//
// The datasheet is rather vague on the relative intensities of the colours.
// Red is   450 - 900 mcd
// Green is 710 - 1400 mcd
// Blue is  224 - 450 mcd

// Taking the middle values,
// Red   675
// Green 1055
// Blue 337

// Scaling by the maximum blue intensity, we should multiply
// red by 0.5, and green by 0.32.
// However in the sample I tested, to get approximately balanced
// colors the scaling was completely different, 0.1 for red, and 1.0 for green and blue.


#if !defined(RGBLED_H)
#define RGBLED_H

#include "mbed.h"
#include <cmath>

constexpr float RedScale   = 0.1;
constexpr float GreenScale = 1.0;
constexpr float BlueScale  = 1.0;

class RGBLED
{
   public:
      RGBLED(PinName RedPin, PinName GreenPin, PinName BluePin)
         : rled(RedPin), gled(GreenPin), bled(BluePin)
      {
         rled.period(0.0005);
         gled.period(0.0005);
         bled.period(0.0005);
      }

      void write(float red, float green, float blue);

      // functions to set various colours
      void off()
      {
         this->write(0, 0, 0);
      }

      void red(float intensity = 1.0)
      {
         this->write(intensity, 0, 0);
      }

      void green(float intensity = 1.0)
      {
         this->write(0, intensity, 0);
      }

      void blue(float intensity = 1.0)
      {
         this->write(0, 0, intensity);
      }

      void cyan(float intensity = 1.0)
      {
         this->write(0, intensity, intensity);
      }

      void yellow(float intensity = 1.0)
      {
         this->write(intensity, intensity, 0);
      }

      void magenta(float intensity = 1.0)
      {
         this->write(intensity, 0, intensity);
      }

      void white(float intensity = 1.0)
      {
         this->write(intensity, intensity, intensity);
      }

      // Write value in H,S,V color space.  Hue is 0..1
      void write_hsv(float hue, float saturation, float value);

   private:
      PwmOut rled;
      PwmOut gled;
      PwmOut bled;
};

inline
void
RGBLED::write(float red, float green, float blue)
{
   rled = 1.0 - RedScale*red;
   gled = 1.0 - GreenScale*green;
   bled = 1.0 - BlueScale*blue;
}

inline
void
RGBLED::write_hsv(float hue, float saturation, float value)
{
   float h = hue * 6;
   float c = value * saturation;
   float v = value;
   float x = c * (1.0 - std::abs(std::fmod(h, 2.0) - 1.0));
   if (h <= 1)
      this->write(v    , x+v-c, v-c  );
   else if (h <= 2)
      this->write(x+v-c, v    , v-c  );
   else if (h <= 3)
      this->write(v-c  , v    , x+v-c);
   else if (h <= 4)
      this->write(v-c  , x+v-c, v    );
   else if (h <= 5)
      this->write(x+v-c, v-c  , v    );
   else
      this->write(v    , v-c  , x+v-c);
}

#endif
