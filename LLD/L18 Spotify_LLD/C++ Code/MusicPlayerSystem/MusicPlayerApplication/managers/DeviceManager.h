// ============================================================================
//  managers/DeviceManager.h — SINGLETON: connected output device ka manager
// ----------------------------------------------------------------------------
//  Ek waqt me ek hi output device active hota hai (jaise phone me ek
//  bluetooth speaker connected) — DeviceManager usse track karta hai.
//  connect() DeviceFactory se naya device banwata hai (purana delete karke),
//  getOutputDevice() se engine use leta hai.
//
//  SINGLETON kyun: poore app me ek hi "currently connected device" state.
//  DHYAN DO — ye MEYERS-ISH style hai (getInstance ke andar `static local`
//  pointer + null check) — thodi mixed hai. Baaki managers (Playlist/Strategy)
//  static member pointer style use karte hain. Ek hi project me alag styles
//  (L10 me detail); consistency ke liye sabko pure Meyers karna better.
//
//  ⭐ connect() me OLD device delete karta hai naya banane se pehle — device
//  switch pe memory leak nahi (achhi practice). Adapter+API dono ka
//  ownership yahin hai.
// ============================================================================
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