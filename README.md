# SCOPE — Space Collision & Orbital Proximity Evaluation System

SCOPE is a console-based project developed for **204ES — Object Oriented Programming**. It models satellites, space debris, and rocket bodies in simplified two-dimensional circular orbits, estimates the closest approach between object pairs, calculates relative velocity, and assigns a project-defined risk category.

---

## Project Overview

Earth's orbital environment contains active satellites, inactive spacecraft, spent rocket stages, and fragments of space debris. When two orbiting objects pass close to one another, the event is commonly described as a **conjunction**. Real-world conjunction assessment requires reliable tracking data, uncertainty analysis, and specialised orbital-dynamics software.

SCOPE presents a smaller, understandable version of this problem for learning and demonstration. The application creates simulated orbital objects, estimates their positions over a selected time interval, searches for the minimum separation between each pair, calculates relative velocity, and reports a risk category. Its main academic purpose is to apply **object-oriented programming in C++17** to a practical, space-related problem.

## Objectives

- Represent different kinds of orbiting objects using reusable C++ classes.
- Model simplified two-dimensional circular orbital motion.
- Estimate the minimum separation and time of closest approach for object pairs.
- Calculate relative velocity at the estimated closest approach.
- Categorise encounters using a transparent, project-defined scoring model.
- Save and reload simulated object data using text files.
- Generate readable reports from simulation results.
- Demonstrate core and advanced C++ object-oriented programming concepts.

## Key Features

- **Multiple object types:** satellites, space debris, and rocket bodies.
- **Simplified orbital model:** circular motion in a two-dimensional coordinate plane.
- **Closest-approach search:** checks object separation over a simulation interval.
- **Relative-velocity estimate:** compares the velocities of two objects at the selected time.
- **Risk categories:** `LOW`, `MODERATE`, `HIGH`, and `CRITICAL`.
- **Console interface:** menu-based interaction and text output.
- **Input validation:** validates numeric input and orbital parameters.
- **File persistence:** saves and loads objects using text files.
- **Report generation:** creates a text-based report of assessed encounters.
- **Automated tests:** the project report records 14 passing test cases.

## How It Works

At a high level, SCOPE follows this sequence:

1. The user selects an operation from the console menu.
2. The user enters object information and orbital parameters.
3. The program validates the input and constructs the appropriate object type.
4. The simulation stores the objects and calculates their positions and velocities at requested times.
5. The simulation compares object pairs and searches for their minimum separation over the selected interval.
6. The risk analyser combines encounter information with object-type risk contributions.
7. Results are displayed in the console and can be written to a report file.
8. Object data can be saved to or restored from text files.

### Main Components

| Component | Responsibility |
|---|---|
| `Vector2D` | Represents two-dimensional position and velocity vectors; supports vector arithmetic. |
| `Orbit` | Stores and validates orbital parameters and calculates circular-orbit position, velocity, speed, angular velocity, and period. |
| `OrbitalObject` | Abstract base class for common identity and orbit information shared by orbiting objects. |
| `Satellite` | Represents an active or inactive satellite. |
| `SpaceDebris` | Represents an uncontrolled debris object. |
| `RocketBody` | Represents a rocket body or spent stage. |
| `Simulation` | Owns the objects, calculates separations, and searches for closest approaches. |
| `Conjunction` | Stores the information associated with an encounter between two objects. |
| `RiskAnalyzer` | Applies the project-defined scoring method and assigns a risk category. |
| `DataManager` | Saves and loads object data. |
| `ReportGenerator` | Produces a readable report of simulation results. |
| `ConsoleUI` | Coordinates user interaction and menu operations. |
| `InputHelper` | Provides reusable, validated input-reading utilities. |

The implementation also uses exception types for invalid input and invalid orbital parameters.

## Mathematical Model

SCOPE uses a simplified circular-orbit model. It does not propagate real satellite ephemerides or account for the full set of physical effects used in professional orbit determination.

### Orbital radius

The orbital radius is measured from Earth's centre:

\[
r = R_E + h
\]

where:

- \(r\) is orbital radius in kilometres;
- \(R_E\) is the adopted mean Earth radius, approximately 6371 km;
- \(h\) is altitude above Earth's surface in kilometres.

### Circular orbital speed

For an ideal circular orbit, the speed is estimated using:

\[
v = \sqrt{\frac{\mu}{r}}
\]

where \(\mu\) is Earth's standard gravitational parameter and consistent units must be used.

### Angular velocity and orbital period

The angular velocity and period are related to the orbital radius and speed:

\[
\omega = \frac{v}{r}, \qquad T = \frac{2\pi}{\omega}
\]

For an initial angular position \(\theta_0\), a simplified angular position at time \(t\) is:

\[
\theta(t) = \theta_0 + d\omega t
\]

where \(d=+1\) or \(d=-1\) represents the chosen direction of motion. The implementation uses angles and time consistently when calculating positions.

### Position and velocity

In a two-dimensional circular model, position can be represented as:

\[
\mathbf{r}(t) = \begin{bmatrix}r\cos\theta(t)\\r\sin\theta(t)\end{bmatrix}
\]

