/*
 Scope - Space Collision & Orbital Proximity Evaluation System
 Compile: g++ -std=c++17 Scope_final.cpp -o Scope.exe
*/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <limits>

using namespace std;
namespace fs = std::filesystem;

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371.0;
const double EARTH_MU = 398600.4418;
const double CLOSE_DISTANCE = 25.0;

// One structure stores the details of a satellite, debris, or rocket body.
struct SpaceObject {
    string id, name, type, source;
    double altitude = 0, angle = 0, mass = 0, sizeCm = 0;
    int direction = 1;
    bool active = true;
};

struct Position {
    double x, y;
};

struct Result {
    string idA, idB, typeA, typeB, risk = "LOW";
    double distance = 0, timeSec = 0, relativeSpeed = 0;
    double distanceScore = 0, speedScore = 0, objectScore = 0, totalScore = 0;
};

vector<SpaceObject> objects;
double durationMinutes = 120.0;
double stepSeconds = 1.0;

// ---------- Basic orbital calculations ----------
double radians(double degrees) {
    return degrees * PI / 180.0;
}

double orbitRadius(const SpaceObject& obj) {
    return EARTH_RADIUS + obj.altitude;
}

double orbitSpeed(const SpaceObject& obj) {
    return sqrt(EARTH_MU / orbitRadius(obj));
}

double angularSpeed(const SpaceObject& obj) {
    return orbitSpeed(obj) / orbitRadius(obj);
}

double angleAt(const SpaceObject& obj, double timeSec) {
    return radians(obj.angle) + obj.direction * angularSpeed(obj) * timeSec;
}

Position positionAt(const SpaceObject& obj, double timeSec) {
    double angle = angleAt(obj, timeSec);
    return {orbitRadius(obj) * cos(angle), orbitRadius(obj) * sin(angle)};
}

Position velocityAt(const SpaceObject& obj, double timeSec) {
    double angle = angleAt(obj, timeSec);
    double speed = obj.direction * orbitSpeed(obj);
    return {-sin(angle) * speed, cos(angle) * speed};
}

double distanceBetween(const Position& a, const Position& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

double distanceAt(const SpaceObject& a, const SpaceObject& b, double timeSec) {
    return distanceBetween(positionAt(a, timeSec), positionAt(b, timeSec));
}

int findObject(const string& id) {
    for (int i = 0; i < static_cast<int>(objects.size()); ++i)
        if (objects[i].id == id) return i;
    return -1;
}

string upperCase(string text) {
    for (char& c : text) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    return text;
}

// ---------- Input helpers ----------
string askText(const string& prompt) {
    string value;
    do {
        cout << prompt;
        getline(cin >> ws, value);
        if (!value.empty()) return value;
        cout << "Input cannot be empty. Try again.\n";
    } while (true);
}

double askDouble(const string& prompt, double low, double high) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= low && value <= high) return value;
        cout << "Invalid value. Enter a number between " << low << " and " << high << ".\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

int askInt(const string& prompt, int low, int high) {
    return static_cast<int>(askDouble(prompt, low, high));
}

SpaceObject readOrbitDetails(string id, string name, string type) {
    SpaceObject obj;
    obj.id = upperCase(id);
    obj.name = name;
    obj.type = type;
    obj.altitude = askDouble("Altitude above Earth (km): ", 160, 40000);
    obj.angle = askDouble("Starting angle (degrees): ", 0, 360);
    int dir = askInt("Direction  1) Prograde  2) Retrograde: ", 1, 2);
    obj.direction = (dir == 1) ? 1 : -1;
    return obj;
}

// ---------- Object management ----------
void addSatellite() {
    cout << "\n--- Add Satellite ---\n";
    string id = upperCase(askText("Object ID (e.g. SAT-004): "));
    if (findObject(id) != -1) { cout << "\n[ERROR] Duplicate object ID: " << id << "\n"; return; }
    string name = askText("Name: ");
    SpaceObject obj = readOrbitDetails(id, name, "Satellite");
    obj.mass = askDouble("Mass (kg): ", 0.001, 1e9);
    int active = askInt("Status  1) Active  2) Inactive: ", 1, 2);
    obj.active = (active == 1);
    objects.push_back(obj);
    cout << "Satellite " << id << " added.\n";
}

