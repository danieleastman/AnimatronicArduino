# Animatronic Arduino
Code for using multiple distance sensors to track an object and have a stepper motor follow its location in 2D space. Eventually this will be used to control statues, and include audio dialog.

![](http://i.imgur.com/R5bjzA3.jpg?1)

For this project I used 2 Maxbotix Ultrasonic Rangefinder (HRLV-EZ0) sensors as they had the widest field of view, while also being highly accurate and appropriate for detecting people.

To use, all you need to do is put in the correct distance between the two sensors and the X and Y distance the motor is from the left sensor. After that, it just uses some basic trigonometry to figure out where the object is.

**ToDo:**
 - Smooth out stepper movement
 - Enable the control of multiple steppers at once
