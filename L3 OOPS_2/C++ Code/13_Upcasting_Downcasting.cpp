#include <iostream>
#include <typeinfo>
using namespace std;

class Shape {
public:
    virtual void draw() const { cout << "Shape::draw\n"; }
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius = 1.0;
public:
    void draw() const override { cout << "Circle::draw r=" << radius << endl; }
    double area() const { return 3.14 * radius * radius; }
};

class Square : public Shape {
public:
    void draw() const override { cout << "Square::draw\n"; }
};

int main() {
    Circle c;
    Square s;

    cout << "=== Upcasting (Derived* → Base*) — implicit, safe ===\n";
    Shape* shapes[] = { &c, &s };
    for (Shape* sh : shapes) {
        sh->draw();  // runtime dispatch
    }

    Shape* up = &c;   // upcast — no cast syntax needed
    up->draw();

    cout << "\n=== Downcasting — use dynamic_cast (RTTI) ===\n";
    Shape* poly = &c;

    if (Circle* cp = dynamic_cast<Circle*>(poly)) {
        cout << "Downcast OK — Circle area=" << cp->area() << endl;
    }

    Shape* wrong = &s;
    if (dynamic_cast<Circle*>(wrong) == nullptr) {
        cout << "dynamic_cast<Circle*>(squarePtr) → nullptr (safe failure)\n";
    }

    cout << "\n=== Reference downcast — throws std::bad_cast on failure ===\n";
    try {
        Circle& cref = dynamic_cast<Circle&>(*poly);
        cout << "Ref downcast OK, area=" << cref.area() << endl;
    } catch (const bad_cast& e) {
        cout << "bad_cast: " << e.what() << endl;
    }

    try {
        (void)dynamic_cast<Circle&>(s);
    } catch (const bad_cast& e) {
        cout << "Expected bad_cast when Square → Circle&: " << e.what() << endl;
    }

    cout << "\nAvoid: static_cast<Circle*>(base) without proof — UB if wrong type.\n";
    cout << "Rule: downcast with dynamic_cast + check nullptr (pointer) or try/catch (ref).\n";
    return 0;
}
