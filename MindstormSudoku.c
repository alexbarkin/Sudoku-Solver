#pragma config(Motor, motorA, centerMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorC, leftMotor, tmotorEV3_Medium, PIDControl, reversed, encoder)
#pragma config(Motor, motorD, rightMotor, tmotorEV3_Medium, PIDControl, encoder)
/*The prior three lines configure the motors to allow different names, and synchronization
of the left and right motors by reversing the left one*/

#include "EV3_FILEIO.c"
/*Used to include the fileIO library found on Learn*/

/****************************************************************************************
*****************************************************************************************
** Code by Alex Barkin, Cameron Nelson, Prabhdeep Soni, and Kunj Patel                 **
** Additional code from syb0rg                                                         **
** found at: https://codereview.stackexchange.com/questions/37430/sudoku-solver-in-c   **
**                                                                                     **
** Code below is for a Sudoku Solver, made for MTE 100 in 2017 for group 847's final   **
** project using the EV3 and Tetrix systems to read in, solve, and output an array     **
*****************************************************************************************
****************************************************************************************/

/*Used for storing the puzzle which the robot read in and solves*/
int puzzle [9][9] =
{0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0};

/*Used for storing the benchmark readings for the RGB values of the colours at the top of
the grid to be used in the sudoku grid*/
int RGBVAL [9][3] =
{0, 0, 0, //Blue
	0, 0, 0, //Green
	0, 0, 0, //Yellow
	0, 0, 0, //Orange
	0, 0, 0, //Red
	0, 0, 0, //Black
	0, 0, 0, //Purple
	0, 0, 0, //LightBlue
	0, 0, 0}; //BabyPink

/*Used for the tolerance allowed for the RG&B values of each colour reading chosen
by testing starting at 5. We found at 9 it started to get confused with other colours,
and at 5, it missed some of the colours*/
const int COLOUR_TOL = 8;
/*Used for encoder limit for moving one square in the Y direction, 2 is the number of
cm between squares, and 1.88 is the radius of the gears in the Y-Plane*/
const int Y_ENC_LIMIT = 2.0 * 360 / (2 * PI * 1.88);
/*Used for encoder limit for moving one square in the X direction, 2 is the number of
cm between squares, and 1.76 is the radius of the gears in the X-Plane*/
const int X_ENC_LIMIT =  2.0 * 360 / (2 * PI * 1.76);
/*Used as number of encoder counts for each motor to spin before the other motor is used
to counteract the natural offset of the motors chosen by the group, we started higher,
but realised that the lower we went the more precise it runs*/
const int MOTOR_PREC = 2;

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*File to output the name of our robot as well as an image to be visually pleasing       *
****************************************************************************************/
void openingDisplay()
{
	drawBmpfile(0,100,"Forest");
	displayBigStringAt(50,105, "Sudoku");
	displayBigStringAt(80,85, "Solver");
}

/****************************************************************************************
*Written By syb0rg edited by Alex Barkin                                                *
*Function to return whether a number is able to go at a specific row or column          *
****************************************************************************************/
bool isAvailable(int row, int col, int num)
{
	int rowStart = (row/3) * 3;
	int colStart = (col/3) * 3;
	int i = 0;

	for(i=0; i<9; ++i)
	{
		if (puzzle[row][i] == num)
			return false;
		if (puzzle[i][col] == num)
			return false;
		if (puzzle[rowStart + (i%3)][colStart + (i/3)] == num)
			return false;
	}
	return true;
}