The corresponding velocity vector is tangential to the circular path. SCOPE uses two-dimensional vectors to compare positions and velocities.

### Separation and relative velocity

For two objects A and B at the same time \(t\), the separation is:

\[
d_{AB}(t) = \left\|\mathbf{r}_A(t)-\mathbf{r}_B(t)\right\|
\]

The relative velocity vector is:

\[
\mathbf{v}_{rel}(t)=\mathbf{v}_A(t)-\mathbf{v}_B(t)
\]

and the relative speed is its magnitude:

\[
v_{rel}(t)=\left\|\mathbf{v}_{rel}(t)\right\|
\]

The program scans the selected interval and identifies the smallest sampled separation. Therefore, the result depends on the time interval and scan resolution; it is an estimate, not a guaranteed continuous-time minimum.

## Risk Assessment

After a closest-approach estimate is found, `RiskAnalyzer` assigns a project-defined risk score and maps it to one of four labels:

- `LOW`
- `MODERATE`
- `HIGH`
- `CRITICAL`

The score considers encounter information and the risk contribution associated with each object type. The documented model assigns different contributions to object categories and distinguishes active from inactive satellites. The exact thresholds are **project-defined**, not official operational thresholds used by a space agency.

A risk label from SCOPE should be interpreted only as the output of this educational scoring model. It is not a probability of collision and is not a substitute for covariance-based conjunction analysis.

## Object-Oriented Design

The program separates orbital objects, simulation calculations, risk evaluation, data storage, report creation, and user interaction into dedicated classes. This separation makes the application easier to explain, test, and extend.

### Class relationships

- **Inheritance:** `Satellite`, `SpaceDebris`, and `RocketBody` derive from `OrbitalObject`.
- **Composition:** each `OrbitalObject` contains an `Orbit`; the simulation owns its stored objects.
- **Association:** a `Conjunction` identifies the two objects involved in an encounter.
- **Dependencies:** the risk analyser, data manager, and report generator use simulation objects and results to perform their tasks.

The simulation stores derived objects through smart pointers to the abstract base class, allowing different object types to be processed through a common interface.

## C++ Concepts Demonstrated

| Concept | How it is used in SCOPE |
|---|---|
| Classes and objects | Domain concepts such as an orbit, satellite, vector, simulation, and conjunction are represented as classes and instances. |
| Encapsulation | Data members are private and accessed through appropriate member functions. Orbital parameters are validated. |
| Constructors | Constructors initialise objects and validate essential data. Derived constructors initialise the base class. |
| Destructors | The virtual base-class destructor supports safe destruction through base-class pointers; object-count tracking is updated during object lifetime. |
| Inheritance | `Satellite`, `SpaceDebris`, and `RocketBody` reuse common features from `OrbitalObject`. |
| Abstraction | `OrbitalObject` defines a common interface without specifying every derived object's behaviour. |
| Abstract classes | `OrbitalObject` contains pure virtual functions and cannot be instantiated directly. |
| Virtual functions | Derived classes provide object-specific behaviour such as type identification, mass calculation, risk contribution, serialisation, and detail display. |
| Runtime polymorphism | Different derived objects are processed through `OrbitalObject` pointers or references. |
| Function overloading | The simulation provides different forms of distance calculation, including object-based and ID-based calls. |
| Operator overloading | `Vector2D` overloads arithmetic operators; stream insertion supports readable output; `Conjunction` comparison supports ordering by severity. |
| Friend functions | The stream insertion operator for `Vector2D` accesses vector data through the declared friend interface. |
| Static members | A class-level object counter and a static orbital-speed utility are used. |
| Templates | `InputHelper` provides reusable numeric input functions for different numeric types. |
| STL containers | `vector`, `map`, and other standard-library utilities store objects, conjunctions, and grouped information. |
| Smart pointers / RAII | `unique_ptr` manages object ownership and automatically releases objects when ownership ends. |
| Composition | An orbital object contains an `Orbit`; the console interface coordinates its manager objects. |
| Exception handling | Invalid parameters and input conditions can raise exceptions that are handled by the application. |
| File handling | Standard file streams and filesystem facilities are used to persist object data and generated reports. |
| Input validation | Reusable input functions and constructor checks help reject invalid data. |
| Sorting and algorithms | Standard-library algorithms are used for ordering or processing stored results where appropriate. |
| Deleted copy operations | Objects with unique identity prevent copying through deleted copy construction/assignment where documented. |
| `const` member functions | Read-only operations are marked `const` where appropriate. |
| Default arguments | Vector construction and selected function interfaces use default arguments where documented. |

These concepts are used to solve concrete design problems rather than being included only as demonstrations.

## Technology Stack

- **Language:** C++17
- **Standard library:** containers, algorithms, smart pointers, exceptions, file streams, and filesystem utilities
- **Compiler:** `g++` with C++17 support; MinGW-w64/MSYS2 can be used on Windows
- **Editor:** Visual Studio Code or another C++ editor
- **Version control:** Git and GitHub
- **Build tool:** a direct compiler command is sufficient; GNU Make may be used if a Makefile is included in the repository

SCOPE uses the C++ standard library and does not require a third-party library, external API, database, or internet connection at runtime.

