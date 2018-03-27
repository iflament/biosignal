
## Instructables Brainwave hat software:

#### Link to tutorial: http://www.instructables.com/id/Build-an-EEG-hat-that-turns-your-brainwaves-into-l/#step8

You can upload the software into your hat either before constructing the LED structure that contains the Arduino, or afterwards when everything is finished. As you prefer.

The Arduino code converts the data string being received from the Neurosky chip into light of different colors, brightness and intensity: all of which are programmed to change according to your mental state. The pompom color is always be a combination of the real-time percentages of "attention" and "relaxation". For example, if a user has chosen yellow as his "relaxation color" and blue as the "attention" color, then the pompom color will be a mix of yellow and blue (100% attention = 100% blue, 50% attention, 50% relaxation = 50% blue 50% yellow, etc). If you acheive a high level of relaxation or attention over time, then the pompom will shine in "attention acheived color" or "relaxation acheived color". These colors are the same as the "attention color" and "relaxation color", only brighter (one Neopixel is white).

The color and brightness settings or "modes", can be controlled using the "Mode switch" (the black pushbutton switch on the hat). This switch allows you to select a specific color to represent the level of "attention". The "relaxation" color is automatically chosen for you by the software, as an opposite color of the "attention" color. For example, if you choose green for the attention color, the software will assign purple as your relaxation color. You can also adjust the maximum brightness of the Neopixels, check the signal quality, or choose to use the pompom as a beacon light without the EEG. Although I have not done it, you can write additional code that allows you to visualize a color that represents a mix of, or dominant frequency band power (alpha, beta, delta, theta, gamma).

The software is in the attached file illumino_updatedOct_2016.zip. Awell.ino is the main Arduino script. All code is commented to be self explanatory. Please don't hesitate to ask if you have any questions. A special thanks to Kurt Olsen for his dedication in assisting with the firmware!

- Mount the FTDI USB onto your Arduino.
- Plug the Arduino into your computer. Always be very careful doing so, as the mini USB component is delicate.
- Install the Arduino IDE 1.0.6 (http://arduino.cc/en/main/software) and appropriate drivers on your computer. Great websites for learning how to program an Arduino: arduino.cc, electronhacks @ youtube, adafruit.
- Select the right board and serial port (COM). This is usually the lowest number.
- Press the "verify" button.
- Press the "upload" button.
- Now, open the serial port. There should be a stream of numbers flowing down fast. This is the brain data incoming from the Neurosky chip.
