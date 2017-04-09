/*
 Name:		Arduino-Night-Light.ino
 Created:	4/8/2017
 Author:	Robin Thomas
*/

// Define inputs
const int BrightnessPotPin = 0;		// Analog input pin
const int ColorPotPin = 1;			// Analog input pin

// Define outputs
const int OnBoardLEDPin = 13;
const int PushButtonPin = 6;

// Declare variables for sleep timer
int runTimeLength = 10;								// In minutes
long startMillis = 0;         						// store the start time of the sketch
long runTimeInMillis = 1000 * 60 * runTimeLength;	// how long to run before going to sleep

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

#define NUMBEROFPIXELS 12   //Number of LEDs on the strip
#define PIXELPIN 8          //Pin where WS281X pixels are connected

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <NeoPixelPainter.h>

Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(NUMBEROFPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);
NeoPixelPainterCanvas pixelcanvas = NeoPixelPainterCanvas(&neopixels); //create canvas, linked to the neopixels (must be created before the brush)
NeoPixelPainterBrush pixelbrush = NeoPixelPainterBrush(&pixelcanvas); //crete brush, linked to the canvas to paint to

int rgbValues[] = {0,0,0};

void setup() {
	//digitalWrite(OnBoardLEDPin, LOW);

	//Setup GPIO pins
	pinMode(OnBoardLEDPin, OUTPUT);
	pinMode(PushButtonPin, INPUT);
	
	//Turn on Push button
	digitalWrite(PushButtonPin, LOW);
	
	//Initialize NeoPixels
	randomSeed(analogRead(0)); //new random seed 
	pinMode(PIXELPIN, OUTPUT);
	neoPixelStrip.begin();
}

// the loop function runs over and over again until power down or reset
void loop() {


	// First check to see if the button was pressed
	unsigned long currentMillis = millis();
	button1State = digitalRead(button1Pin);

	// If button was pressed, reset sleep timer
	if (button1State == LOW)
	{
		startMillis = currentMillis;
	}

	if (currentMillis - startMillis < sleepTimeDelay)
	{
		// Read the voltage from the pots (Range: 0-1023)
		int brightnessValue = analogRead(BrightnessPotPin);
		int colorValue = analogRead(ColorPotPin);

		double percentBrightness = MapAnalogValuesToPercent(brightnessValue)
		uint32_t rgbColor = SetRGBFromAnalogValues(colorValue);
		AnimateNeoPixels();
	}
	else
	{
		//Turn off all neopixels
		for (int i = 0; i < NeoPixelCount; i++)
		{ 
			neoPixelStrip.setPixelColor(i, 0, 0,0);
		}
	}
}


double MapAnalogValuesToPercent(int value)
{
	// Turn the pot reading into a brightness percentage
	double britghtnessPercent = map(BrightnessValue, 0, 1023, 1023, 0);
	britghtnessPercent = map(britghtnessPercent, 0, 1023, 0, 100);
	britghtnessPercent = britghtnessPercent / 100;
	return britghtnessPercent;
}

uint32_t SetRGBFromAnalogValues(int value)
{
	int mapRGB1, mapRGB2, constrained1, constrained2, redValue, greenValue, blueValue;
	
	// Set a RGB LED to a position on the "rainbow" of all colors.
	// RGBposition should be in the range of 0 to 1023 (such as
	// from an analog input).

	// Take ColorValue and turn it into three RGB values.
	// The three values are computed so that the colors mix and 
	// produce a rainbow of colors across the 0-1023 input range.

	// For each channel (red green blue), we're creating a "peak"
	// a third of the way along the 0-1023 range. By overlapping
	// these peaks with each other, the colors are mixed together.
	// This is most easily shown with a diagram:
	// http://sfecdn.s3.amazonaws.com/education/SIK/SchematicImages/Misc/RGB_function.jpg


	// RED - Create the red peak, which is centered at 0.
	// (Because it's centered at 0, half is after 0, and half is before 1023):
	mapRGB1 = map(value, 0, 341, 255, 0);
	constrained1 = constrain(mapRGB1, 0, 255);

	mapRGB2 = map(value, 682, 1023, 0, 255);
	constrained2 = constrain(mapRGB2, 0, 255);

	redValue = constrained1 + constrained2;

	// GREEN - Create the green peak, which is centered at 341, one-third of the way to 1023
	greenValue = constrain(map(value, 0, 341, 0, 255), 0, 255) - constrain(map(value, 341, 682, 0, 255), 0, 255);

	// BLUE - Create the blue peak, which is centered at 682, two-thirds of the way to 1023
	blueValue = constrain(map(value, 341, 682, 0, 255), 0, 255) - constrain(map(value, 682, 1023, 0, 255), 0, 255);

	return strip.Color(redValue, greenValue, blueValue);
	
}

void AnimateNeoPixels(int int brightnessValue)
{
 	//------------------------------
	//HUE FADER: demo of hue fading
  	//------------------------------

	//hue fading can be done in two ways: change the color moving the shortest distance around the colorwheel (setFadeHueNear)
	//or intentionally moving around the colorwheel choosing the long way (setFadeHueFar)
	//two brushes move along the strip in different speeds, each painting a different color that the canvas will then fade to
	//a new color is set when the first brush passes pixel 0
	//both brushes act on the same canvas

	while(true) //create a loop with an additional brush (is deleted automatically once the loop finishes)
	{
		//create an additional brush, painting on the same canvas as the globally defined brush
		NeoPixelPainterBrush pixelbrush2 = NeoPixelPainterBrush(&pixelcanvas); 

		pixelcanvas.clear(); //clear the canvas

		for(loopcounter = 0; loopcounter<duration; loopcounter++) 
		{
			static unsigned int lastposition = 0; //to detect zero crossing only once (brush may stay at pixel zero for some time since it uses sub-pixel resolution movement)

			if (pixelbrush.getPosition() == 0 && lastposition > 0) initialized = false; //choose new color & speed if brush reaches pixel 0

			lastposition = pixelbrush.getPosition(); //save current position for next position check

			if (initialized == false)
			{
				initialized = true;

				HSV brushcolor;

				brushcolor.h = random(255); 	//random color
				brushcolor.s = 255; 			//full saturation
				brushcolor.v = 130; 			//medium brightness

				pixelbrush.setSpeed(random(150) + 150); //random movement speed
				pixelbrush.setFadeSpeed(random(10) + 20); //set random fading speed
				pixelbrush.setColor(brushcolor); //update the color of the brush
				pixelbrush.setFadeHueNear(true); //fade using the near path on the colorcircle
				
				//second brush paints on the same canvas
				brushcolor.h = random(255);
				pixelbrush2.setSpeed(random(150) + 150);
				pixelbrush2.setFadeSpeed(random(10) + 20);
				pixelbrush2.setColor(brushcolor);
				pixelbrush2.setFadeHueNear(true); //fade using the near path on the colorcircle
				//pixelbrush.setFadeHueFar(true); //fade using the far path on the colorcircle (if both are set, this path is chosen)
				pixelbrush2.setBounce(true); //bounce this brush at the end of the strip
			}

			neopixels.clear();

			pixelbrush.paint(); //apply the paint of the first brush to the canvas (and update the brush)
			pixelbrush2.paint(); //apply the paint of the second brush to the canvas (and update the brush)
			pixelcanvas.transfer(); //transfer the canvas to the neopixels

			neopixels.show();
		}
		break; //quit the while loop immediately (and delete the created brush)
	}

	initialized = false; //reset the variable before moving to the next loop

}