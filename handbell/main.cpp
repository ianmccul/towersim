
#include "USBJoystick.h"
#include "USBKeyboard.h"
#include "MMA8451Q.h"
#include "mbed.h"
#include <chrono>

using namespace std::chrono;

// top switch (black) is PTE30 / PTE29
// bottom switch (red) is PTE23 / PTE22
// toggle switch is PTE3 (left), PTE4 (centre), PTE5 (right)

DigitalOut bSwitch(PTE30);
DigitalOut rSwitch(PTE23);
DigitalOut tSwitch(PTE4);

DigitalIn BlackSw(PTE29, PullUp);
DigitalIn RedSw(PTE22, PullUp);

DigitalIn LeftToggle(PTE3, PullUp);
DigitalIn RightToggle(PTE5, PullUp);

#define MMA8451_I2C_ADDRESS (0x1d<<1)

void SetupAccelerometer(MMA8451Q& acc)
{
   acc.reset();
   acc.set_range(2);
   acc.set_low_noise_mode(true);
   acc.set_sampling_rate(MMA8451Q::RATE_50);
//   acc.set_sampling_rate(MMA8451Q::RATE_100);
   //   acc.set_oversampling_mode(MMA8451Q::OSMODE_HighResolution);
   acc.set_oversampling_mode(MMA8451Q::OSMODE_Normal);
   acc.set_int_data_ready_pin(2);
   acc.set_int_polarity(true);
   acc.set_int_data_ready(true);
   acc.set_active(true);
   //acc.set_active(false);
   if (acc.OK())
   {
      printf("Accelerometer initialized.\r\n");
   }
   else
   {
//      MyLed->magenta();
      printf("Accelerometer initialization failed!\r\n");
   }
}



USBJoystick* J = NULL;
USBJoystick& joystick = *J;
USBKeyboard keyboard;

enum class StateEnum { None, LeftKeyboard, RightKeyboard, Joy };

StateEnum CurrentState = StateEnum::None; // the last detected physical position of the switch

int const ToggleDebounce_ms = 100;

float const HandstrokeThreshold = 700;
float const BackstrokeThreshold = -550;
bool AtHand = false; // position of the bell - resting position is after a backstroke
int QueuedBells = 0;  // number of times to ring the bell.  We need this to take into account the debounce limit
int const BellRangDebounceTime_ms = 300;
Timer BellRangDebouncer;

void TrySendBellRang()
{
   if (QueuedBells > 0 && BellRangDebouncer.read_ms() > BellRangDebounceTime_ms)
   {
      if (CurrentState == StateEnum::LeftKeyboard)
         keyboard.putc('f');
      else if (CurrentState == StateEnum::RightKeyboard)
         keyboard.putc('j');
      BellRangDebouncer.reset();
      QueuedBells = (--QueuedBells) % 2;  // if we have lots of pending sounds, remove all but one of them

   }
}

void RedButton()
{
   if (CurrentState == StateEnum::Joy)
   {
      joystick.buttons(0x01);
   }
   else if (CurrentState == StateEnum::LeftKeyboard)
   {
      keyboard.putc('t');
   }
   else if (CurrentState == StateEnum::RightKeyboard)
   {
      keyboard.putc('n');
   }
}

void BlackButton()
{
   if (CurrentState == StateEnum::Joy)
   {
      joystick.buttons(0x02);
   }
   else if (CurrentState == StateEnum::LeftKeyboard)
   {
      keyboard.putc('l');
   }
   else if (CurrentState == StateEnum::RightKeyboard)
   {
      keyboard.putc('b');
   }
}

int const BellDebounce_ms = 25;

