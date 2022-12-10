#include <Joystick.h>
#include <Mouse.h>

// set our pins
int potpinRx = A0;
int potpinRy = A1;
int potpinLx = A2;
int potpinLy = A3;
int modepin = 22;

// set our variables
int mode = 0;
int lastmode = 0;
String pressed;

// setup 4x4 matrix using pins 4-7 and 8-11
char cols[4] = {4, 5, 6, 7};
char rows[4] = {8, 9, 10, 11};
const int colCount = sizeof(cols) / sizeof(cols[0]);
const int rowCount = sizeof(rows) / sizeof(rows[0]);
byte keys[colCount][rowCount];

// function for range mapping
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// setup our joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   16, 0,                // Button Count, Hat Switch Count
                   true, true, false,    // X and Y, but no Z Axis
                   true, true, false,    // Rx, Ry, but no Rz
                   false, false,         // No rudder or throttle
                   false, false, false); // No accelerator, brake, or steering

// our setup function
void setup()
{
    // set pin d22 as pullup input
    pinMode(modepin, INPUT_PULLUP);
    // read the mode pin for our initial state
    lastmode = digitalRead(modepin);
    // Initialize Joystick Library
    Joystick.begin();
    Joystick.setXAxisRange(-127, 127);
    Joystick.setYAxisRange(-127, 127);
    Joystick.setRxAxisRange(-127, 127);
    Joystick.setRyAxisRange(-127, 127);

    // initialize the matrix
    for (int i = 0; i < colCount; i++)
    {
        pinMode(cols[i], OUTPUT);
        digitalWrite(cols[i], HIGH);
        Serial.print(cols[i]);
        Serial.print(" ");
    }
    Serial.println();
    for (int i = 0; i < rowCount; i++)
    {
        pinMode(rows[i], INPUT_PULLUP);
        Serial.print(rows[i]);
        Serial.print(" ");
    }
}

// our function to read the matrix
void readMatrix()
{
    // iterate the columns
    for (int colIndex = 0; colIndex < colCount; colIndex++)
    {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);

        // row: interate through the rows
        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
        {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            keys[colIndex][rowIndex] = !digitalRead(rowCol);
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}

// Last state of the buttons
int lastButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// our main loop
void loop()
{
    // Read the mode pin
    int tmpmode = digitalRead(modepin);
    // if the mode pin has changed then change the mode from 0 to 1 or 1 to 0
    if (tmpmode != lastmode && tmpmode == 0)
    {
        mode = !mode;
        Serial.println(mode);
        lastmode = tmpmode;
        // start up the mouse if 1 and stop it if 0
        if (mode == 1)
        {
            Mouse.begin();
        }
        else if (mode == 0)
        {
            Mouse.end();
        }
    }
    else
    {
        lastmode = tmpmode;
    }

    // use the readMatrix function to read the matrix
    readMatrix();

    // check which mode we are in
    if (mode == 0)
    {
        // Read pin values, map them to the proper range
        int potMappedRx = mapfloat(analogRead(potpinRx), 0, 1023, -127, 127);
        int potMappedRy = mapfloat(analogRead(potpinRy), 0, 1023, -127, 127);
        int potMappedLx = mapfloat(analogRead(potpinLx), 0, 1023, -127, 127);
        int potMappedLy = mapfloat(analogRead(potpinLy), 0, 1023, -127, 127);
        // send the joystick values to the computer
        Joystick.setXAxis(potMappedLx);
        Joystick.setYAxis(potMappedLy);
        Joystick.setRxAxis(potMappedRx);
        Joystick.setRyAxis(potMappedRy);

        // read through keys array by row and see if any buttons have changed state
        for (int i = 0; i < rowCount; i++)
        {
            for (int j = 0; j < colCount; j++)
            {
                int buttonIndex = i * colCount + j;
                int buttonState = keys[j][i];
                // if the button state has changed then send the button press to the computer
                if (buttonState != lastButtonState[buttonIndex])
                {
                    Serial.print("keys[");
                    Serial.print(j);
                    Serial.print("][");
                    Serial.print(i);
                    Serial.print("] ");
                    Serial.print("Button ");
                    Serial.print(buttonIndex);
                    Serial.print(" ");
                    Serial.println(buttonState);
                    if (buttonState == 1)
                    {
                        Joystick.pressButton(buttonIndex);
                    }
                    else
                    {
                        Joystick.releaseButton(buttonIndex);
                    }
                    lastButtonState[buttonIndex] = buttonState;
                }
            }
        }
    }
    else if (mode == 1)
    {
        // map potmapped values to between -1 and 1
        float potMappedRy = mapfloat(analogRead(potpinRy), 0, 1023, -10, 10);
        float potMappedLx = mapfloat(analogRead(potpinLx), 0, 1023, -20, 20);
        float potMappedLy = mapfloat(analogRead(potpinLy), 0, 1023, -20, 20);
        Serial.print("ly: ");
        Serial.print(potMappedLy);
        Serial.print(" lx: ");
        Serial.print(potMappedLx);
        Serial.print(" ry: ");
        Serial.println(potMappedRy);

        // move the mouse
        Mouse.move(potMappedLx, potMappedLy, potMappedRy);
        // deal with clicks
        if (keys[0][0] == 1) // left click
        {
            Mouse.press(MOUSE_LEFT);
        }
        else
        {
            Mouse.release(MOUSE_LEFT);
        }
        if (keys[2][0] == 1) // right click
        {
            Mouse.press(MOUSE_RIGHT);
        }
        else
        {
            Mouse.release(MOUSE_RIGHT);
        }
        if (keys[3][0] == 1) // middle click
        {
            Mouse.press(MOUSE_MIDDLE);
        }
        else
        {
            Mouse.release(MOUSE_MIDDLE);
        }
        // delay for 50ms
        delay(25);
    }
}
