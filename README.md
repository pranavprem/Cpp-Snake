# Nokia-Snake-with-graphics.h
Computer Graphics project for Pranav Prem during 3rd year engineering at Goa College of Engineering

#Objective
To recreate the magic of one of the first mobile games using graphics.h
Also score some marks for the Graphics practicals.

#How It's Done
The screen is divided logically into gridmax columns and rows of size div (both in the #define to quickly change size of screen).
Each function accepts the x-y coordinates of a square on the grid and then paints something specific within the square.
Each piece of the snake is treated individually and painted in. The head is painted in a different fashion to show the direction of motion of the snake
The code accepts user input to dictate direction of snake. 

#How to play
use w-a-s-d controls to move the snake around. Eating leaves (vegan snake) makes the snake grow longer and move faster also adds to your score.
If the snake reaches a decent size, you win. If you eat yourself or crash into the boundaries, you lose.
