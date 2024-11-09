#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
#include <FEHSD.h>
#include <FEHRandom.h>

// define for servo_dish_cream
#define SERVO_DISH_CREAM_MIN 508
#define SERVO_DISH_CREAM_MAX 2422
#define SERVO_TICKET_MIN 507
#define SERVO_TICKET_MAX 2494
#define SERVO_BURGER_MIN 500
#define SERVO_BURGER_MAX 2450
// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35
// Defines for pulsing the robot
#define PULSE_TIME .07
#define PULSE_TIME_TWO .04
#define PULSE_POWER 22
// Define for the motor power
#define POWER 15
// Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_2);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
FEHMotor left_motor(FEHMotor::Motor0, 9.0);
AnalogInputPin cdsCell(FEHIO::P0_4);
FEHServo servo_ticket(FEHServo::Servo0);
FEHServo servo_dish_cream(FEHServo::Servo3);
FEHServo servo_burger(FEHServo::Servo6);
DigitalInputPin bump_switch_left(FEHIO::P3_0);
DigitalInputPin bump_switch_right(FEHIO::P3_2);

int motor_percent = 33;                 // Input power level here
double expected_counts = 40.4890175226; // 40.4890175226
double rotate = 6.1;                    // 6.47953484803
int frontBurger = 0;
float frontBurger_x = 27.95, frontBurger_y = 58;
int frontTicket = 1;
float frontTicket_x = 28.199, frontTicket_y = 43.;
int vanilla = 2;
float vanilla_x = 16.6, vanilla_y = 48;
int twist = 3;
float twist_x = 17.2, twist_y = 52.099;
int chocolate = 4;
float chocolate_x = 20., chocolate_y = 55.9;
float sPercentChange = .2;
// Light values are slightly higher than their avg
float red_light = .46;
float blue_light = .8;
// function prototypes
void move_straight(int percent, double counts);
void pulse_forward(int percent, float seconds);
void pulse_counterclockwise(int percent, float seconds);
void check_x(float x_coordinate, float time);
void check_y(float y_coordinate, float time);
void check_heading(float heading);
void ticket();
void move_straight(int percent, double counts);
void turn_right(int percent, double counts);
void turn_left(int percent, double counts);
// bool collision(robot *r, Hitbox *a);
void travel_to(int state);
void printCDS();
void start(bool file);
void individualComp();
void burger_flip();
void ice_cream();
void sink();
void collect_data();
/*
collect data of positions for travel_to function as well as
data of light differences for jukebox and start function
*/
void collect_data()
{
    // Declare variables
    float touch_x, touch_y;
    int points[5] = {0, 1, 2, 3, 4};
    // Call this function to initialize the RPS to a course
    RPS.InitializeTouchMenu();

    // Open SD file for writing
    FEHFile *fptr = SD.FOpen("RPS_Data.txt", "w");

    // Wait for touchscreen to be pressed and released
    LCD.WriteLine("Press Screen to Start");
    while (!LCD.Touch(&touch_x, &touch_y))
        ;
    while (LCD.Touch(&touch_x, &touch_y))
        ;

    // Clear screen
    LCD.Clear();

    // Write initial screen info
    LCD.WriteRC("X Position:", 2, 0);
    LCD.WriteRC("Y Position:", 3, 0);
    LCD.WriteRC("   Heading:", 4, 0);

    // Step through each path point to record position and heading
    for (int n = 0; n < 5; n++)
    {
        // Write point letter
        LCD.WriteRC("Touch to set point ", 0, 0);
        LCD.WriteRC(points[n], 0, 20);

        // Wait for touchscreen to be pressed and display RPS data
        while (!LCD.Touch(&touch_x, &touch_y))
        {
            LCD.WriteRC(RPS.X(), 2, 12);       // update the x coordinate
            LCD.WriteRC(RPS.Y(), 3, 12);       // update the y coordinate
            LCD.WriteRC(RPS.Heading(), 4, 12); // update the heading

            Sleep(10); // wait for a 10ms to avoid updating the screen too quickly
        }
        while (LCD.Touch(&touch_x, &touch_y))
            ;

        // Print RPS data for this path point to file
        SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
    }
    // Close SD file
    SD.FClose(fptr);
}
/*
 * Pulse forward a short distance using time
 */
