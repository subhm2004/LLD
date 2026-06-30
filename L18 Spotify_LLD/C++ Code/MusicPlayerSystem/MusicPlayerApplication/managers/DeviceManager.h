// managers/DeviceManager.h — SINGLETON: currently connected output device ko
// manage karta hai (connect/switch). Factory se device banwa ke engine ko deta.
# ifndef DEVICE_MANAGER_HPP
# define DEVICE_MANAGER_HPP
#include<iostream>
#include "../device/IAudioOutputDevice.h"
#include "../enums/DeviceType.h"
#include "../factories/DeviceFactory.h"

using namespace std;

class DeviceManager {
private:
    IAudioOutputDevice* currentOutputDevice;
    DeviceManager() {
        currentOutputDevice = nullptr;
    }
public:
    static DeviceManager* getInstance() {
        static DeviceManager* instance = nullptr;
        if (instance == nullptr) {
            instance = new DeviceManager();
        }
        return instance;
    }
    void connect(DeviceType deviceType) {
        if (currentOutputDevice) {
            delete currentOutputDevice;
        }

        currentOutputDevice = DeviceFactory::createDevice(deviceType);

        switch(deviceType) {
            case DeviceType::BLUETOOTH:
                cout<< "Bluetooth device connected \n";
                break;
            case DeviceType::WIRED:
                cout<< "Wired device connected \n";
                break;
            case DeviceType::HEADPHONES:
                cout<< "Headphones connected \n";
        }
    }

    IAudioOutputDevice* getOutputDevice() {
        if (!currentOutputDevice) {
            throw runtime_error("No output device is connected.");
        }
        return currentOutputDevice;
    }

    bool hasOutputDevice() {
        return currentOutputDevice != nullptr;
    }
};

#endif // DEVICE_MANAGER_HPP