void addDebris() {
    cout << "\n--- Add Space Debris ---\n";
    string id = upperCase(askText("Object ID (e.g. DEB-004): "));
    if (findObject(id) != -1) { cout << "\n[ERROR] Duplicate object ID: " << id << "\n"; return; }
    string name = askText("Name: ");
    SpaceObject obj = readOrbitDetails(id, name, "Space Debris");
    obj.sizeCm = askDouble("Approximate size/diameter (cm): ", 0.01, 100000);
    obj.source = askText("Source/description: ");
    double radiusM = obj.sizeCm / 200.0;
    obj.mass = (4.0 / 3.0) * PI * radiusM * radiusM * radiusM * 2700.0;
    objects.push_back(obj);
    cout << "Space debris " << id << " added.\n";
}

void addRocketBody() {
    cout << "\n--- Add Rocket Body ---\n";
    string id = upperCase(askText("Object ID (e.g. RB-002): "));
    if (findObject(id) != -1) { cout << "\n[ERROR] Duplicate object ID: " << id << "\n"; return; }
    string name = askText("Name: ");
    SpaceObject obj = readOrbitDetails(id, name, "Rocket Body");
    obj.mass = askDouble("Mass (kg): ", 0.001, 1e9);
    objects.push_back(obj);
    cout << "Rocket body " << id << " added.\n";
}

void viewObjects() {
    if (objects.empty()) { cout << "\n[ERROR] No orbital objects registered yet. Add some or load the sample data.\n"; return; }
    cout << "\n--- Registered Orbital Objects (" << objects.size() << ") ---\n";
    cout << left << setw(10) << "ID" << setw(16) << "Type" << setw(20) << "Name"
         << right << setw(12) << "Alt (km)" << setw(12) << "Mass (kg)" << "\n"
         << string(70, '-') << "\n";
    for (const auto& obj : objects)
        cout << left << setw(10) << obj.id << setw(16) << obj.type << setw(20) << obj.name
             << right << fixed << setprecision(2) << setw(12) << obj.altitude
             << setw(12) << obj.mass << "\n";

    cout << "\nEnter an ID for full details (or press Enter to skip): ";
    string id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, id);
    if (!id.empty()) {
        int i = findObject(upperCase(id));
        if (i == -1) { cout << "\n[ERROR] Object not found: " << id << "\n"; return; }
        const auto& o = objects[i];
        cout << "\nID: " << o.id << "\nName: " << o.name << "\nType: " << o.type
             << "\nAltitude: " << o.altitude << " km\nStarting angle: " << o.angle
             << " degrees\nDirection: " << (o.direction == 1 ? "Prograde" : "Retrograde")
             << "\nOrbital speed: " << orbitSpeed(o) << " km/s\nOrbital period: "
             << (2 * PI / angularSpeed(o)) / 60.0 << " minutes\nMass: " << o.mass << " kg\n";
        if (o.type == "Satellite") cout << "Status: " << (o.active ? "Active" : "Inactive") << "\n";
        if (o.type == "Space Debris") cout << "Size: " << o.sizeCm << " cm\nSource: " << o.source << "\n";
    }
}

// ---------- Simulation and close approach detection ----------
void printPositions(double timeSec) {
    cout << "  Positions at t = " << fixed << setprecision(1) << timeSec / 60.0 << " min\n"
         << "  " << left << setw(10) << "ID" << right << setw(14) << "x (km)"
         << setw(14) << "y (km)" << setw(14) << "angle (deg)" << "\n";
    for (const auto& obj : objects) {
        Position p = positionAt(obj, timeSec);
        double angle = fmod(angleAt(obj, timeSec) * 180.0 / PI, 360.0);
        if (angle < 0) angle += 360;
        cout << "  " << left << setw(10) << obj.id << right << setprecision(2)
             << setw(14) << p.x << setw(14) << p.y << setw(14) << angle << "\n";
    }
}