void pulse_forward(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
/*
 * Pulse counterclockwise a short distance using time
 */
void pulse_counterclockwise(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-1 * percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Use RPS to move to the desired x_coordinate based on the orientation of the QR code
 */
void check_x(float x_coordinate, float time)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = POWER;
    float currentTime = TimeNow();
    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while ((RPS.X() < x_coordinate - .05 || RPS.X() > x_coordinate + .05) && TimeNow() - currentTime < time)
    {
        float difference = RPS.X() - x_coordinate;

        // If current greater than desired
        if (difference > 0)
        {
            // Calculate percent difference
            float percentDiff = difference / x_coordinate;
            // Add thresholds for movement
            if (percentDiff > .3)
            {
                // Pulse backwards at high power
                pulse_forward(-2 * power, .3);
            }
            else if (percentDiff > 0.15)
            {
                // Pulse backwards at medium power
                pulse_forward(-2 * power, .15);
            }
            else if (percentDiff > 0.03)
            {
                // Pulse backwards at medium power
                pulse_forward(-2 * power, PULSE_TIME);
            }
            else
            {
                // Pulse backwards at low power
                pulse_forward(-1 * power, PULSE_TIME_TWO);
            }
        }
        // If current less than desired
        else if (difference < 0)
        {
            // Calculate percent difference
            float percentDiff = difference / x_coordinate;
            // Take the absolute value (because it should be negative)
            percentDiff *= -1;

            // Add thresholds for movement
            if (percentDiff > .3)
            {
                // Pulse backwards at high power
                pulse_forward(2 * power, .3);
            }
            else if (percentDiff > 0.15)
            {
                // Pulse backwards at medium power
                pulse_forward(2 * power, .15);
            }
            else if (percentDiff > 0.03)
            {
                // Pulse backwards at medium power
                pulse_forward(2 * power, PULSE_TIME);
            }
            else
            {
                // Pulse backwards at low power
                pulse_forward(1 * power, PULSE_TIME_TWO);
            }
        }
        else
        {
            break;
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
 * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
 */
void check_y(float y_coordinate, float time)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    float currentTime = TimeNow();
    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while ((RPS.Y() < y_coordinate - .05 || RPS.Y() > y_coordinate + .05) && TimeNow() - currentTime < time)
    {
        float difference = RPS.Y() - y_coordinate;

        // If current greater than desired
        if (difference > 0)
        {
            // Calculate percent difference
            float percentDiff = difference / y_coordinate;
            // Add thresholds for movement
            if (percentDiff > .3)
            {
                // Pulse backwards at high power
                pulse_forward(-2 * power, .3);
            }
            else if (percentDiff > 0.15)
            {
                // Pulse backwards at medium power
                pulse_forward(-2 * power, .15);
            }
            else if (percentDiff > 0.03)
            {
                // Pulse backwards at medium power
                pulse_forward(-2 * power, PULSE_TIME);
            }
            else
            {
                // Pulse backwards at low power
                pulse_forward(-1 * power, PULSE_TIME_TWO);
            }
        }
        // If current less than desired
        else if (difference < 0)
        {
            // Calculate percent difference
            float percentDiff = difference / y_coordinate;
            // Take the absolute value (because it should be negative)
            percentDiff *= -1;

            // Add thresholds for movement
            if (percentDiff > .3)
            {
                // Pulse backwards at high power
                pulse_forward(2 * power, .3);
            }
            else if (percentDiff > 0.15)
            {
                // Pulse backwards at medium power
                pulse_forward(2 * power, .15);
            }
            else if (percentDiff > 0.03)
            {
                // Pulse backwards at medium power
                pulse_forward(2 * power, PULSE_TIME);
            }
            else
            {
                // Pulse backwards at low power
                pulse_forward(1 * power, PULSE_TIME_TWO);
            }
        }
        else
        {
            break;
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    float currentTime = TimeNow();
    float time = 28.;
    while (TimeNow() - currentTime < time)
    {
        float dh = heading - RPS.Heading();
        if (dh < 1 && dh > -1)
        {
            break;
        }
        else if (dh > 0 && RPS.Y() >= 0 && RPS.X() >= 0)
        {
            if (dh > 180)
            {
                pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
            }
            else
            {
                pulse_counterclockwise(-PULSE_POWER, PULSE_TIME_TWO);
            }
            Sleep(RPS_WAIT_TIME_IN_SEC);
        }
        else if (dh < 0 && RPS.Y() >= 0 && RPS.X() >= 0)
        {
            if (dh < -180)
            {
                pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
            }
            else
            {
                pulse_counterclockwise(PULSE_POWER, PULSE_TIME_TWO);
            }
            Sleep(RPS_WAIT_TIME_IN_SEC);
        }
    }
}

void ticket()
{
    // put ticket pushed down
    servo_ticket.SetDegree(100);
    Sleep(.5);
    servo_ticket.SetDegree(66);
    Sleep(.5);
    // drive length
    move_straight(-1 * motor_percent, expected_counts * 8);
    // move ticket holder back up
    servo_ticket.SetDegree(0);
    Sleep(.5);
    servo_ticket.Off();
}
void sink()
{
    servo_dish_cream.SetDegree(105);
    Sleep(.5);
    servo_dish_cream.SetDegree(180);
}
void burger_flip()
{
    servo_burger.SetDegree(45);
    float time = TimeNow();
    right_motor.SetPercent(-motor_percent);
    left_motor.SetPercent(motor_percent);
    while (TimeNow() - time < 2.)
        ;
    right_motor.Stop();
    left_motor.Stop();
    Sleep(.5);
    // turn into it secure the arms
    int direction_right = 1;
    int direction_left = -1;
    for (int i = motor_percent; i >= 0; i -= 6)
    {
        if (direction_right == 1)
        {
            right_motor.SetPercent(direction_right * -i);
            left_motor.SetPercent(direction_left * (i - 5));
        }
        else
        {
            right_motor.SetPercent(direction_right * -(i - 5));
            left_motor.SetPercent(direction_left * i);
        }
        Sleep(.5);
        direction_right *= -1;
        direction_left *= -1;
    }
    servo_burger.SetDegree(180);
    Sleep(.8);
    servo_burger.SetDegree(45);
    Sleep(.5);
}
void ice_cream()
{
    LCD.Clear();
    int cream = RPS.GetIceCream();
    if(cream==1){
        cream=0;
    }
    // Check which ice cream lever to flip
    if (cream == 0)
    {
        // move to vanilla lever
        move_straight(-motor_percent, expected_counts * 12);
        turn_left(motor_percent, expected_counts * rotate);
        move_straight(motor_percent, expected_counts * 10);
        turn_right(motor_percent, expected_counts * 2 * rotate);
        travel_to(vanilla);
    }
    else if (cream == 1)
    {
        // move to twist lever
        move_straight(-motor_percent, expected_counts * 8);
        turn_left(motor_percent, expected_counts * rotate);
        move_straight(motor_percent, expected_counts * 6);
        turn_right(motor_percent, expected_counts * 2 * rotate);
        travel_to(twist);
    }
    else if (cream == 2)
    {
        // move to chocolate lever
        move_straight(-motor_percent, expected_counts * 4);
        turn_left(motor_percent, expected_counts * rotate);
        move_straight(motor_percent, expected_counts * 4);
        turn_right(motor_percent, expected_counts * 2 * rotate);
        travel_to(chocolate);
    }
    right_motor.SetPercent(-motor_percent);
    left_motor.SetPercent(motor_percent);
    while (RPS.Y() >= 0)
        ;
    right_motor.Stop();
    left_motor.Stop();
    move_straight(-motor_percent, expected_counts * .5);
    // push cream down
    servo_dish_cream.SetDegree(70);
    Sleep(.5);
    servo_dish_cream.SetDegree(90);
    move_straight(-1 * motor_percent, expected_counts * 4);
    Sleep(6.);
    servo_dish_cream.SetDegree(50);
    move_straight(motor_percent, expected_counts * 4);
    // move cream back up
    Sleep(.5);
    servo_dish_cream.SetDegree(120);
    move_straight(-motor_percent, expected_counts * .5);
    Sleep(.5);
    servo_dish_cream.SetDegree(50);
    if (cream == 0)
    {
        move_straight(-motor_percent, expected_counts * 8.25);
        LCD.WriteLine("CREAM");
        turn_right(motor_percent, expected_counts * (6.4 / 2));
        move_straight(-motor_percent, expected_counts * 6);
    }
    else if (cream == 1)
    {
        move_straight(-motor_percent, expected_counts * 4.25);
        LCD.WriteLine("CREAM");
        turn_right(motor_percent, expected_counts * (6.4 / 2));
        move_straight(-motor_percent, expected_counts * 14);
    }
    else if (cream == 2)
    {
        LCD.WriteLine("CREAM");
        turn_right(motor_percent, expected_counts * (rotate / 2));
        move_straight(-motor_percent, expected_counts * 20);
    }
    servo_dish_cream.SetDegree(145);
    LCD.WriteLine("UP!");
}

void move_straight(int percent, double counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    left_motor.SetPercent(percent);
    right_motor.SetPercent(-percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts)
        ;

    // Turn off motors
    left_motor.Stop();
    right_motor.Stop();
}
void turn_right(int percent, double counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-1 * percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts)
        ;
    right_motor.Stop();
    left_motor.Stop();
}
void turn_left(int percent, double counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts)
        ;
    right_motor.Stop();
    left_motor.Stop();
}
void travel_to(int state)
{
    switch (state)
    {
    // in front of burger
    // degree:90
    case 0:
        check_heading(0);
        check_x(frontBurger_x, 50.);
        turn_left(motor_percent, expected_counts * rotate);
        check_heading(90);
        break;
    // in front of ticket
    // degree:0
    case 1:
        check_heading(90);
        move_straight(-motor_percent, expected_counts * 6);
        check_y(frontTicket_y, 30.);
        turn_right(motor_percent, expected_counts * rotate);
        check_heading(0);
        break;
    // in front of vanilla
    // degree:135
    case 2:
        check_heading(0);
        check_x(vanilla_x, 30.);
        turn_left(motor_percent, expected_counts * (rotate * 1.5));
        check_heading(135);
        break;
    // in front of twist
    // degree:135
    case 3:
        check_heading(0);
        check_x(twist_x, 30.);
        turn_left(motor_percent, expected_counts * (rotate * 1.5));
        check_heading(135);
        break;
    // in front of chocolate
    // degree:135
    case 4:
        check_heading(0);
        check_x(chocolate_x, 30.);
        turn_left(motor_percent, expected_counts * (rotate * 1.5));
        check_heading(135);
        break;
    // default case
    default:
        LCD.Write("travel_to ERROR");
        break;
    }
}
void printCDS()
{
    LCD.WriteLine("CDS: ");
    LCD.WriteLine(cdsCell.Value());
    Sleep(2.0);
}
void start(bool file)
{
    if (file)
    {
        FEHFile *fptr = SD.FOpen("RPS_Data.txt", "r");
        SD.FScanf(fptr, "%f%f", &frontBurger_x, &frontBurger_y);
        SD.FScanf(fptr, "%f%f", &frontTicket_x, &frontTicket_y);
        SD.FScanf(fptr, "%f%f", &vanilla_x, &vanilla_y);
        SD.FScanf(fptr, "%f%f", &twist_x, &twist_y);
        SD.FScanf(fptr, "%f%f", &chocolate_x, &chocolate_y);
        SD.FClose(fptr);
    }
    // Declare variables
    float touch_x, touch_y;
    LCD.Clear(BLACK);
    LCD.WriteLine("Press Screen to Start");
    while (!LCD.Touch(&touch_x, &touch_y))
        ;
    while (LCD.Touch(&touch_x, &touch_y))
        ;
    // Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);
    servo_dish_cream.SetMin(SERVO_DISH_CREAM_MIN);
    servo_dish_cream.SetMax(SERVO_DISH_CREAM_MAX);
    servo_ticket.SetMin(SERVO_TICKET_MIN);
    servo_ticket.SetMax(SERVO_DISH_CREAM_MAX);
    servo_burger.SetMin(SERVO_BURGER_MIN);
    servo_burger.SetMax(SERVO_BURGER_MAX);
    float t_now;
    t_now = TimeNow();
    double cds = -1;
    double percentChange = -1;
    cds = cdsCell.Value(); // initial CDS value, no light. doesn't change
    while (TimeNow() - t_now < 30.)
    {
        printCDS();
        percentChange = (cds - cdsCell.Value()) / cds;
        if (percentChange < 0)
        {
            percentChange *= -1;
        }
        LCD.WriteLine("percent change");
        LCD.WriteLine(percentChange);
        if (percentChange > sPercentChange)
        {
            break;
        }
    }
}

void individualComp()
{
    // complete course
    // was data collected to the SD?
    bool file = true;
    start(file);
    move_straight(motor_percent, expected_counts * 10);
    turn_left(motor_percent, expected_counts * rotate / 2);
    move_straight(motor_percent, expected_counts * .8);
    int juke = -1; // variable for jukebox; 0==red, 1==blue
    // Set both motors to desired percent
    bool light = false;
    right_motor.SetPercent(-motor_percent);
    left_motor.SetPercent(motor_percent);
    float time = TimeNow();
    while (TimeNow() - time < 3)
    {
        if (cdsCell.Value() < blue_light)
        {
            light = true;
            break;
        }
    }
    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
    if (light)
    {
        move_straight(motor_percent, expected_counts * .125);
        Sleep(.5);
        printCDS();
        if (cdsCell.Value() < red_light)
        {
            juke = 0;
            LCD.WriteLine("RED");
            move_straight(motor_percent, expected_counts * 5);
        }
        else
        {
            LCD.WriteLine("BLUE");
            move_straight(motor_percent, expected_counts * 1);
            juke = 1;
        }
    }
    else
    {
        LCD.WriteLine("RED_DEFAULT");
        move_straight(-motor_percent, expected_counts * 1.3);
    }

    turn_right(motor_percent, expected_counts * rotate);
    check_heading(90);
    // run into jukebox
    right_motor.SetPercent(motor_percent);
    left_motor.SetPercent(-1 * motor_percent);
    Sleep(1.5);
    right_motor.Stop();
    left_motor.Stop();
    move_straight(motor_percent, expected_counts * 12);
    turn_right(motor_percent, expected_counts * (rotate + .2));
    if (juke == 0 || juke == -1)
    {
        move_straight(motor_percent, expected_counts * 9);
    }
    else
    {
        move_straight(motor_percent, expected_counts * 6);
    }
    move_straight(motor_percent, expected_counts * 2);
    turn_left(motor_percent, expected_counts * rotate);
    // go up the ramp
    check_heading(90);
    move_straight(75, expected_counts * 28.5);
    check_y(50, 10.);
    // do the dishes
    turn_left(motor_percent, expected_counts * rotate);
    left_motor.SetPercent(motor_percent);
    right_motor.SetPercent(-motor_percent);
    float currentTime = TimeNow();
    while (TimeNow() - currentTime < 1.5)
    {
        if (bump_switch_left.Value() == 0 && bump_switch_right.Value() == 0)
        {
            break;
        }
    }
    left_motor.Stop();
    right_motor.Stop();
    move_straight(-1 * motor_percent, expected_counts * 2);
    turn_left(motor_percent, expected_counts * rotate);
    left_motor.SetPercent(motor_percent);
    right_motor.SetPercent(-motor_percent);
    currentTime = TimeNow();
    while (TimeNow() - currentTime < .7)
    {
        if (bump_switch_left.Value() == 0 && bump_switch_right.Value() == 0)
        {
            break;
        }
    }
    left_motor.Stop();
    right_motor.Stop();
    // drop the tray
    LCD.WriteLine("DISHES");
    sink();
    // tray dropped
    move_straight(-1 * motor_percent, expected_counts * 2);
    turn_left(motor_percent, expected_counts * rotate);
    move_straight(motor_percent, expected_counts * 16);
    turn_left(motor_percent, expected_counts * rotate);
    travel_to(frontTicket);
    float t = TimeNow();
    while (TimeNow() - t < .5)
    {
        right_motor.SetPercent(-motor_percent);
        left_motor.SetPercent(motor_percent);
    }
    right_motor.Stop();
    left_motor.Stop();
    check_heading(0);
    // move to ticket
    LCD.WriteLine("TICKET");
    ticket();
    turn_left(motor_percent, expected_counts * (rotate - 2));
    check_heading(60);
    move_straight(motor_percent, expected_counts * 16);
    // burger time
    turn_right(motor_percent, expected_counts * (rotate - 2));
    travel_to(frontBurger);
    LCD.WriteLine("FLIPBURG");
    burger_flip();
    ice_cream();
    check_heading(85);
    move_straight(-2 * motor_percent, expected_counts * 26);
    turn_left(motor_percent, expected_counts * (rotate / 2));
    Sleep(.1);
    LCD.WriteLine("END");
    move_straight(-3 * motor_percent, expected_counts * 20);
    while (true)
    {
        move_straight(motor_percent, expected_counts * 5);
        turn_right(motor_percent, expected_counts * rotate / 1.2);
        Sleep(.1);
        LCD.WriteLine("END");
        move_straight(-motor_percent, expected_counts * 10);
    }
}

int main(void)
{
    collect_data();
    LCD.Clear();
    individualComp();
}