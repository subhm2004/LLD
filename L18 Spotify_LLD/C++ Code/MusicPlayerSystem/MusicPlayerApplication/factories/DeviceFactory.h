// ============================================================================
//  factories/DeviceFactory.h — FACTORY (L9): sahi adapter banane ki dukaan 🏭
// ----------------------------------------------------------------------------
//  DeviceType enum do, sahi adapter (external API ke saath wrapped) lo.
//  DEKHO — yahan FACTORY + ADAPTER dono ek saath kaam karte hain:
//    new BluetoothSpeakerAdapter(new BluetoothSpeakerAPI())
//         ^^^^^^ adapter (target)      ^^^^^^ adaptee (external API)
//  Factory ne adapter ko uski API ke saath bana ke ready kar diya.
//
//  Fayda: DeviceManager ko concrete adapter/API classes ka pata hi nahi —
//  wo bas DeviceType bolta hai, factory sahi combo bana deti hai (loose
//  coupling). Naya device (jaise USB speaker) = naya case + adapter + API,
//  DeviceManager untouched.
//
//  Static method — factory ka koi state nahi, seedha createDevice() call.
// ============================================================================
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