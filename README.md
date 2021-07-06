# Breakout-Ai
A simple Ai created with C++ and OpenCV for Breakout

![Screenshot](https://github.com/BrandonBahret/Breakout-AI/blob/master/project-img.gif)

For a longer preview you can watch this [video](https://www.youtube.com/watch?v=K8Ctrg2LeRE)

Usage:

 1. open the Atari 2600 emulator Stella
 2. load a legal copy of Breakout
 3. Tab out of the emulator
 4. Start the Ai
 5. Focus Stella by clicking the window
 6. Press f2, and then space, to start the game.
 watch the magic.
 
Disclaimer: The ai isn't close to perfect, and results may vary.

## Brief explanation
I'm using the Windows API to read the game display. The gamestate is parsed from the resulting image stream with OpenCV, which is then used to drive the ai. I extracted the position of the ball from which I could calculate its trajectory. From there, I estimate where the ball will be when it approaches the paddle. With this information I generate the necessary inputs to position the paddle to meet with the falling ball.