void simulateOrbits() {
    if (objects.empty()) { cout << "\n[ERROR] No orbital objects to simulate.\n"; return; }
    cout << "\n--- Simulate Orbits ---\n";
    double start = askDouble("Start time (minutes): ", 0, 100000);
    double interval = askDouble("Interval between snapshots (minutes): ", 0.1, 10000);
    int count = askInt("Number of snapshots (1-10): ", 1, 10);
    for (int i = 0; i < count; ++i) {
        cout << "\n";
        printPositions((start + i * interval) * 60);
    }
}

Result closestApproach(const SpaceObject& a, const SpaceObject& b) {
    double bestDistance = 1e100, bestTime = 0;
    double endTime = durationMinutes * 60;
    int steps = max(1, static_cast<int>(endTime / stepSeconds));
    for (int i = 0; i <= steps; ++i) {
        double t = min(endTime, i * stepSeconds);
        double d = distanceAt(a, b, t);
        if (d < bestDistance) { bestDistance = d; bestTime = t; }
    }

    Position va = velocityAt(a, bestTime), vb = velocityAt(b, bestTime);
    double dx = va.x - vb.x, dy = va.y - vb.y;
    Result r;
    r.idA = a.id; r.idB = b.id; r.typeA = a.type; r.typeB = b.type;
    r.distance = bestDistance; r.timeSec = bestTime;
    r.relativeSpeed = sqrt(dx * dx + dy * dy);
    return r;
}

vector<Result> allPairs() {
    vector<Result> results;
    for (size_t i = 0; i < objects.size(); ++i)
        for (size_t j = i + 1; j < objects.size(); ++j)
            results.push_back(closestApproach(objects[i], objects[j]));
    sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
        return a.distance < b.distance;
    });
    return results;
}

double objectRisk(const SpaceObject& obj) {
    if (obj.type == "Space Debris") return 15;
    if (obj.type == "Rocket Body") return 12;
    if (obj.type == "Satellite" && !obj.active) return 8;
    return 0;
}

void assessRisk(Result& r) {
    if (r.distance <= 1) r.distanceScore = 60;
    else if (r.distance <= 5) r.distanceScore = 45;
    else if (r.distance <= 10) r.distanceScore = 30;
    else if (r.distance <= CLOSE_DISTANCE) r.distanceScore = 15;

    if (r.relativeSpeed >= 10) r.speedScore = 25;
    else if (r.relativeSpeed >= 5) r.speedScore = 15;
    else if (r.relativeSpeed >= 1) r.speedScore = 8;

    int a = findObject(r.idA), b = findObject(r.idB);
    if (a >= 0 && b >= 0) r.objectScore = objectRisk(objects[a]) + objectRisk(objects[b]);
    r.totalScore = r.distanceScore + r.speedScore + r.objectScore;

    if (r.distance >= CLOSE_DISTANCE) r.risk = "LOW";
    else if (r.totalScore >= 80) r.risk = "CRITICAL";
    else if (r.totalScore >= 55) r.risk = "HIGH";
    else if (r.totalScore >= 30) r.risk = "MODERATE";
    else r.risk = "LOW";
}

void showResult(const Result& r, bool breakdown = false) {
    cout << "Objects: " << r.idA << " (" << r.typeA << ") and " << r.idB << " (" << r.typeB << ")\n"
         << "Minimum distance: " << fixed << setprecision(2) << r.distance << " km\n"
         << "Time of closest approach: " << r.timeSec / 60.0 << " minutes\n"
         << "Relative velocity: " << r.relativeSpeed << " km/s\n"
         << "Risk level: " << r.risk << "\n";
    if (breakdown)
        cout << "Distance score: " << r.distanceScore << "\nVelocity score: " << r.speedScore
             << "\nObject score: " << r.objectScore << "\nTotal score: " << r.totalScore << "\n";
}