/****************************************************************************************
*Written By syb0rg edited by Alex Barkin                                                *
*Function to fill the sudoku grid with the correct values                               *
****************************************************************************************/
bool fillSudoku(int row, int col)
{
	int i = 0;
	if(row < 9 && col < 9)
	{
		if(puzzle[row][col] != 0)
		{
			if((col + 1) < 9)
				return fillSudoku(row, col + 1);
			else if((row + 1) < 9)
				return fillSudoku(row + 1, 0);
			else
				return true;
		}
		else
		{
			for(i = 0; i < 9; ++i)
			{
				if(isAvailable(row, col, i + 1))
				{
					puzzle[row][col] = i + 1;
					if((col + 1) < 9)
					{
						if(fillSudoku(row, col + 1))
							return true;
						else
							puzzle[row][col] = 0;
					}
					else if((row + 1) < 9)
					{
						if(fillSudoku(row + 1, 0))
							return true;
						else
							puzzle[row][col] = 0;
					}
					else
						return true;
				}
			}
		}
		return false;
	}
	else
		return true;
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used	to output the 9 by 9 array with grid lines, as well as thicker grid     *
*lines marking the individual 3 by 3 grids                                              *
****************************************************************************************/
void outputArray (bool solved)
{
	if (solved)
	{
		drawRect(6,1,123,118);
		for (int line = 0; line < 8; line++)
		{
			drawLine(19 + 13*line,1,19 + 13*line,118);
			drawLine(6,14 + 13*line,123,14 + 13*line);
			if ((line + 1) % 3 == 0)
			{
				drawLine(18 + 13*line,1,18 + 13*line,118);
				drawLine(20 + 13*line,1,20 + 13*line,118);
				drawLine(6,13 + 13*line,123,13 + 13*line);
				drawLine(6,15 + 13*line,123,15 + 13*line);
			}
		}
		for (int row = 0;row < 9; row++)
			for (int col = 0; col < 9; col ++)
				if (puzzle[row][col] != 0)
					displayStringAt(10 + 13 * col, 115 - 13 * row, "%d", puzzle[row][col]);
		displayBigStringAt(127,110, "Time");
		displayBigStringAt(127,90, "%d", time1[T1]/60000);
		displayBigStringAt(127,70, "Min");
		int sec = (time1[T1]/1000)%60;
		if (sec>9)
			displayBigStringAt(127,50, "%d",sec);
		else
			displayBigStringAt(127,50, "0%d",sec);
		displayBigStringAt(127,30, "Sec");
	}
	else
		displayCenteredBigTextLine (5, "NO SOLUTION");
}

/****************************************************************************************
*Written By Kunj Patel                                                                  *
*Function used to shutdown the robot properly, so the motors are shut off and the main	*
*is aborted	                                                                            *
****************************************************************************************/
void shutDown ()
{
	motor[leftMotor] = motor[centerMotor] = motor[rightMotor] = 0;
	stopAllTasks();
}

/****************************************************************************************
*Written By Kunj Patel                                                                  *
*Function used to call the shut down function when any button is double pressed         *
****************************************************************************************/
task checkPressed()
{
	while (taskStateRunning)
	{
		if (getButtonPress(buttonAny))
		{
			while (getButtonPress(buttonAny))
			{}
			time1[T4] = 0;
			while (time1[T4] < 1000)
			{
				if (getButtonPress(buttonAny))
					shutDown();
			}
		}
	}
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used to display a loading bar and the string "loading..."                     *
****************************************************************************************/
task loadingBar()
{
	setLEDColor(ledOrangeFlash);
	while (taskStateRunning)
	{
		eraseDisplay();
		drawEllipse(26,61,32,55);
		drawEllipse(143,61,149,55);
		drawLine(30,61,147,61);
		drawLine(30,55,147,55);
		eraseRect(30,56,147,60);
		displayCenteredBigTextLine(6, "Solving...");
		for (int bar = 0; bar < 13; bar ++)
		{
			if (bar != 0)
				fillEllipse(26,61,32,55);
			if (bar != 12 && bar != 0)
				fillRect(30,56,30+10*bar,60);
			else
			{
				if (bar != 0)
				{
					fillRect(30,56,147,60);
					fillEllipse(143,61,149,55);
				}
			}
			wait1Msec(1000);
		}
	}
}

/****************************************************************************************
*Written By Cameron Nelson                                                              *
*Function used to return the number corresponding with the RGB values sent in           *
****************************************************************************************/
int colourToNum (long redRead, long greenRead, long blueRead)
{
	int colour = 0;

	if (fabs(redRead - RGBVAL[0][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[0][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[0][2]) < COLOUR_TOL)
	colour = 1;
	else if (fabs(redRead - RGBVAL[1][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[1][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[1][2]) < COLOUR_TOL)
	colour = 2;
	else if (fabs(redRead - RGBVAL[2][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[2][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[2][2]) < COLOUR_TOL)
	colour = 3;
	else if (fabs(redRead - RGBVAL[3][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[3][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[3][2]) < COLOUR_TOL)
	colour = 4;
	else if (fabs(redRead - RGBVAL[4][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[4][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[4][2]) < COLOUR_TOL)
	colour = 5;
	else if (fabs(redRead - RGBVAL[5][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[5][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[5][2]) < COLOUR_TOL)
	colour = 6;
	else if (fabs(redRead - RGBVAL[6][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[6][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[6][2]) < COLOUR_TOL)
	colour = 7;
	else if (fabs(redRead - RGBVAL[7][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[7][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[7][2]) < COLOUR_TOL)
	colour = 8;
	else if (fabs(redRead - RGBVAL[8][0]) < COLOUR_TOL && fabs(greenRead - RGBVAL[8][1])
		< COLOUR_TOL && fabs(blueRead - RGBVAL[8][2]) < COLOUR_TOL)
	colour = 9;

	return colour;
}

/****************************************************************************************
*Written By Alex Barkin	                                                                *
*Function used to drive the left and right motors in the Y-direction. To go straight it *
*checks opposite motor encoder values every 2 encoder clicks at a certain speed         *
****************************************************************************************/
void driveY (int speed)
{
	motor[leftMotor] = motor[rightMotor] = speed;
	while (nMotorEncoder[leftMotor] == 0
		|| fabs(nMotorEncoder[leftMotor]) % MOTOR_PREC != 0)
	{}
	while (fabs(nMotorEncoder[rightMotor]) % MOTOR_PREC != 0)
	{}
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used to drive the center motor in the X-direction at a given speed            *
****************************************************************************************/
void driveX (int speed)
{
	motor[centerMotor] = speed;
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used to reset the sensor hub in the Y-direction by running until both touch 	*
*sensors are hit	                                                                      *
****************************************************************************************/
void resetY ()
{
	nMotorEncoder[rightMotor] = nMotorEncoder[leftMotor] = 0;

	while (SensorValue[S3] == 0 || SensorValue[S1] == 0)
		driveY(-10);

	motor[leftMotor] = motor[rightMotor] = 0;
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used to place the sensor hub at a consistent spot at the bottom right of the  *
*robot(where the hub hits the touch sensors                                             *
****************************************************************************************/
void calibrateMotors ()
{
	driveX(-10);
	while (SensorValue[S4] == 0)
	{}
	motor[centerMotor] = 0;
	while (SensorValue[S3] == 0 || SensorValue[S1] == 0)
			driveY(-10);
	motor[leftMotor] = motor[rightMotor] = 0;
}

/****************************************************************************************
*Written By Prabhdeep Soni                                                              *
*Function used to move the sensor hub one square in the X-direction                     *
****************************************************************************************/
void moveOneX ()
{
	nMotorEncoder[centerMotor] = 0;
	driveX(7);
	while (nMotorEncoder[centerMotor] < X_ENC_LIMIT)
	{}
	motor[centerMotor] = 0;
}

/****************************************************************************************
*Written By Prabhdeep Soni                                                              *
*Function used to move the sensor hub one square in the Y-direction                     *
****************************************************************************************/
void moveOneY ()
{
	nMotorEncoder[rightMotor] = nMotorEncoder[leftMotor] = 0;

	while (nMotorEncoder[rightMotor] < Y_ENC_LIMIT)
		driveY(9);

	motor[rightMotor] = motor[leftMotor] = 0;
}

/****************************************************************************************
*Written By Cameron Nelson                                                              *
*Function used to take in a colour reading for each of the 9 colours to get readings to *
*compare the rest of the grid to this will store the values in the global array RGBVAL	*
****************************************************************************************/
void calibrateColours()
{
	long redRead =0, greenRead = 0, blueRead = 0;
	//First reading to allow clour sensor to get ready
	getColorRGB(S2, redRead, greenRead, blueRead);

	for (int row = 0; row <= 8; row ++)
	{
		eraseDisplay();
		displayCenteredBigTextLine(7, "Calibrating Number:");
		displayCenteredBigTextLine(7, "%d", row+1);
		wait1Msec(2000);
		getColorRGB(S2, redRead, greenRead, blueRead);
		RGBVAL[row][0] = redRead;
		RGBVAL[row][1] = greenRead;
		RGBVAL[row][2] = blueRead;
		if (row !=8)
			moveOneY();
	}
	wait1Msec(50);
	resetY();
	wait1Msec(50);
	moveOneX();
}

/****************************************************************************************
*Written By Cameron Nelson                                                              *
*Function used to take in a colour reading, and assign it the corresponding number at	  *
*the array and column sent in                                                           *
****************************************************************************************/
void readColours (int row, int col)
{
	long redRead =0, greenRead = 0, blueRead = 0;
	getColorRGB(S2, redRead, greenRead, blueRead);

	int colour = colourToNum(redRead, greenRead, blueRead);
	puzzle[row][col] = colour;

	time1[T3] = 0;
	while (time1[T3] < 500)
	{}
}

/****************************************************************************************
*Written By Prabhdeep Soni                                                              *
*Function used to traverse the 9 by 9 grid and calls the function to take colour        *
*readings at every square                                                               *
****************************************************************************************/
void readGrid ()
{
	eraseDisplay();
	for (int xDir = 0; xDir < 9; xDir ++)
	{
		for (int yDir = 8; yDir >= 0; yDir --)
		{
			readColours(xDir, yDir);
			outputArray(true);
			if (yDir != 0)
				moveOneY();
		}
		if (xDir != 8)
		{
			resetY();
			moveOneX();
		}
		else
			calibrateMotors();
	}
}

/****************************************************************************************
*Written By Prabhdeep Soni                                                              *
*Function used to wait until any button is pressed and released before continuing       *
****************************************************************************************/
void waitButton ()
{
	while (!getButtonPress(buttonAny))
	{}
	while (getButtonPress(buttonAny))
	{}
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Function used to output the solved sudoku to a file on the EV3 brick which is to be    *
*accessed by a computer. Ends program, if file cannot be written.                       *
****************************************************************************************/
void writeFile ()
{
	TFileHandle fout;
	bool fileOkay = openWritePC(fout, "SudokuOut.txt");

	if (!fileOkay)
	{
		eraseDisplay();
		displayString(1, "FILE NOT ABLE TO WRITE");
		wait1Msec(1000);
		shutDown();
	}

	writeTextPC(fout, "-------------------------------------");
	writeEndlPC(fout);
	for (int row = 0;row < 9; row++)
	{
			for (int col = 0; col < 9; col ++)
			{
					writeCharPC(fout,'|');
					writeCharPC(fout,' ');
					writeLongPC(fout,puzzle[row][col]);
			}
			writeCharPC(fout,'|');
			writeEndlPC(fout);
			writeTextPC(fout, "-------------------------------------");
			writeEndlPC(fout);
	}

	closeFilePC(fout);
}

/****************************************************************************************
*Written By Alex Barkin                                                                 *
*Main function used to call the other functions, and set up the sensors. It can be      *
*exited by a button press after the grid is outputted, or by a double press of a button *
*or touch sensor while executing                                                        *
****************************************************************************************/
task main()
{
	SensorType [S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode [S2] = modeEV3Color_Color;
	SensorType[S3] = sensorEV3_Touch;
	SensorType[S4] = sensorEV3_Touch;
	startTask(checkPressed);

	openingDisplay();
	waitButton();
	calibrateMotors();
	waitButton();

	calibrateColours();

	time1[T1] = 0;
	readGrid();

	startTask(loadingBar);
	bool solved = fillSudoku(0,0);
	stopTask(loadingBar);

	if (solved)
		setLEDColor(ledGreen);
	else
		setLEDColor(ledRed);
	setSoundVolume(100); //UNMUTED
	playSound(soundBeepBeep); //To signify that array has been outputted
	eraseDisplay();

	outputArray(solved);
	writeFile ();
	waitButton();

	shutDown();
}
