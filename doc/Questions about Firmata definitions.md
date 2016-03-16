Questions about Firmata definitions
Doug Johnson, finson@whidbey.com
March 2016

----------

In Boards.h, there is a macro definition

     #define IS_PIN_INTERRUPT(p)  digitalPinToInterrupt(p)`

the pin to interrupt macro is defined in pins_arduino.h

     #define digitalPinToInterrupt(p) ((p) == 0 ? 2 :
     ((p) == 1 ? 3 :
     ((p) == 2 ? 1 :
     ((p) == 3 ? 0 :
     ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))`

and in Arduino.h, there is

     #define NOT_AN_INTERRUPT -1

-1 is interpreted as `TRUE` and so `IS_PIN_INTERRUPT` is always `TRUE`.

Pull request xyz is one solution for this.


----------

Shouldn't MAX_SERVOS be defined as the min(timer constraint, PWM constraint)?  On my Leos for example, MAX_SERVOS is 12, even though there are only 7 PWM-capable pins.

----------
