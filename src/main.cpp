#include "main.h"
#include "UvuvLib/Definitions.h"
#include "UvuvLib/Drivetrain.h"
#include "UvuvLib/Flywheel.h"
#include "UvuvLib/GraphingTool.h"
#include "UvuvLib/Motor.h"
#include "UvuvLib/PIDTuner.h"
#include "UvuvLib/UvuvController.h"
#include "pros/misc.h"
#include "UvuvLib/UvuvMain.h"
#include "pros/motors.hpp"
#include <sys/types.h>



UvuvBasicController* controller = new UvuvBasicController(pros::E_CONTROLLER_MASTER);

pros::IMU* inertialSensor = new pros::IMU(9);
pros::GPS gpsSensor(11);

UvuvMotor* leftIntake = new UvuvMotor(1, false, G_GREEN);
UvuvMotor* rightIntake = new UvuvMotor(2, true, G_GREEN);;



UvuvMotor* leftFront = new UvuvMotor(3, false, G_BLUE);
UvuvMotor* leftMount = new UvuvMotor(4, false, G_BLUE);
UvuvMotor* leftBack = new UvuvMotor(5, false, G_BLUE);

UvuvMotor* rightFront = new UvuvMotor(6, true, G_BLUE);
UvuvMotor* rightMount = new UvuvMotor(7, true, G_BLUE);
UvuvMotor* rightBack = new UvuvMotor(8, true, G_BLUE);


UvuvMotor* flywheelA = new UvuvMotor(0, false, G_BLUE);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	inertialSensor->reset(true);
	gpsSensor.initialize_full(0, 0, 0, 0, 0);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {

	// Drivetrain initialization

	UvuvMotorGroup* driveLeftSide = new UvuvMotorGroup({leftFront, leftMount, leftBack});

	UvuvMotorGroup* driveRightSide = new UvuvMotorGroup({rightFront, rightMount, rightBack});

	UvuvDrivetrain drivetrain(driveLeftSide, driveRightSide, G_343_RPM, 
		4.125, controller, inertialSensor, ControlScheme::E_ARCADE_DRIVE);

	// Path intialization
	//Path path = {{0,0}, {12,12}, {0, 12}, {5, 7}};
	//PurePursuit purePursuit(path, &drivetrain, 6, 30);

	while (true) {
		
		// Position can be with whatever you want. I'm using the raw GPS sensor data.
		//purePursuit.step( { gpsSensor.get_status().x, gpsSensor.get_status().y }, 
		//	inertialSensor->get_heading());
		
		drivetrain.takeControllerInput(controller);

		drivetrain.driveTrainMainLoop();

		pros::delay(20);
	
	}

}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */


void opcontrol() {

	// Intake

	std::vector<UvuvMotor*> intakeMotors = {leftIntake, rightIntake};
	
	UvuvMotorGroup intakeGroup(intakeMotors);

	// Drivetrain initialization

	UvuvMotorGroup* driveLeftSide = new UvuvMotorGroup({leftFront, leftMount, leftBack});

	UvuvMotorGroup* driveRightSide = new UvuvMotorGroup({rightFront, rightMount, rightBack});

	UvuvDrivetrain drivetrain(driveLeftSide, driveRightSide, G_343_RPM, 
		4.125, controller, inertialSensor, ControlScheme::E_ARCADE_DRIVE);

	// Flywheel

	UvuvMotorGroup* flywheelMotors = new UvuvMotorGroup({flywheelA});
	UvuvFlywheelController flywheel({flywheelMotors}, G_DIRECT, 4.0, {0,0,0,0});


	while (1) {
		
		drivetrain.driveTrainMainLoop();

		flywheel.step();

		if (controller->getButton(pros::E_CONTROLLER_DIGITAL_R1)) {
			intakeGroup.spinPerc(100);
		}
		else if (controller->getButton(pros::E_CONTROLLER_DIGITAL_R2)) {
			intakeGroup.spinPerc(-100);
		}
		else {
			intakeGroup.spinPerc(0);
		}

		pros::delay(20);

	}

	delete leftIntake;
	delete rightIntake;

	delete rightBack;
	delete rightMount;
	delete rightFront;

	delete leftBack;
	delete leftMount;
	delete leftFront;

	delete flywheelA;

	delete driveRightSide;
	delete driveLeftSide;
	delete flywheelMotors;

}
