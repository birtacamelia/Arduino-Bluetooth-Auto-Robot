# Arduino-Bluetooth-Auto-Robot
This project features a smart dual-mode robot built on a mobile car chassis. It is designed to operate in two distinct modes: an Autonomous Mode that uses ultrasonic sensors to navigate while avoiding obstacles and a Manual Mode for remote steering via Bluetooth.

🛠️ Hardware Architecture & Specifications

🧠 Control & Processing

Microcontroller: Arduino Uno — Acts as the central brain, executing the state machine logic and processing real-time sensor data.

Motor Driver: L293D Shield — A dedicated high-current driver capable of controlling the 4 DC motors simultaneously.

📡 Sensing & Communication

Ultrasonic Sensor (HC-SR04): Used for non-contact distance measurement. It allows the robot to "see" obstacles during CRUISE mode.

Bluetooth Module (HC-05): Enables wireless serial communication, allowing for manual overrides via a smartphone application.

⚙️ Drive System (Movement & Steering)

4x DC Motors: Provides 4-wheel drive traction, ensuring the robot can maneuver on various flat surfaces.

1x Micro Servo (SG90): Positioned at the front to rotate the ultrasonic sensor 180°, enabling environment scanning before choosing a turn direction.

Secțiunea Project Showcase

<p align="center"><img src="images/poza ta.jpg" width="600"></p>

