// ============================================================================
//  enums/DeviceType.h — Output device types: BLUETOOTH / WIRED / HEADPHONES
// ----------------------------------------------------------------------------
//  DeviceFactory isse decide karta hai kaunsa adapter banana hai (aur
//  DeviceManager kaunsa "connected" message print kare). `enum class`
//  (scoped, type-safe). Naya device type add karna = ek value yahan +
//  factory me case + naya adapter/API.
// ============================================================================
#ifndef DEVICE_TYPE_HPP
#define DEVICE_TYPE_HPP

enum class DeviceType { 
    BLUETOOTH, 
    WIRED,
    HEADPHONES 
};

#endif // DEVICE_TYPE_HPP