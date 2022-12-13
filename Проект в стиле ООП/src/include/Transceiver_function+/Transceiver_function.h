#ifndef __MY_TRANSCEIVER_H_INCLUDED
#define __MY_TRANSCEIVER_H_INCLUDED



#define		PIN_MODE_OUT(pin)			pinMode(pin, OUTPUT)
#define		PIN_MODE_IN(pin)			pinMode(pin, INPUT)
#define		PIN_MODE_IN_PULLUP(pin)		pinMode(pin, INPUT_PULLUP)
#define		PIN_MODE_IN_PULLDOWN(pin)	


#define		PIN_SET_HIGH(pin)			digitalWrite(pin, 1)
#define		PIN_SET_LOW(pin)			digitalWrite(pin, 0)


#define		ANALOG_LEVEL_MAX				255
#define		PIN_SET_ANALOG(pin, percent)	analogWrite(pin, (int)(ANALOG_LEVEL_MAX * (percent) / 100))


#define		PIN_READ_ANALOG(pin)		analogRead(pin)
#define		PIN_READ_DIGITAL(pin)		digitalRead(pin)

#define		INTERRUPTS_ON				sei()
#define		INTERRUPTS_OFF				cli()


#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif
#define abs(x) ((x)>0?(x):-(x))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#endif