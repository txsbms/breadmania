# breadmania
Hit as many notes as you can as they fall down the screen!

School project for CS/EE120B at UCR. 

This project was designed around an ATmega328P microcontroller on a breadboard. The code provided will interface with a joystick, an HD44780 LCD screen, shift registers (74HC595) which control an 8x8 LED matrix, and seven buttons. The LCD screen outputs custom characters using CGROM on the HD44780, and the project keeps score saved in memory even after a device reboot by utilizing EEPROM functionality on the ATmega1284P.

All code outside of main.c was provided for the purpose of completing this project.

A demo showcasing the project (including rules and objectives) can be found at: https://www.youtube.com/watch?v=3I2peg5MazA
