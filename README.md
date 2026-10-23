# SCOPE — Space Collision & Orbital Proximity Evaluation System

> **EDUCATIONAL SIMULATION.** SCOPE uses *simulated* data and a *simplified* 2D circular-orbit model to demonstrate Object-Oriented Programming in C++. It is **not** an operational collision-prediction system and its risk levels are **project-defined**, not official ISRO/NASA/ESA standards.

Course project for **204ES – Object Oriented Programming** (2nd year, Semester 3, Electronics & Communication Engineering). Individual project.

---

## 1. Project Overview
SCOPE is a menu-driven C++ console application. You register satellites, space debris and rocket bodies; SCOPE simulates their orbits, finds the closest approach of every pair, classifies the encounter as LOW / MODERATE / HIGH / CRITICAL, and writes a risk report.

## 2. Problem Statement
The increasing number of satellites and space debris in Earth's orbit raises the risk of close approaches and potential collisions. Continuous monitoring of orbital objects is essential to identify hazardous encounters and maintain safe space operations. Early risk assessment can help prevent collisions and protect valuable space assets.

## 3. Motivation
Thousands of active satellites share orbit with a large amount of debris moving at ~7–8 km/s. Even a small fragment can be destructive at such speed. Space agencies run sophisticated conjunction-assessment systems; SCOPE shows, at student level, how such a problem can be **modelled with objects** (satellite, debris, orbit, conjunction, analyzer) and solved with clean OOP design.

## 4. Objectives
1. Model orbital objects using inheritance, abstraction and polymorphism.
2. Simulate simplified circular orbits using basic physics.
3. Detect the closest approach of every pair of objects.
4. Classify risk with a transparent, explainable scoring model.
5. Save/load data and generate reports using file handling.
6. Handle all normal user errors using exceptions.

## 5. Features
Add satellites / debris / rocket bodies · view objects and details · simulate positions at several times · detect close approaches (min distance, time, relative velocity) · analyse one pair or all pairs · generate and save risk reports · save/load data files · built-in SIMULATED sample scenario · adjustable simulation duration and time step · robust input validation.

## 6. Technologies Used
C++17, STL (`vector`, `map`, `unique_ptr`, `sort`, `any_of`), `<fstream>`, `<filesystem>`, `<cmath>`, g++, Make (optional). No external libraries, APIs or internet.

## 7. OOP Concepts Used (and where)

| Concept | Where in SCOPE | Why it is useful |
|---|---|---|
| Classes & objects | Every `.h/.cpp` pair | Each real-world thing is one class |
| Encapsulation | All data members `private`; access via getters. `Orbit` validates in its constructor | An object can never hold an invalid altitude |
| Constructors | `Orbit(alt, angle, dir)` validates and computes r, v, ω, T; derived constructors call `OrbitalObject(...)` | Objects always start valid |
| Destructor | `virtual ~OrbitalObject()` decrements the object counter | Correct clean-up through base pointers |
| Inheritance | `Satellite`, `SpaceDebris`, `RocketBody` ← `OrbitalObject` | Shared ID/name/orbit code written once |
| Abstraction / abstract class | `OrbitalObject` has pure virtual functions, so it cannot be instantiated | "An orbital object" is a concept; only concrete types exist |
| Runtime polymorphism / virtual functions | `getObjectType()`, `calculateMass()`, `calculateRiskContribution()`, `serialize()`, `displayDetails()` called through `OrbitalObject&`/pointer in `Simulation::listObjects`, `RiskAnalyzer::assess`, `DataManager::saveAll` | The same call behaves differently per type; adding a new object type needs no change in those classes |
| Function overloading | `Simulation::getDistance(obj, obj, t)` and `getDistance(id, id, t)` | Same task, different input forms |
| Operator overloading | `Vector2D` `+ - *` and `<<`; `Conjunction::operator<` | `(posB - posA).magnitude()` reads like maths; sorting by severity |
| Static members | `OrbitalObject::totalObjects_`/`getTotalObjects()`, `Orbit::speedAtAltitude()` | Class-level data/utility with no object needed |
| Friend function | `operator<<(ostream&, const Vector2D&)` | Standard way to print an object with `cout <<` |
| Templates | `InputHelper::readNumber<T>`, `readNumberInRange<T>` | One routine reads `int` and `double` safely |
| STL containers | `vector<unique_ptr<OrbitalObject>>`, `vector<Conjunction>`, `map<string, vector<string>>` | Dynamic storage, sorting, grouping |
| Composition | `OrbitalObject` *has an* `Orbit`; `Simulation` *owns* its objects | Life-time managed automatically |
| Exception handling | `Exceptions.h` hierarchy; caught in `ConsoleUI::run` and in `DataManager` loading | Program never crashes on user mistakes |
| File handling | `DataManager` (`ifstream`, `ofstream`), `ReportGenerator` | Persistent data and reports |
| `const` correctness, smart pointers | Getters are `const`; `unique_ptr` ownership | Safer, leak-free code |

