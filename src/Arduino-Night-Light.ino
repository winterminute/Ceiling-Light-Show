/*
 Name:		Arduino-Night-Light.ino
 Created:	4/8/2017
 Author:	Robin Thomas
*/

// Define inputs
const int LeftHandPotPin = 0;			// Analog input pin
const int RightHandPotPin = 1;			// Analog input pin

// Define outputs
const int OnBoardLEDPin = 13;
const int PushButtonPin = 6;

// Declare variables for sleep timer
int runtimeLength = 10;										// In minutes
unsigned long startMillis = 0;         						// store the start time of the sketch
unsigned long runTimeInMillis = 1000 * 60 * runtimeLength;	// how long to run before going to sleep

// #######################################
// 		Setup NeoPixels
//#######################################
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

const int NeoPixelPin = 8;
const int NeoPixelCount = 12;

//#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <NeoPixelPainter.h>

Adafruit_NeoPixel neoPixelStrip = Adafruit_NeoPixel(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ800);
NeoPixelPainterCanvas pixelcanvas = NeoPixelPainterCanvas(&neoPixelStrip); //create canvas, linked to the neopixels (must be created before the brush)
NeoPixelPainterBrush pixelbrush = NeoPixelPainterBrush(&pixelcanvas); //crete brush, linked to the canvas to paint to
NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas); 
bool animationIntialized = false;
unsigned long loopcounter;			//count the loops, switch to next animation after a while
const int duration = 500;  //number of loops to run each animation for

void setup() {
	//digitalWrite(OnBoardLEDPin, LOW);

	//Setup GPIO pins
	pinMode(OnBoardLEDPin, OUTPUT);
	pinMode(PushButtonPin, INPUT);
	
	//Turn on Push button
	//digitalWrite(PushButtonPin, LOW);
	
	//Initialize NeoPixels
	pinMode(NeoPixelPin, OUTPUT);
	neoPixelStrip.begin();
}

// the loop function runs over and over again until power down or reset
void loop() {



	// First check to see if the button was pressed
	unsigned long currentMillis = millis();

	// If button was pressed, reset sleep timer
	//if (digitalRead(PushButtonPin) == LOW)
	//{
	//	startMillis = currentMillis;
	//}

	//Time has expired, shut everything down
	//if (currentMillis - startMillis > runTimeInMillis)
	//{
	//	//Turn off all neopixels
	//	for (int i = 0; i < NeoPixelCount; i++)
	//	{ 
	//		neoPixelStrip.setPixelColor(i, 0, 0,0);
	//	}
	//	neoPixelStrip.clear();
	//	animationIntialized == false;
	//}
	//else
	//{
		// Do Fun Stuff !!
		// Read the voltage from the pots (Range: 0-1023)
		int britghtnessValue = analogRead(LeftHandPotPin);
		britghtnessValue = map(britghtnessValue, 1023, 0, 0, 1023);
		britghtnessValue = map(britghtnessValue, 0, 1023, 0, 255);
		//double percentBrightness = britghtnessValue / 100;

		int animationSpeed = analogRead(RightHandPotPin);
		animationSpeed = map(animationSpeed, 1023, 0, 0, 1023);
		animationSpeed = map(animationSpeed, 0, 1023, 1, 100);
		
		//Setup animation sequence
		if (animationIntialized == false)
		{
			InitializeNeoPixelPainter(britghtnessValue, animationSpeed);
		}
		AnimateNeoPixels();
	//}
}

void InitializeNeoPixelPainter(int brightness, int speed)
{
	//brightness = 255 * brightness;// * brightness;

	HSV brushcolor;
   	brushcolor.s = 255;						//255 == full saturation
	brushcolor.v = brightness;

	brushcolor.h = random(255);				//random color
	pixelbrush.setSpeed(1000);				//random movement speed
	pixelbrush.setFadeSpeed(speed);				//set random fading speed
	pixelbrush.setColor(brushcolor);		//update the color of the brush
	pixelbrush.setFadeHueNear(true);		//fade using the near path on the colorcircle
				
	//second brush paints on the same canvas
	brushcolor.h = random(255);
	pixelbrush2.setSpeed(1000);
	pixelbrush2.setFadeSpeed(speed);
	pixelbrush2.setColor(brushcolor);
	pixelbrush2.setFadeHueNear(true); //fade using the near path on the colorcircle
	//pixelbrush.setFadeHueFar(true); //fade using the far path on the colorcircle (if both are set, this path is chosen)
	pixelbrush2.setBounce(true); //bounce this brush at the end of the strip

	animationIntialized == true;

}
void AnimateNeoPixels()
{
	neoPixelStrip.clear();

	pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
	pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
	pixelcanvas.transfer(); //transfer the canvas to the neopixels

	neoPixelStrip.show();
}