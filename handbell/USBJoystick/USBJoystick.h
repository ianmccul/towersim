/* mbed USBJoystick Library
 * Copyright (c) 2012, v01:  Initial version, WH,
 *                           Modified USBMouse code ARM Limited.
 *                           (c) 2010-2011 mbed.org, MIT License
 *               2016, v02:  Updated USBDevice Lib, Added waitForConnect, Updated 32 bits button
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, inclumosig without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUmosiG BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef USBJOYSTICK_H
#define USBJOYSTICK_H

#include "USBHID.h"

class USBJoystick: public USBHID {
   public:

   /**
     *   Constructor
     *
     * @param vendor_id Your vendor_id (default: 0x1234)
     * @param product_id Your product_id (default: 0x0002)
     * @param product_release Your product_release (default: 0x0001)
     */
//     USBJoystick(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0100, uint16_t product_release = 0x0001, int waitForConnect = true):    // 4 buttons, no padding on buttons
//     USBJoystick(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0500, uint16_t product_release = 0x0001, int waitForConnect = true):    // 8 buttons, no padding on buttons
     USBJoystick(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0600, uint16_t product_release = 0x0001, int waitForConnect = true):    // 32 buttons, no padding on buttons
       USBHID(0, 0, vendor_id, product_id, product_release, false) {
         _init();
         //connect(waitForConnect);
     };

     /**
       * Write state of the joystick
       *
       * @param x x-axis position
       * @param y y-axis position
       * @param z z-axis position
       * @param buttons buttons state
       * @returns true if there is no error, false otherwise
       */
     bool update(int16_t x, int16_t y, int16_t z, uint8_t buttons);

     /**
       * Write state of the joystick
       *
       * @returns true if there is no error, false otherwise
       */
     bool update();

     /**
       * Move the cursor to (x, y, z)
       *
       * @param x-axis position
       * @param y-axis position
       * @param z-axis position
       * @returns true if there is no error, false otherwise
       */
     bool move(int16_t x, int16_t y, int16_t z);

     /**
       * Press one or several buttons
       *
       * @param buttons buttons state
       * @returns true if there is no error, false otherwise
       */
     bool buttons(uint8_t buttons);

     /**
       * To define the report descriptor. Warning: this method has to store the length of the report descriptor in reportLength.
       *
       * @returns pointer to the report descriptor
       */
       virtual uint8_t * reportDesc();

   private:
     int16_t _x;
     int16_t _y;
     int16_t _z;
     uint8_t _buttons;

     void _init();
};

#endif