(Concepts such as multiple inheritance or `fstream` read/write mode are deliberately **not** forced in.)

## 8. System Architecture

```
                        main.cpp
                           |
                       ConsoleUI  (menu, input, output)
        +------------+-------+----------+---------------+
        |            |                  |               |
   Simulation   RiskAnalyzer      DataManager    ReportGenerator
   (owns objects) (assesses)      (save / load)   (build / save report)
        |   \________ uses ________/
        |
  vector<unique_ptr<OrbitalObject>>      Conjunction (result of one pair)
        |                                      
 OrbitalObject (abstract) --has--> Orbit --uses--> Vector2D
     ^        ^        ^
 Satellite SpaceDebris RocketBody
```

## 9. Class Diagram (UML-style)

```
                 <<abstract>>
                 OrbitalObject
   ------------------------------------------
   - id_ : string      - name_ : string
   - orbit_ : Orbit    - totalObjects_ : int {static}
   ------------------------------------------
   + getId(), getName(), getOrbit()
   + getPositionAt(t) : Vector2D
   + getVelocityAt(t) : Vector2D
   + {abstract} getObjectType() : string
   + {abstract} calculateMass() : double
   + {abstract} calculateRiskContribution() : double
   + {abstract} serialize() : string
   + displayDetails(os)            <<virtual>>
   + getTotalObjects() : int       {static}
   # serializeCommon() : string
                    ^
      +-------------+--------------+
      |             |              |
  Satellite     SpaceDebris     RocketBody
  - massKg_     - sizeCm_       - massKg_
  - active_     - source_

 Orbit                          Vector2D
 - altitudeKm_, startAngleDeg_  - x_, y_
 - direction_, radiusKm_        + magnitude(), distanceTo()
 - speedKmS_, omega_, periodSec_+ operator+ - *   friend operator<<
 + positionAt(t), velocityAt(t)
 + speedAtAltitude() {static}

 Simulation                     Conjunction
 - objects_ : vector<unique_ptr>- idA_, idB_, minDistanceKm_
 - durationSec_, stepSec_       - tcaSec_, relVelocityKmS_
 + addObject(), getObject()     - scores, level_ : RiskLevel
 + getDistance() x2 (overload)  + setRisk(), display()
 + findClosestApproach()        + operator<
 + findAllApproaches()

 RiskAnalyzer        DataManager            ReportGenerator
 + assess()          + saveAll(sim)         + buildReport(results, sim)
 + analyzePair()     + loadAll(sim)         + saveReport(text)
 + analyzeAll()      + populateSampleData()

 ConsoleUI  (uses all of the above)
```

**Relationships**
- **Inheritance (is-a):** Satellite / SpaceDebris / RocketBody → OrbitalObject.
- **Composition (part-of, same life-time):** OrbitalObject → Orbit; Orbit → (computes with) Vector2D values; Simulation → owns its `unique_ptr<OrbitalObject>` list; ConsoleUI → owns Simulation, RiskAnalyzer, DataManager, ReportGenerator.
- **Association:** Conjunction refers to two objects by their IDs.
- **Dependency (uses temporarily):** RiskAnalyzer → Simulation, Conjunction, OrbitalObject; ReportGenerator → Conjunction, Simulation; DataManager → Simulation and the three concrete classes (to create them when loading).

## 10. Class Descriptions