void detectCloseApproaches() {
    if (objects.size() < 2) { cout << "\n[ERROR] At least two objects are needed.\n"; return; }
    cout << "\nScanning every pair for " << durationMinutes << " min (step " << stepSeconds << " s)...\n";
    auto results = allPairs();
    int found = 0;
    for (auto r : results) {
        if (r.distance >= CLOSE_DISTANCE) continue;
        assessRisk(r);
        ++found;
        cout << "\n*** CLOSE APPROACH DETECTED ***\n";
        showResult(r);
    }
    cout << "\n" << found << " of " << results.size() << " pairs came closer than "
         << CLOSE_DISTANCE << " km.\n";
    if (found == 0) cout << "All pairs stayed safely separated.\n";
    if (!results.empty())
        cout << "Closest pair overall: " << results[0].idA << " & " << results[0].idB
             << " at " << fixed << setprecision(2) << results[0].distance << " km.\n";
}

void analyzeRisk() {
    if (objects.size() < 2) { cout << "\n[ERROR] At least two objects are needed for risk analysis.\n"; return; }
    cout << "\n--- Analyze Collision Risk ---\n  1) One pair\n  2) All pairs\n";
    int mode = askInt("Choice: ", 1, 2);
    if (mode == 1) {
        string a = upperCase(askText("First object ID : "));
        string b = upperCase(askText("Second object ID: "));
        if (a == b || findObject(a) == -1 || findObject(b) == -1) {
            cout << "\n[ERROR] Please enter IDs of two different existing objects.\n"; return;
        }
        Result r = closestApproach(objects[findObject(a)], objects[findObject(b)]);
        assessRisk(r);
        cout << "\n"; showResult(r, true);
    } else {
        auto results = allPairs();
        cout << "\n" << left << setw(18) << "Pair" << right << setw(12) << "Min (km)"
             << setw(12) << "TCA (min)" << setw(12) << "Rel v" << setw(8) << "Score" << "  Risk\n"
             << string(70, '-') << "\n";
        for (auto r : results) {
            assessRisk(r);
            cout << left << setw(18) << (r.idA + "/" + r.idB) << right << fixed << setprecision(2)
                 << setw(12) << r.distance << setw(12) << r.timeSec / 60
                 << setw(12) << r.relativeSpeed << setw(8) << setprecision(0) << r.totalScore
                 << "  " << r.risk << "\n";
        }
    }
}

// ---------- Sample data, file handling and reports ----------
void loadSampleData() {
    objects.clear();
    objects.push_back({"SAT-001", "EduSat-1", "Satellite", "", 550, 0, 250, 0, 1, true});
    objects.push_back({"SAT-002", "EduSat-2", "Satellite", "", 700, 90, 420, 0, 1, true});
    objects.push_back({"SAT-003", "WeatherSim-3", "Satellite", "", 800, 200, 1200, 0, 1, true});
    objects.push_back({"DEB-001", "Fragment-A", "Space Debris", "Simulated fragmentation", 551.82, 188, 0, 12, -1, true});
    objects.push_back({"DEB-002", "Fragment-B", "Space Debris", "Simulated fragmentation", 700.60, 190, 0, 8, -1, true});
    objects.push_back({"DEB-003", "Fragment-C", "Space Debris", "Simulated paint/bolt debris", 812, 30, 0, 20, -1, true});
    objects.push_back({"RB-001", "SimStage-1", "Rocket Body", "", 400, 45, 2100, 0, 1, true});
    cout << "\nSIMULATED sample data loaded (" << objects.size() << " objects).\nCurrent objects were replaced.\n";
}