int main(void)
{
   bSwitch = 0;
   rSwitch = 0;
   tSwitch = 0;

   bool BlackButtonPressed = false;
   bool RedButtonPressed = false;

   Timer BlackSwDebouncer;
   BlackSwDebouncer.reset();
   BlackSwDebouncer.start();

   Timer RedSwDebouncer;
   RedSwDebouncer.reset();
   RedSwDebouncer.start();

   Timer BellDebouncer;
   BellDebouncer.reset();
   BellDebouncer.start();

   Timer SwitchDebouncer;
   SwitchDebouncer.reset();
   SwitchDebouncer.start();

   BellRangDebouncer.reset();
   BellRangDebouncer.start();

   I2C i2c0(PTE25, PTE24);
   i2c0.frequency(1000000);
   MMA8451Q acc(i2c0);

   DigitalIn AccINT1(PTA14);
   DigitalIn AccINT2(PTA15);
   SetupAccelerometer(acc);

   DigitalOut RedLed(LED_RED);
   DigitalOut BlueLed(LED_BLUE);
   DigitalOut GreenLed(LED_GREEN);
   bool gl = false;

   int8_t buttons = 0;

   while (1)
   {
      StateEnum NowState = StateEnum::None;
      if (LeftToggle.read() == 0)
         NowState = StateEnum::LeftKeyboard;
      else if (RightToggle.read() == 0)
         NowState = StateEnum::RightKeyboard;
      else
         NowState = StateEnum::Joy;

      if (NowState == StateEnum::Joy)
      {
         RedLed = 1;
         BlueLed = 0;
         GreenLed = 1;
      }
      #if 0
      if (NowState == StateEnum::LeftKeyboard)
      {
         RedLed = 0;
         BlueLed = 1;
         GreenLed = 1;
      }
      if (NowState == StateEnum::RightKeyboard)
      {
         RedLed = 1;
         BlueLed = 1;
         GreenLed = 0;
      }
#endif

      if (NowState != CurrentState)
      {
         if (SwitchDebouncer.read_ms() > ToggleDebounce_ms)
         {
            if (CurrentState == StateEnum::None)
            {
               if (NowState == StateEnum::Joy)
               {
                  RedLed = 0;
                  GreenLed = 0;
                  BlueLed = 0;
                  //joystick.connect();
               }
               else if (NowState == StateEnum::LeftKeyboard)
               {
                  RedLed = 0;
                  GreenLed = 1;
                  BlueLed = 1;
                  keyboard.connect();
               }
               else if (NowState == StateEnum::RightKeyboard)
               {
                  RedLed = 1;
                  GreenLed = 0;
                  BlueLed = 1;
                  keyboard.connect();
               }
            }
            else if (CurrentState == StateEnum::Joy)
            {
               RedLed = 1;
               GreenLed = 1;
               BlueLed = 1;
               joystick.disconnect();
               //keyboard.connect();
            }
            else if (NowState == StateEnum::Joy)
            {
               keyboard.disconnect();
               joystick.connect();
            }
            CurrentState = NowState;
         }
      }
      else
      {
         SwitchDebouncer.reset();
      }

      if (CurrentState == StateEnum::None)
         continue;

      if (AccINT2)
      {
         MMA8451Q::vector Data;
         acc.readAll(Data);

         // the y-axis is the main axis for motion control
         int16_t x = 0;
         int16_t y = 0;
         int16_t z = Data[1];

         float Joy = z * (1000.0 / 16384.0);

         if (CurrentState == StateEnum::Joy)
         {
            joystick.update(x, y, z, buttons);
         }
         else
         {
            if (AtHand && Joy < BackstrokeThreshold && BellDebouncer.read_ms() > BellDebounce_ms)
            {
               ++QueuedBells;
               BellDebouncer.reset();
               AtHand = false;
            }
            else if (!AtHand && Joy > HandstrokeThreshold && BellDebouncer.read_ms() > BellDebounce_ms)
            {
               ++QueuedBells;
               BellDebouncer.reset();
               AtHand = true;
            }
         }
      }

      if (QueuedBells > 0)
         TrySendBellRang();

      if (BlackSw.read() == 0)
      {
         //BlueLed = 0;
         //fffffBlackButton();
         if (!BlackButtonPressed && BlackSwDebouncer.read_ms() > ToggleDebounce_ms)
         {
            BlackButtonPressed = true;
            BlackButton();
            BlackSwDebouncer.reset();
            //GreenLed = gl;
            //gl = !gl;
         }
      }
      else
      {
         //GreenLed = 1;
         BlackButtonPressed = false;
      }
      if (RedSw.read() == 0)
      {
         if (!RedButtonPressed && RedSwDebouncer.read_ms() > ToggleDebounce_ms)
         {
            RedButtonPressed = true;
            RedButton();
            RedSwDebouncer.reset();
         }
      }
      else
      {
         RedButtonPressed = false;
      }
   }
}
