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


#include "stdint.h"
#include "USBJoystick.h"

bool USBJoystick::update(int16_t x, int16_t y, int16_t z, uint8_t buttons) {

   _x = x;
   _y = y;
   _z = z;
   _buttons = buttons;

   return update();
}

bool USBJoystick::update() {
   HID_REPORT report;

   // Fill the report according to the Joystick Descriptor
   report.data[0] = _x & 0xff;
   report.data[1] = (_x >> 8) & 0xff;
   report.data[2] = _y & 0xff;
   report.data[3] = (_y >> 8) & 0xff;
   report.data[4] = _z & 0xff;
   report.data[5] = (_z >> 8) & 0xff;
   //Use 6 bit padding for buttons
   report.data[6] = (_buttons & 0x03) ;
   report.length = 7;

   return send(&report);
}

bool USBJoystick::move(int16_t x, int16_t y, int16_t z) {
   _x = x;
   _y = y;
   _z = z;
   return update();
}

bool USBJoystick::buttons(uint8_t buttons) {
   _buttons = buttons;
   return update();
}

void USBJoystick::_init() {
   _x = 0;
   _y = 0;
   _z = 0;
   _buttons = 0x00;
}


uint8_t * USBJoystick::reportDesc()
{
   static uint8_t reportDescriptor[] =
   {
      // value in () is the number of bytes.  These bytes follow the comma, least significant byte first
      // see USBHID_Types.h for more info
      USAGE_PAGE(1), 0x01,           // Generic Desktop
      LOGICAL_MINIMUM(1), 0x00,      // Logical_Minimum (0)
      USAGE(1), 0x04,                // Usage (Joystick)
      COLLECTION(1), 0x01,           // Application
         // 3-axis joystick
         USAGE_PAGE(1), 0x01,            // Generic Desktop
         USAGE(1), 0x01,                 // Usage (Pointer)
         COLLECTION(1), 0x00,            // Physical
            USAGE(1), 0x30,                 // X
            USAGE(1), 0x31,                 // Y
            USAGE(1), 0x32,                 // Z
            LOGICAL_MINIMUM(2), 0x00, 0x80, // -32768 (using 2's complement)
            LOGICAL_MAXIMUM(2), 0xff, 0x7f, // 32767 (0x7fff, least significant byte first)
            REPORT_SIZE(1), 0x10,           // REPORT_SIZE describes the number of bits in this element (16, in this case)
            REPORT_COUNT(1), 0x03,          // number of elements
            INPUT(1), 0x02,                 // Data, Variable, Absolute
         END_COLLECTION(0),
         // 2 buttons
         USAGE_PAGE(1), 0x09,            // Buttons
         USAGE_MINIMUM(1), 0x01,         // Buttons 1 ..
         USAGE_MAXIMUM(1), 0x02,         //           .. 2
         LOGICAL_MINIMUM(1), 0x00,       // Button can be 0 or 1
         LOGICAL_MAXIMUM(1), 0x01,       //
         REPORT_SIZE(1), 0x01,           // 1 bit per element
         REPORT_COUNT(1), 0x02,          // 2 elements
         UNIT_EXPONENT(1), 0x00,         // Unit_Exponent (0)
         UNIT(1), 0x00,                  // Unit (None)
         INPUT(1), 0x02,                 // Data, Variable, Absolute
         // Padding 6 bits
         REPORT_SIZE(1), 0x01,
         REPORT_COUNT(1), 0x06,
         INPUT(1), 0x01,                 // Constant
      END_COLLECTION(0)
   };

   reportLength = sizeof(reportDescriptor);
   return reportDescriptor;
}
