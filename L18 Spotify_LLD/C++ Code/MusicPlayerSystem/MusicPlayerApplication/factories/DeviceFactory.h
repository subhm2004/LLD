// factories/DeviceFactory.h — FACTORY: DeviceType ke hisaab se sahi adapter
// (Bluetooth/Wired/Headphones) bana ke deta hai, uske API ko wrap karke.
// Object creation logic ek jagah centralized.
#ifndef DEVICE_FACTORY_HPP
#define DEVICE_FACTORY_HPP
#include<iostream>
#include "../device/IAudioOutputDevice.h"
#include "../device/BluetoothSpeakerAdapter.h"
#include "../device/WiredSpeakerAdapter.h"
#include "../device/HeadphonesAdapter.h"
#include "../enums/DeviceType.h"

using namespace std;

class DeviceFactory {
public:
    static IAudioOutputDevice* createDevice(DeviceType deviceType) {
        if (deviceType == DeviceType::BLUETOOTH) {
            return new BluetoothSpeakerAdapter(new BluetoothSpeakerAPI());
        } else if (deviceType == DeviceType::WIRED) {
            return new WiredSpeakerAdapter(new WiredSpeakerAPI());
        } else { // HEADPHONES
            return new HeadphonesAdapter(new HeadphonesAPI());
        }
    }
};
#endif // DEVICE_FACTORY_HPP

#