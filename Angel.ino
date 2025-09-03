
// ==============================
// ==== Ultrasonic Sensor Setup ===
// ==============================

// Define ultrasonic sensor pins
const int TRIG_PIN_FRONT = 13;
const int ECHO_PIN_FRONT = 12;

const int TRIG_PIN_LEFT = 17; // Assign appropriate pins
const int ECHO_PIN_LEFT = 18;

const int TRIG_PIN_RIGHT = 15;
const int ECHO_PIN_RIGHT = 16;

//CHRISTIAN added for upper sensor
const int TRIG_PIN_ABOVE = 11;
const int ECHO_PIN_ABOVE = 10;

//CHRISTIAN added time to wait for fall
const int FALL_WAIT_TIME = 5000;

//CHRISTIAN added LED sensor pin
const int ledPin = 9;

// Constants for distance calculation
const float SOUND_SPEED = 0.0343;           // cm/us (speed of sound at 20°C)
const unsigned long SENSOR_TIMEOUT = 30000; // 30ms timeout for echo

// Obstacle detection threshold (centimeters)
const float OBSTACLE_THRESHOLD = 40.0;

// Measurement interval (milliseconds)
const unsigned long SENSOR_MEASUREMENT_INTERVAL = 100;

// Function Prototypes for Ultrasonic Sensors
void setupUltrasonicSensors();
float measureDistance(int trigPin, int echoPin);

// ==============================
// ======= Motor Control Setup ===
// ==============================

// Define motor control pins
const int ENA = 2; // Enable pin for Motor Left (PWM)
const int IN1 = 3; // Direction pin 1 for Motor 1/Left (Output)
const int IN2 = 4; // Direction pin 2 for Motor 1/Left (Input)

const int ENB = 5; // Enable pin for Motor Right (PWM)
const int IN3 = 6;  // Direction pin 1 for Motor 2/Right (Output)
const int IN4 = 7;  // Direction pin 2 for Motor 2/Right (Input)

// Define motor speed
const int MOTOR_SPEED = 200; // Speed value (0-330)


// Function Prototypes for Motor Control
void setupMotorPins();
void moveForward(int speed);
void stopMotors();


// ==============================
// ========= Setup Function ======
// ==============================

void setup()
{
    // Initialize Serial Communication
    Serial.begin(9600);

    //CHRISTIAN added Initialize LED light, start with it off
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // Initialize Ultrasonic Sensors
    setupUltrasonicSensors();

    // Initialize Motor Control
    setupMotorPins();
}

// ==============================
// ========= Loop Function =======
// ==============================

void loop()
{
    //CHRISTIAN separate logic here to test follow, detect, respond functionality
    Serial.println("Delaying to let user get positioned");
    delay(5000);

    while (true) {
        follow();
    }

/*
    int result = detect();
    if (result == 1) {
        respond();
    }
    else {
        Serial.println("No detection. Resuming movement.");
    }
    */
}

//CHRISTIAN added
void follow() {
    float frontDist;
    
    while ((frontDist = measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT)) > OBSTACLE_THRESHOLD) {
        //accelerateForward(MOTOR_SPEED, 20, 100);
        moveForward(MOTOR_SPEED);
    }
    stopMotors();
    Serial.println("Person detected. Stopping to monitor.");
}

//CHRISTIAN added
int detect() {
    float frontDist = measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
    float aboveDist = measureDistance(TRIG_PIN_ABOVE, ECHO_PIN_ABOVE);

    delay(FALL_WAIT_TIME); // time to wait for person falling
    float frontTest = measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
    float aboveTest = measureDistance(TRIG_PIN_ABOVE, ECHO_PIN_ABOVE);

    float frontDif = abs(frontTest - frontDist);
    float aboveDif = aboveTest - aboveDist;

    if (aboveDif > OBSTACLE_THRESHOLD && frontDif < OBSTACLE_THRESHOLD) {
        return 1;
    }
    return 0;
}

//CHRISTIAN added
void respond() {
    Serial.println("Person fell! Activating alert.");
    digitalWrite(ledPin, HIGH);
    delay(5000); // 5 seconds
    digitalWrite(ledPin, LOW);
}


// ==============================
// ==== Ultrasonic Sensor Functions ===
// ==============================

void setupUltrasonicSensors()
{
    // Front sensor
    pinMode(TRIG_PIN_FRONT, OUTPUT);
    pinMode(ECHO_PIN_FRONT, INPUT);
    digitalWrite(TRIG_PIN_FRONT, LOW);

    // Left sensor
    pinMode(TRIG_PIN_LEFT, OUTPUT);
    pinMode(ECHO_PIN_LEFT, INPUT);
    digitalWrite(TRIG_PIN_LEFT, LOW);

    // Right sensor
    pinMode(TRIG_PIN_RIGHT, OUTPUT);
    pinMode(ECHO_PIN_RIGHT, INPUT);
    digitalWrite(TRIG_PIN_RIGHT, LOW);

    //CHRISTIAN added Above sensor
    pinMode(TRIG_PIN_ABOVE, OUTPUT);
    pinMode(ECHO_PIN_ABOVE, INPUT);
    digitalWrite(TRIG_PIN_ABOVE, LOW);

}

float measureDistance(int trigPin, int echoPin)
{
    // Send a trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the duration of the echo pulse in microseconds
    float duration = pulseIn(echoPin, HIGH, SENSOR_TIMEOUT);

    // Check for timeout (no echo received)
    if (duration == 0)
    {
        return -1; // Indicates out of range or no object detected
    }

    // Calculate distance: (duration * speed of sound) / 2
    float distance = (duration * SOUND_SPEED) / 2;
    return distance;
}


// ==============================
// ==== Motor Control Functions ===
// ==============================

void setupMotorPins()
{
    // Initialize Motor 1 pins
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // Initialize Motor 2 pins
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void moveForward(int speed)
{
    // Motor 1 forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);

    // Motor 2 forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
}

void accelerateForward(int finalSpeed, int increment, int delayTime) {
    int currentSpeed = 0; // Start from zero speed

    while (currentSpeed < finalSpeed) {
        // Apply the current speed to both motors
        moveForward(currentSpeed);

        // Increase speed incrementally
        currentSpeed += increment;

        // Ensure the speed does not exceed the finalSpeed
        if (currentSpeed > finalSpeed) {
            currentSpeed = finalSpeed;
        }

        // Wait for a short delay before increasing speed again
        delay(delayTime);
    }
}


void stopMotors()
{
    // Stop Motor 1
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);

    // Stop Motor 2
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
}
