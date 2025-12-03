kandidat nr:
**Parking Quest – threepp-based 3D Parking Game**

This project is a 3D parking game built using the threepp library for C++.
The camera follows behind the car, and the player drives around a large parking lot, avoiding obstacles, completing parking tasks, collecting a key, and opening a door to win the game.

The project is version-controlled using Git and regularly pushed to GitHub as new features were implemented and tested.

**Functionality
Controls**

W – Accelerate the car
S – Reverse
A – Steer left
D – Steer right
SPACE – Handbrake
R – Reset the entire game

**Game Features**

A fully drivable car with acceleration, friction, steering, and momentum

Collision detection with parking cones

A large procedurally generated parking lot

Randomly selected target parking spots

A timed parking mechanic: stay inside the spot for 1.5s to complete it

A key object that spawns after all target spots are completed

A door that opens when the key is collected

A win condition when the player drives through the door

A reset system that restores the entire game without restarting the program

Four visible rotating wheels that visually show motion

To modify behavior such as required parking time, car parameters, or target count, values can be changed directly inside the Game logic code.

**Project Structure**

The project is organized into several modules:

Car – Handles car physics (acceleration, steering, friction, heading, speed)

CameraRig – Third-person follow camera

Parking – Creation of parking spots and parking detection logic

TrafficCones – Spawning random cones as obstacles

Game – Main gameplay controller (state machine, key, door, win, UI text, input)

main.cpp – Application startup and render loop

This structure keeps code modular, readable, and aligned with good software design principles.

**Design Notes**

The program uses several concepts from modern C++ and object-oriented design:

Encapsulation – Physics, rendering models, and game states are separated into classes

Abstraction – Car, camera, parking logic, and world objects have independent responsibilities

Low coupling – The Game class interacts with other modules through clear interfaces

High cohesion – Each class does exactly one job

Although some rendering elements are necessarily tied to threepp, gameplay logic is separated as much as reasonably possible.

**Unit Tests**

The project includes unit tests using Catch2, covering:

Parking spot detection

Random target sequence generation

Car acceleration behavior

Tests can be run through CMake using:

ctest --output-on-failure


Catch2 is integrated using FetchContent to ensure the project builds trivially on other systems.

**Credits**

Several parts of this program were inspired by official threepp examples, especially regarding scene setup, camera behavior, and basic object construction.

ChatGPT (OpenAI) has been used as a tool for assistance in structuring the project, clarifying threepp usage, and generating parts of the code during development. All generated code has been reviewed, modified, and adapted to fit the goals of the project.

threepp repository:
https://github.com/markaren/threepp

Catch2 testing framework:
https://github.com/catchorg/Catch2

**UML Class Diagram**

Below is a placeholder for the UML diagram.
<img width="2083" height="788" alt="image" src="https://github.com/user-attachments/assets/3b404c78-e4f9-4a1e-8651-af46e287e291" />



**Summary**

This project demonstrates:

Modern C++ (C++20)

Good project structure and modular design

3D rendering and input handling with threepp

Custom physics and gameplay logic

Proper Git version control usage

Integration of unit tests with Catch2

Potential for further expansion (UI, audio, sensors, OpenCV, physics engines)