void saveData() {
    if (objects.empty()) { cout << "\n[ERROR] Nothing to save: no objects registered.\n"; return; }
    fs::create_directories("data");
    ofstream sat("data/satellites.txt"), debris("data/debris.txt"), rocket("data/rocketbodies.txt");
    if (!sat || !debris || !rocket) { cout << "\n[ERROR] Could not save data files.\n"; return; }
    sat << "# id|name|altitude|angle|direction|mass|active\n";
    debris << "# id|name|altitude|angle|direction|size_cm|source\n";
    rocket << "# id|name|altitude|angle|direction|mass\n";
    for (const auto& o : objects) {
        if (o.type == "Satellite") sat << o.id << "|" << o.name << "|" << o.altitude << "|" << o.angle << "|" << o.direction << "|" << o.mass << "|" << o.active << "\n";
        else if (o.type == "Space Debris") debris << o.id << "|" << o.name << "|" << o.altitude << "|" << o.angle << "|" << o.direction << "|" << o.sizeCm << "|" << o.source << "\n";
        else rocket << o.id << "|" << o.name << "|" << o.altitude << "|" << o.angle << "|" << o.direction << "|" << o.mass << "\n";
    }
    cout << "\nSaved " << objects.size() << " objects to folder 'data'.\n";
}

void loadData() {
    vector<SpaceObject> loaded;
    auto readFile = [&](string filename, string type) {
        ifstream file("data/" + filename);
        string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            stringstream ss(line);
            vector<string> fields;
            string field;
            while (getline(ss, field, '|')) fields.push_back(field);
            try {
                if (fields.size() < 6) continue;
                SpaceObject o;
                o.id = fields[0]; o.name = fields[1]; o.type = type;
                o.altitude = stod(fields[2]); o.angle = stod(fields[3]); o.direction = stoi(fields[4]);
                if (type == "Space Debris") { o.sizeCm = stod(fields[5]); if (fields.size() > 6) o.source = fields[6]; o.mass = (4.0/3.0)*PI*pow(o.sizeCm/200.0,3)*2700.0; }
                else { o.mass = stod(fields[5]); if (type == "Satellite" && fields.size() > 6) o.active = (fields[6] == "1"); }
                loaded.push_back(o);
            } catch (...) {}
        }
    };
    readFile("satellites.txt", "Satellite");
    readFile("debris.txt", "Space Debris");
    readFile("rocketbodies.txt", "Rocket Body");
    if (loaded.empty()) { cout << "\n[ERROR] No data files found in folder 'data'. Save data first.\n"; return; }
    objects = loaded;
    cout << "\nLoaded " << objects.size() << " objects (0 skipped).\n";
}

string timestamp(const char* format) {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), format, t);
    return buffer;
}

