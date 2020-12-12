

#ifndef USBJOYSTICK_H
#define USBJOYSTICK_H

#include "USBHID.h"
#include "Stream.h"

/* Modifiers, left keys then right keys. */
enum MODIFIER_KEY {
    KEY_CTRL = 0x01,
    KEY_SHIFT = 0x02,
    KEY_ALT = 0x04,
    KEY_LOGO = 0x08,
    KEY_RCTRL = 0x10,
    KEY_RSHIFT = 0x20,
    KEY_RALT = 0x40,
    KEY_RLOGO = 0x80,
};


enum MEDIA_KEY {
    KEY_NEXT_TRACK,     /*!< next Track Button */
    KEY_PREVIOUS_TRACK, /*!< Previous track Button */
    KEY_STOP,           /*!< Stop Button */
    KEY_PLAY_PAUSE,     /*!< Play/Pause Button */
    KEY_MUTE,           /*!< Mute Button */
    KEY_VOLUME_UP,      /*!< Volume Up Button */
    KEY_VOLUME_DOWN,    /*!< Volume Down Button */
};

enum FUNCTION_KEY {
    KEY_F1 = 128,   /* F1 key */
    KEY_F2,         /* F2 key */
    KEY_F3,         /* F3 key */
    KEY_F4,         /* F4 key */
    KEY_F5,         /* F5 key */
    KEY_F6,         /* F6 key */
    KEY_F7,         /* F7 key */
    KEY_F8,         /* F8 key */
    KEY_F9,         /* F9 key */
    KEY_F10,        /* F10 key */
    KEY_F11,        /* F11 key */
    KEY_F12,        /* F12 key */

    KEY_PRINT_SCREEN,   /* Print Screen key */
    KEY_SCROLL_LOCK,    /* Scroll lock */
    KEY_CAPS_LOCK,      /* caps lock */
    KEY_NUM_LOCK,       /* num lock */
    KEY_INSERT,         /* Insert key */
    KEY_HOME,           /* Home key */
    KEY_PAGE_UP,        /* Page Up key */
    KEY_PAGE_DOWN,      /* Page Down key */

    RIGHT_ARROW,        /* Right arrow */
    LEFT_ARROW,         /* Left arrow */
    DOWN_ARROW,         /* Down arrow */
    UP_ARROW,           /* Up arrow */
};

class USBKeyboardJoystick: public USBHID {
   public:
      // @param vendor_id Your vendor_id (default: 0x1234)
      // @param product_id Your product_id (default: 0x0002)
      // @param product_release Your product_release (default: 0x0001)
      USBKeyboardJoystick(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0600, uint16_t product_release = 0x0001, int waitForConnect = true):
            USBHID(0, 0, vendor_id, product_id, product_release, false)
      {
         this->init();
         //this->connect(waitForConnect);
      };

      // Write state of the joystick and update
      bool update(int16_t x, int16_t y, int16_t z, uint8_t buttons);

      // update the state of the joystick (send USB state along the wire)
      bool update();

      // move the joystick position without updating
      bool move(int16_t x, int16_t y, int16_t z);

      // press joystick buttons
      bool buttons(uint8_t buttons);

      // send a key code
      bool keyCode(uint8_t key, uint8_t modifier = 0);

      // send a character
      int putc(int c);

      // send a media control key
      bool mediaControl(MEDIA_KEY key);

      // read the status of the keyboard lock keys
      //   - First bit: NUM_LOCK
      //   - Second bit: CAPS_LOCK
      //   - Third bit: SCROLL_LOCK
      uint8_t lockStatus();

      // The report descriptor
      virtual uint8_t * reportDesc();

      // called when data is received  on the OUT endpoint.  Used to read
      // the keyboard led status
      virtual bool EPINT_OUT_callback();


   private:
      // jotsyick status
      int16_t _x;
      int16_t _y;
      int16_t _z;
      uint8_t _buttons;

      // leyboard led status
      uint8_t lock_status;

      void init();
};

#endif