| Class | Purpose | Key data | Key functions |
|---|---|---|---|
| `Vector2D` | 2D position/velocity | x, y | `magnitude`, `distanceTo`, `+ - *`, `<<` |
| `Orbit` | Circular orbit maths | altitude, angle, direction, r, v, ω, T | `positionAt`, `velocityAt`, `speedAtAltitude` |
| `OrbitalObject` (abstract) | Common identity + interface | id, name, `Orbit` | virtual interface above |
| `Satellite` | Active/inactive satellite | mass, active | risk contribution 2.0 (active) / 5.0 (inactive) |
| `SpaceDebris` | Uncontrolled fragment | size, source | mass from size (aluminium sphere); risk 7.0 |
| `RocketBody` | Spent stage | mass | risk 5.0 |
| `Conjunction` | Result for one pair | IDs, min distance, TCA, rel. velocity, score, level | `display`, `isCloseApproach`, `operator<` |
| `Simulation` | Owns objects, finds closest approaches | `vector<unique_ptr<OrbitalObject>>`, duration, step | `addObject`, `findClosestApproach`, `findAllApproaches` |
| `RiskAnalyzer` | Scores and classifies | — | `assess`, `analyzePair`, `analyzeAll` |
| `DataManager` | File save/load, sample data | data folder | `saveAll`, `loadAll`, `populateSampleData` |
| `ReportGenerator` | Report text + file | report folder | `buildReport`, `saveReport` |
| `ConsoleUI` | Menu and input | the five objects above | `run`, one function per menu item |
| `InputHelper` (templates) | Safe input | — | `readNumber<T>`, `readNumberInRange<T>` |
| `Exceptions.h` | Error types | — | `ScopeException` and subclasses |

## 11. Mathematical Model
All symbols: distances in km, time in s.

| Quantity | Formula | Meaning |
|---|---|---|
| Earth radius | `R_E = 6371 km` | mean radius |
| Orbital radius | `r = R_E + h` | distance from Earth's centre (h = altitude) |
| Circular orbital speed | `v = sqrt(GM / r)`, `GM = 398600.4418 km³/s²` | speed needed to stay in circular orbit |
| Angular velocity | `ω = v / r` | how fast the angle grows (rad/s) |
| Period | `T = 2π / ω` | time for one revolution |
| Angle | `θ(t) = θ₀ ± ω t` (+ prograde, − retrograde) | where on the circle |
| Position | `x = r cos θ`, `y = r sin θ` | 2D coordinates |
| Velocity | `v⃗ = ±v(−sin θ, cos θ)` | tangent to the circle |
| Relative position | `Δ = P_B − P_A` | |
| Distance | `d = sqrt(Δx² + Δy²)` | separation |
| Relative velocity | `|v⃗_A − v⃗_B|` | closing speed |
| Debris mass (estimate) | `m = ρ (4/3)π (D/2)³`, ρ = 2700 kg/m³ | |

Example: h = 550 km → r = 6921 km, v ≈ 7.59 km/s, T ≈ 95.5 min.

**Why simplified?** A 2D circle needs only trigonometry, which keeps the focus on OOP. **Real orbital mechanics differs:** orbits are ellipses in 3D with inclination; Earth's shape, atmospheric drag, Sun/Moon gravity and solar pressure perturb them; positions come from measured data with uncertainty; and real assessments compute a probability of collision.

## 12. Algorithm – Closest Approach (per pair)
1. Compute both positions at `t = 0`, then at every step `t = Δt, 2Δt, …` up to the duration (default 120 min, 1 s).
2. Separation `d(t) = |P_B(t) − P_A(t)|`.
3. A sample smaller than both neighbours is a **local minimum**.
4. Around each local minimum, shrink the time interval repeatedly (60 passes) to find the true minimum between samples (otherwise at 15 km/s a 1 s step could miss ~15 km).
5. Keep the smallest; on ties keep the earliest. Record distance and time.
6. Relative velocity `|v⃗_A − v⃗_B|` at that time.
7. If `d ≤ 25 km` (project threshold) → **close approach**.

Cost: pairs × (duration/step) distance evaluations — a few hundred thousand for the sample, instant.

## 13. Risk Assessment Methodology (project-defined, educational)
If minimum distance > 25 km → **LOW** (no close approach). Otherwise score = distance points + velocity points + object points (max ≈ 100):

| Distance | Pts | Rel. velocity | Pts | Object contribution (each object) | Pts |
|---|---|---|---|---|---|
| ≤ 1 km | 60 | ≥ 10 km/s | 25 | Active satellite | 2 |
| ≤ 5 km | 45 | ≥ 5 km/s | 15 | Inactive satellite | 5 |
| ≤ 10 km | 30 | ≥ 1 km/s | 8 | Rocket body | 5 |
| ≤ 25 km | 15 | below 1 | 0 | Space debris | 7 |

