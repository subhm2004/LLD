#include <iostream>
using namespace std;

// Command Interface
class Command
{
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~Command() {}
};

// Receivers
class Light
{
public:
    void on()
    {
        cout << "Light is ON" << endl;
    }
    void off()
    {
        cout << "Light is OFF" << endl;
    }
};

class Fan
{
public:
    void on()
    {
        cout << "Fan is ON" << endl;
    }
    void off()
    {
        cout << "Fan is OFF" << endl;
    }
};

// Concrete Command for Light
class LightCommand : public Command
{
private:
    Light *light;

public:
    LightCommand(Light *l)
    {
        light = l;
    }

    void execute()
    {
        light->on();
    }

    void undo()
    {
        light->off();
    }
};

// Concrete Command for Fan
class FanCommand : public Command
{
private:
    Fan *fan;

public:
    FanCommand(Fan *f)
    {
        fan = f;
    }
    void execute()
    {
        fan->on();
    }
    void undo()
    {
        fan->off();
    }
};

/* --- Dynamic Invoker: RemoteController --- */
class RemoteController
{
private:
    // Dynamic 2D structure: rows contain columns of commands
    vector<vector<Command *>> buttons;
    vector<vector<bool>> buttonPressed;

public:
    // Constructor handles dynamic grid size
    RemoteController(int rows, int cols)
    {
        buttons.resize(rows, vector<Command *>(cols, nullptr));
        buttonPressed.resize(rows, vector<bool>(cols, false));
    }

    // Command set karne ke liye
    void setCommand(int row, int col, Command *cmd)
    {
        if (row < buttons.size() && col < buttons[row].size())
        {
            if (buttons[row][col] != nullptr)
                delete buttons[row][col];

            buttons[row][col] = cmd;
            buttonPressed[row][col] = false;
        }
    }

    // Button press logic
    void pressButton(int row, int col)
    {
        if (row < buttons.size() && col < buttons[row].size() && buttons[row][col] != nullptr)
        {
            if (buttonPressed[row][col] == false)
            {
                buttons[row][col]->execute();
            }
            else
            {
                buttons[row][col]->undo();
            }
            buttonPressed[row][col] = !buttonPressed[row][col];
        }
        else
        {
            cout << "Invalid button or no command at [" << row << "][" << col << "]\n";
        }
    }

    // Destructor to clean up all dynamic commands
    ~RemoteController()
    {
        for (auto &row : buttons)
        {
            for (auto &cmd : row)
            {
                if (cmd != nullptr)
                    delete cmd;
            }
        }
    }
};

int main()
{
    // 1. Receivers create karein
    Light *livingRoomLight = new Light();
    Fan *ceilingFan = new Fan();

    // 2. RemoteController ka object banayein (Example: 2 rows aur 2 columns ka grid)
    // Constructor ab (rows, cols) leta hai
    RemoteController *remote = new RemoteController(2, 2);

    // 3. Commands set karein (Ab row aur column index dena hoga)
    // Row 0, Col 0 par Light ka command
    remote->setCommand(0, 0, new LightCommand(livingRoomLight));

    // Row 0, Col 1 par Fan ka command
    remote->setCommand(0, 1, new FanCommand(ceilingFan));

    // 4. Simulate button presses (Toggle behavior)
    cout << "--- Toggling Light Button [0][0] ---" << endl;
    remote->pressButton(0, 0); // ON
    remote->pressButton(0, 0); // OFF

    cout << "--- Toggling Fan Button [0][1] ---" << endl;
    remote->pressButton(0, 1); // ON
    remote->pressButton(0, 1); // OFF

    // 5. Invalid ya Unassigned button press karke check karein
    cout << "--- Pressing Unassigned Button [1][1] ---" << endl;
    remote->pressButton(1, 1);

    // 6. Clean up
    // Note: RemoteController ka destructor khud commands delete kar dega
    delete remote;
    delete livingRoomLight;
    delete ceilingFan;

    return 0;
}
