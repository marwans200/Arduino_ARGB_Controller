# Arduino_ARGB_Controller
An LED controller I made for an ARGB Case Fan

# Important!!
it is important to close all application using the port of the Arduino or close the port so the application can work properly.

# How to Use
Upload the code to your arduino and also connect the neccesary pins (I also added an LCD display for fun).
You can now use it using serial communication through the arduino IDE but instead of doing that, you can just download the zip file and run the application and select the desired arduino through the COM selector, select the preset or use one of the available custom preset and change the values. You can also set the speeds of the different modes and finally, click apply(In the application).

If you are not keen on using applications from a random stranger online, there are a list of commands you can use to set values, change preset etc.
They are:
O (Case Insensitive): RGB Rainbow Spin
N (Case Insensitive): RGB Rainbow Fade(to Different Colors)
B (Case Insensitive): Backlight of the LCD
s (Case Sensitive)  : Speed of the Rainbow Spin eg:- s1, s15, s12
c (Case Sensitive)  : Sets the mode to Color Sequence mode and colors must be selected. Color selection eg:- cr,b Sets the mode to Color Sequence and sets the two colors to Red and Blue. 
Only a few colours are set by me.

R (Case Sensitive)  : Spin with one color fading to the last led, like a loading icon. Also only set colors. Use eg:- Rr (Sets to red color), Rb (Sets to blue color), Rg (Sets to green color)
q (Case Sensitive)  : Sets the Color Sequence Speed eg:- q1, q65, q75 (q40 is the most optimum value in my opinion)
r (Case Sensitive)  : Sets the Speed of the Spin Types eg:- r1, r25, r15
Y (Case Sensitive)  : Sets the mode to Yin Yang Mode. takes two RGB values and makes them spin opposite to each other like the Yin Yang. eg:- Y255,0,0,0,0,255 Sets it to Red and Blue. YR1,G1,B1,R2,B2,G2
N (Case Insensitive): Changes the preset values. eg:- N0 sets it to custom, N1 Sets it to RGB Spin.
L (Case Sensitive)  : Sets the LCD message. Maximum 32 Characters Allowed. 16 per line as im using the 1602 LCD. eg:- LHello World! sets it to "Hello World!" without the quotes of course.
~ (Case Sensitive)  : Sets the mode to Single LED Spin and a custom RGB color, which is just one LED spinning around the strip. eg:- ~255,0,0 Sets it to Red, ~0,0,255 Sets it to Blue.

Presets are:-

Custom Mode

rainbowWithOffset(ledSpeed); //Rainbow Spin

rainbowNoOffset(ledSpeed); //Rainbow Color fade

displayColorSequence(colorSequence, speedCC * 10); //Custom Color Sequence (If saved)

displayColorSequence("r,b", speedCC * 10); //Red and Blue (Police Siren) Color Sequence or flash

rotateWithShadow(speedR, 'r'); //Spin one color with trail in Red

rotateWithShadow(speedR, 'g'); //Spin one color with trail in Green

rotateWithShadow(speedR, 'b'); //Spin one color with trail in Blue
 
rotateWithShadow(speedR, 'c'); //Spin one color with trail in Cyan

rotateWithShadow(speedR, 'f'); //Spin one color with trail in White

yinYang(speedR,yingYangColors[0],yingYangColors[1],yingYangColors[2],yingYangColors[3],yingYangColors[4],yingYangColors[5]); //Custom Yin Yang Values if saved or set

yinYang(speedR,255,0,0,0,0,255); //Red and Blue Yin Yang

rotateWithShadow(speedR, 'r'); //RGB color changing spin
rotateWithShadow(speedR, 'g'); //This is a part of it
rotateWithShadow(speedR, 'b'); //This too

SingleLED(speedR, strip.Color(255,0,0)); //Single LED spin Red

SingleLED(speedR, strip.Color(0,255,0)); //Single LED spin Green

SingleLED(speedR, strip.Color(0,0,255)); //Single LED spin Blue
