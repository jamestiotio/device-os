/**
 ******************************************************************************
 * @file    servo.cpp
 * @authors Satish Nair
 * @version V1.0.0
 * @date    7-Oct-2014
 * @brief   SERVO test application
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "application.h"
#include "servo_hal.h"
#include "unit-test/unit-test.h"

#if HAL_PLATFORM_GEN == 3
#if PLATFORM_ID == PLATFORM_ESOMX || PLATFORM_ID == PLATFORM_P2
static const hal_pin_t pin = D1, pin2 = D8;
#elif PLATFORM_ID == PLATFORM_TRACKERM
static const hal_pin_t pin = A2, pin2 = D8;
#else
static const hal_pin_t pin = A0, pin2 = A1;
#endif
#else
#error "Unsupported platform"
#endif

test(SERVO_01_CannotAttachWhenPinSelectedIsNotTimerChannel) {
#if HAL_PLATFORM_NRF52840
# if PLATFORM_ID == PLATFORM_TRACKER
    hal_pin_t pin = BTN;
#elif PLATFORM_ID == PLATFORM_ESOMX
    hal_pin_t pin = A0;
# else
    hal_pin_t pin = D0;
# endif
#elif HAL_PLATFORM_RTL872X
    hal_pin_t pin = D5;
#else
#error "Unsupported platform"
#endif
    Servo testServo;
    // when
    assertFalse(testServo.attach(pin));
    // then
    //Servo works on fixed PWM frequency of 50Hz
    assertNotEqual(HAL_Servo_Read_Frequency(pin), SERVO_TIM_PWM_FREQ);
    //To Do : Add test for remaining pins if required
}

test(SERVO_02_CannotAttachWhenPinSelectedIsOutOfRange) {
    hal_pin_t pin = 51;//pin under test (not a valid user pin)
    Servo testServo;
    assertFalse(testServo.attach(pin));
}

test(SERVO_03_AttachedOnPinResultsInCorrectFrequency) {
    Servo testServo;
    // when
    assertTrue(testServo.attach(pin));
    // then
    //Servo works on fixed PWM frequency of 50Hz
    assertEqual(HAL_Servo_Read_Frequency(pin), SERVO_TIM_PWM_FREQ);
    //To Do : Add test for remaining pins if required
}

test(SERVO_04_WritePulseWidthOnPinResultsInCorrectMicroSeconds) {
    uint16_t pulseWidth = 1500;//value corresponding to servo's mid-point
    Servo testServo;
    // when
    assertTrue(testServo.attach(pin));
    testServo.writeMicroseconds(pulseWidth);
    uint16_t readPulseWidth = testServo.readMicroseconds();
    // then
    assertEqual(readPulseWidth, pulseWidth);
    //To Do : Add test for remaining pins if required
}

// FIXME: P2 doesn't support pulseIn()
#if !HAL_PLATFORM_RTL872X
test(SERVO_05_DetachDoesntAffectAnotherServoUsingSameTimer) {
    const int pulseWidth = 2000;
    // Attach 1st servo
    Servo servo1;
    servo1.attach(pin);
    servo1.writeMicroseconds(pulseWidth);
    // Attach 2nd servo
    Servo servo2;
    servo2.attach(pin2);
    servo2.writeMicroseconds(pulseWidth);
    // Detach 1st servo
    servo1.detach();
    // Ensure 2nd servo is not affected
    int readPulseWidth = 0;
    const uint32_t start = millis();
    for (int i = 0; i < 100; ++i) {
        readPulseWidth += pulseIn(pin2, HIGH);
        if (millis() - start > 3000) {
            break; // pulseIn() takes too long time
        }
    }
    readPulseWidth /= 100; // Average pulse width
    servo2.detach();
    assertTrue(readPulseWidth > pulseWidth - 50 && readPulseWidth < pulseWidth + 50);
}
#endif // !HAL_PLATFORM_RTL872X