void generateReport() {
    if (objects.size() < 2) { cout << "\n[ERROR] At least two objects are needed for risk analysis.\n"; return; }
    auto results = allPairs();
    for (auto& r : results) assessRisk(r);
    sort(results.begin(), results.end(), [](const Result& a, const Result& b) { return a.totalScore > b.totalScore; });

    int counts[4] = {0, 0, 0, 0};
    for (const auto& r : results) {
        if (r.risk == "LOW") counts[0]++;
        else if (r.risk == "MODERATE") counts[1]++;
        else if (r.risk == "HIGH") counts[2]++;
        else counts[3]++;
    }

    ostringstream report;
    report << "==================================================================\n"
           << " Scope - COLLISION RISK REPORT\n"
           << " Space Collision & Orbital Proximity Evaluation System\n"
           << "==================================================================\n"
           << " Generated        : " << timestamp("%Y-%m-%d %H:%M:%S") << "\n"
           << " DISCLAIMER       : EDUCATIONAL SIMULATION. Uses simulated data and a\n"
           << "                    simplified 2D circular-orbit model. Risk levels use\n"
           << "                    project-defined thresholds, NOT official standards.\n"
           << " Objects analysed : " << objects.size() << "\n"
           << " Simulated window : " << durationMinutes << " min, step " << stepSeconds << " s\n"
           << " Pairs analysed   : " << results.size() << "\n\n"
           << " SUMMARY\n   CRITICAL : " << counts[3] << "\n   HIGH     : " << counts[2]
           << "\n   MODERATE : " << counts[1] << "\n   LOW      : " << counts[0]
           << "\n\n DETAILED RESULTS (most severe first)\n"
           << "------------------------------------------------------------------\n";

    int number = 1;
    for (const auto& r : results) {
        report << " #" << number++ << (r.distance < CLOSE_DISTANCE ? "  CLOSE APPROACH DETECTED" : "  no close approach") << "\n"
               << "Objects: " << r.idA << " (" << r.typeA << ") and " << r.idB << " (" << r.typeB << ")\n"
               << "Minimum distance: " << fixed << setprecision(2) << r.distance << " km\n"
               << "Time of closest approach: " << r.timeSec / 60.0 << " minutes\n"
               << "Relative velocity: " << r.relativeSpeed << " km/s\n"
               << "Risk level: " << r.risk << "\n"
               << "Distance score: " << r.distanceScore << "\nVelocity score: " << r.speedScore
               << "\nObject score: " << r.objectScore << "\nTotal score: " << r.totalScore << "\n"
               << "------------------------------------------------------------------\n";
    }
    report << " End of report.\n";

    // Save the report safely and show a useful message if the folder/file cannot be created.
    error_code folderError;
    fs::create_directories("reports", folderError);
    if (folderError) {
        cout << "\n[ERROR] Could not create the 'reports' folder: "
             << folderError.message() << "\nThe report is shown below but was not saved.\n";
        cout << "\n" << report.str() << "\n";
        return;
    }

    string path = string("reports/risk_report_") + timestamp("%Y%m%d_%H%M%S") + ".txt";
    ofstream file(path);
    if (!file.is_open()) {
        cout << "\n[ERROR] Could not open the report file for writing: " << path
             << "\nCheck folder permissions or run the program from a writable folder.\n";
        cout << "\n" << report.str() << "\n";
        return;
    }
    file << report.str();
    file.close();
    if (!file) {
        cout << "\n[ERROR] A problem occurred while writing the report file: " << path << "\n";
        cout << "\n" << report.str() << "\n";
        return;
    }
    cout << "\n" << report.str() << "\nReport saved to: " << path << "\n";
}

void changeSettings() {
    cout << "\n--- Simulation Settings ---\nCurrent duration: " << durationMinutes
         << " min, step " << stepSeconds << " s\n";
    durationMinutes = askDouble("New duration (minutes): ", 0.1, 1440);
    stepSeconds = askDouble("New time step (seconds): ", 0.01, 60);
    cout << "Settings updated.\n";
}

// ---------- Main menu ----------
void showBanner() {
    cout << "\n=============================================\n"
         << "                  SCOPE\n"
         << "      SPACE COLLISION & ORBITAL\n"
         << "     PROXIMITY EVALUATION SYSTEM\n"
         << "=============================================\n"
         ;
}

void showMenu() {
    cout << "\n---------------------------------------------\n"
         << "  Objects loaded: " << objects.size() << "\n"
         << "---------------------------------------------\n"
         << "  1. Add Satellite\n"
         << "  2. Add Space Debris\n"
         << "  3. Add Rocket Body\n"
         << "  4. View Orbital Objects\n"
         << "  5. Simulate Orbits\n"
         << "  6. Detect Close Approaches\n"
         << "  7. Analyze Collision Risk\n"
         << "  8. Generate Risk Report\n"
         << "  9. Save Data\n"
         << " 10. Load Data\n"
         << " 11. Load Sample Data (SIMULATED)\n"
         << " 12. Simulation Settings\n"
         << "  0. Exit\n";
}

int main() {
    showBanner();
    bool running = true;
    while (running) {
        showMenu();
        int choice = askInt("Enter your choice: ", 0, 12);
        switch (choice) {
            case 1: addSatellite(); break;
            case 2: addDebris(); break;
            case 3: addRocketBody(); break;
            case 4: viewObjects(); break;
            case 5: simulateOrbits(); break;
            case 6: detectCloseApproaches(); break;
            case 7: analyzeRisk(); break;
            case 8: generateReport(); break;
            case 9: saveData(); break;
            case 10: loadData(); break;
            case 11: loadSampleData(); break;
            case 12: changeSettings(); break;
            case 0: running = false; break;
        }
    }
    cout << "\nThank you for using Scope. Goodbye!\n";
    return 0;
}
