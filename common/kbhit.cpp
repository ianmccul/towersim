#include "kbhit.h"
#include <termios.h>
#include <unistd.h>   // for read()
#include <stdlib.h>

namespace
{

   struct termios initial_settings, new_settings;
   bool KeyboardInit = false;
   int peek_character = -1;

} // namespacae

void init_keyboard()
{
   tcgetattr(0,&initial_settings);
   new_settings = initial_settings;
   new_settings.c_lflag &= ~ICANON;
   new_settings.c_lflag &= ~ECHO;
   new_settings.c_lflag &= ~ISIG;
   new_settings.c_cc[VMIN] = 1;
   new_settings.c_cc[VTIME] = 0;
   tcsetattr(0, TCSANOW, &new_settings);
   KeyboardInit = true;
   atexit(close_keyboard);
}

void close_keyboard()
{
   if (KeyboardInit)
      tcsetattr(0, TCSANOW, &initial_settings);
   KeyboardInit = false;
}

bool kbhit()
{
   if (peek_character != -1)
      return true;

   new_settings.c_cc[VMIN]=0;
   tcsetattr(0, TCSANOW, &new_settings);
   char ch;
   int nread = read(0,&ch,1);
   new_settings.c_cc[VMIN]=1;
   tcsetattr(0, TCSANOW, &new_settings);
   if(nread == 1)
   {
      peek_character = ch;
      return true;
   }
   return false;
}

int readch()
{
   if(peek_character != -1)
   {
      char ch = peek_character;
      peek_character = -1;
      return ch;
   }
   char ch;
   int r = read(0,&ch,1);
   return ch;
}
