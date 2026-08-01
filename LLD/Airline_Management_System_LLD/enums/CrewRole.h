// ============================================================================
//  enums/CrewRole.h  —  Crew member ka role (flight chalane wale log)
// ----------------------------------------------------------------------------
//     PILOT      -> flight uda-ne wala (cockpit)
//     CO_PILOT   -> pilot ka partner/backup (cockpit)
//     CABIN_CREW -> passengers ki dekh-bhaal (air hostess/steward)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MINIMUM CREW RULE — flight tabhi book khulti hai jab crew poori ho    │
//  │                                                                          │
//  │  Business rule: booking allow karne ke liye kam se kam                    │
//  │     - ek "cockpit" wala (PILOT ya CO_PILOT), AUR                          │
//  │     - ek CABIN_CREW                                                       │
//  │  hona chahiye. CrewAssignmentService::hasMinimumCrew() yahi check karti   │
//  │  hai. Bina crew ke flight ud nahi sakti, to booking bhi nahi khulti.     │
//  │                                                                          │
//  │  💡 Notice: hasMinimumCrew me PILOT aur CO_PILOT dono "cockpit" gine      │
//  │     jaate hain — yaani ek CO_PILOT bhi minimum requirement poori kar deta.│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_CREWROLE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_CREWROLE_H

namespace airline_mgmt {

enum class CrewRole { PILOT, CO_PILOT, CABIN_CREW };

} // namespace airline_mgmt

#endif