| Score | Level |
|---|---|
| ≥ 80 | CRITICAL |
| ≥ 55 | HIGH |
| ≥ 30 | MODERATE |
| < 30 | LOW |

Time of closest approach is reported and flagged "(imminent)" when within 30 min, but does not change the score. Object contributions reflect that debris cannot manoeuvre away. **These numbers are teaching choices, not official standards.**

## 14. Assumptions
Circular orbits · one shared 2D plane · Earth is a perfect sphere · only Earth's central gravity · objects never manoeuvre · exact positions (no measurement error) · objects are points (no size).

## 15. Limitations
Not 3D, no ellipses/inclination, no perturbations, no uncertainty/covariance, no probability of collision, no real tracking data. In one plane, objects on **different** altitudes in the **same** direction never get close, so the sample encounters use retrograde debris (which gives realistic ~15 km/s closing speeds). Results must never be used for real operations.

## 16–18. Installation, Compilation, Execution
Requirements: g++ with C++17 (g++ 9 or newer), optional `make`.
```bash
git clone https://github.com/<your-username>/SCOPE-Space-Collision-Orbital-Proximity-Evaluation.git
cd SCOPE-Space-Collision-Orbital-Proximity-Evaluation

# With make (Linux / macOS / MinGW):
make            # builds ./scope
./scope

# Without make (any platform):
g++ -std=c++17 -Wall -Wextra -Iinclude main.cpp src/*.cpp -o scope
./scope         # Windows: scope.exe

# Tests
make test
# or: g++ -std=c++17 -Iinclude tests/test_scope.cpp $(ls src/*.cpp | grep -v ConsoleUI) -o test_scope && ./test_scope
```
Run the program **from the project folder** so that `data/` and `reports/` are found. Quick demo: choose `11` (load sample data) → `6` (detect close approaches) → `7` (analyze) → `8` (report).

## 19. Sample Output
```
*** CLOSE APPROACH DETECTED ***
  Object 1          : SAT-001 (Satellite)
  Object 2          : DEB-001 (Space Debris)
  Minimum Distance  : 1.82 km
  Time of Approach  : 24.94 minutes  (imminent)
  Relative Velocity : 15.18 km/s
  Status            : HIGH RISK

Pair                Min (km)   TCA (min)       Rel v   Score  Risk
----------------------------------------------------------------------
SAT-002/DEB-002         0.60       13.70       15.02      94  CRITICAL
SAT-001/DEB-001         1.82       24.94       15.18      79  HIGH
SAT-003/DEB-003        12.00       26.61       14.90      49  MODERATE
SAT-003/DEB-002        99.40       48.45       14.96       0  LOW
...
```
Sample scenario (all **SIMULATED**): 3 satellites (SAT-001 550 km, SAT-002 700 km, SAT-003 800 km), 3 debris (DEB-001 551.82 km, DEB-002 700.6 km, DEB-003 812 km, all retrograde), 1 rocket body (RB-001 400 km). Result: 1 CRITICAL, 1 HIGH, 1 MODERATE, 18 LOW out of 21 pairs.

## 20. Testing
14 automated tests in `tests/test_scope.cpp`; all pass. Full table with inputs, expected and actual results: [`docs/TESTING.md`](docs/TESTING.md).

## 21. Future Scope
3D orbits with inclination and ellipses (Kepler elements) · perturbations (J2, drag) · reading real TLE data and SGP4 propagation · covariance and probability of collision · manoeuvre planning · a graphical (SFML/Qt) or 2D orbit-plot interface · unit-test framework (GoogleTest) · binary/JSON file formats.

## 22. Conclusion
SCOPE applies core OOP — abstraction, inheritance, polymorphism, encapsulation, templates, STL, exceptions and file handling — to a space-safety problem, in a tested, documented, modular program a single student can explain end to end.

## 23. Author
**<Your Name>** — B.E./B.Tech ECE, 2nd Year, Semester 3 · Course 204ES Object Oriented Programming · <College Name> · <Roll No.>

## Project Structure
```
SCOPE/
├── include/   (15 headers)      ├── data/      sample save files
├── src/       (14 sources)      ├── reports/   generated reports
├── tests/test_scope.cpp         ├── docs/      report, viva, slides, guides
├── main.cpp  Makefile  README.md  .gitignore  LICENSE
```