## Requirements

### Hardware

- A standard laptop or desktop capable of running a modern operating system.
- A small amount of free disk space for the source code, saved data, and generated reports.

### Software

- A C++17-compatible compiler such as a recent `g++` release.
- A terminal or command prompt.
- An editor such as Visual Studio Code (optional but recommended).

## Build and Run

1. Clone or download this repository.
2. Open a terminal in the folder containing the C++ source file.
3. Compile the source using a C++17 compiler.
4. Run the generated executable.

For a single-file source named `Scope_final.cpp`, the commands are:

### Linux / macOS / GitHub Codespaces

```bash
g++ -std=c++17 -Wall -Wextra -pedantic Scope_final.cpp -o scope_app
./scope_app
```

### Windows (MinGW-w64 / MSYS2)

```bash
g++ -std=c++17 -Wall -Wextra -pedantic Scope_final.cpp -o scope_app.exe
./scope_app.exe
```

> **If your repository uses multiple `.cpp` files**, compile all implementation files together, or use the project's supplied build configuration. The commands above assume a single source file named `Scope_final.cpp`.

## Typical Workflow

1. Start the application.
2. Select the relevant option from the console menu.
3. Add or load simulated orbital objects.
4. Review the stored objects and their orbital details.
5. Run the simulation to evaluate closest approaches.
6. Review estimated separation, encounter time, relative velocity, and risk category.
7. Generate and save a report if required.
8. Save the object data before exiting if you want to reuse it later.

Menu wording and file names depend on the version of the source code in the repository.

## Data Storage and Reports

SCOPE uses text-based files rather than a database. The `DataManager` is responsible for serialising and restoring object information. The `ReportGenerator` creates a readable report from the assessed encounters. Text files make it easier to inspect example inputs and generated results while learning file handling in C++.

The implementation uses a delimiter-based serialisation format for object records. Keep values compatible with the expected format when editing saved data manually; malformed records may be rejected during loading.

## Testing

The project report records **14 passing automated test cases**. Testing is intended to cover core behaviour such as:

- Validation of orbital parameters and invalid input.
- Orbital position and velocity calculations.
- Object creation and type-specific behaviour.
- Distance and closest-approach calculations.
- Risk assessment and encounter ordering.
- Data saving/loading and report generation.
- Input error handling.

Run the test command or test executable included in the repository, if present. The precise command depends on whether tests are kept in a separate source file or integrated into the main project. Do not treat the reported test count as a guarantee that every possible input or physical scenario has been tested.

## Limitations

- Uses idealised two-dimensional circular orbits.
- Uses simulated rather than authoritative real-world tracking data.
- Searches over a sampled time interval, so the closest approach is resolution-dependent.
- Does not model orbital uncertainty or covariance propagation.
- Does not include the full perturbation effects used in professional orbit propagation.
- Uses a project-defined risk score rather than an operational collision probability.
- Does not connect to satellite catalogues, tracking networks, or live space situational-awareness services.
- Is intended for education and demonstration, not operational use.

## Future Improvements

Possible extensions, if the project scope is expanded, include:

- Adding a more refined time-step strategy or numerical minimisation for closest approach.
- Supporting additional orbital models beyond circular motion.
- Adding uncertainty-aware analysis using suitable covariance data.
- Importing validated orbital elements from an authorised data source for research demonstrations.
- Improving report formatting and adding plots of separation versus time.
- Adding a graphical interface while keeping the simulation logic independent of the interface.
- Expanding unit tests and adding continuous integration.
- Improving configuration and documentation for multiple source files and build systems.

Any such extension would require careful validation before its outputs could be interpreted physically.

## Project Structure

A typical single-source repository may look like this:

```text
SCOPE/
├── Scope_final.cpp       # Main C++17 implementation (if using a single-file version)
├── README.md             # Project documentation
├── data/                 # Saved simulation data, if included
├── reports/              # Generated reports, if included
└── tests/                # Test source files, if included
```

This is an illustrative layout, not a claim that every directory already exists. Adjust the tree to match the files actually committed to your GitHub repository. Avoid committing compiled executables or temporary output unless your course requires them.

## Learning Outcomes

Through this project, the developer practises:

- Translating a real-world-inspired problem into classes and responsibilities.
- Designing an abstract base class and extending it with derived classes.
- Applying runtime polymorphism and smart-pointer ownership.
- Performing vector calculations and implementing simplified mathematical models.
- Validating input and handling exceptions safely.
- Using standard-library containers and algorithms.
- Reading and writing structured text files.
- Testing, documenting, compiling, and maintaining a C++ application with Git and GitHub.

## Author and Academic Context

**Project:** SCOPE — Space Collision & Orbital Proximity Evaluation System  
**Author:** Dhruvansh Panchal  
**Student ID:** 25EC417  
**Programme:** Electronics and Communication Engineering  
**Academic level:** Second Year, Semester 3  
**Course:** 204ES — Object Oriented Programming  
**Faculty:** Mayur Sevak  
**Institution:** Birla Vishvakarma Mahavidyalaya, Vallabh Vidyanagar, Gujarat, India
