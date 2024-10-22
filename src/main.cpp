#include "main.h"

const float pi = 3.1415926535897932384626433;
float tmp;
float strafe;
float forwd;
float robotheta;

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
	pros::Imu imu_sensor(11);
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
  	imu_sensor.reset();

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
void autonomous() {}

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
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Imu imu_sensor(11);
	pros::Motor rightback(5);    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::Motor rightfront(6);    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::Motor leftback(-7);    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::Motor leftfront(-2);    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	int time = pros::millis();
	int iter = 0;
	while (imu_sensor.is_calibrating()) {
		printf("IMU calibrating... %d\n", iter);
		iter += 10;
		pros::delay(10);
	}
    printf("IMU is done calibrating (took %d ms)\n", iter - time);
	while (true) {
		robotheta = (imu_sensor.get_rotation() * (pi / 180));
   		tmp = (cos(robotheta) * (-1 * master.get_analog(ANALOG_LEFT_Y)) + sin(robotheta) * (-1 * master.get_analog(ANALOG_LEFT_X)));
    	strafe = (sin(robotheta) * (master.get_analog(ANALOG_LEFT_Y)) + cos(robotheta) * (-1 * master.get_analog(ANALOG_LEFT_X)));
		forwd = tmp;

		if (master.get_analog(ANALOG_RIGHT_X) != 0) {
			leftfront.move(-0.5 * master.get_analog(ANALOG_RIGHT_X)); 
			rightfront.move(0.5 * master.get_analog(ANALOG_RIGHT_X)); 
			rightback.move(0.5 * master.get_analog(ANALOG_RIGHT_X)); 
			leftback.move(-0.5 * master.get_analog(ANALOG_RIGHT_X)); 
		}
		else {
			leftfront.move(0.5 * (forwd + strafe));	
			rightfront.move((forwd - strafe) * 0.5);	
			rightback.move(0.5 * (forwd + strafe));	
			leftback.move((forwd - strafe) * 0.5);	
		}
		printf("rotation: %f radians\n", robotheta);
		printf("rotation: %f degrees\n", imu_sensor.get_rotation());
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		pros::delay(20);                               // Run for 20 ms then update
	}
}