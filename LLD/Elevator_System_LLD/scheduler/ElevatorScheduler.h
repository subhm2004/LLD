// ============================================================================
//  scheduler/ElevatorScheduler.h  —  SMART DISPATCH (Requirement #22)
// ----------------------------------------------------------------------------
//  Kaam ek hi: "passenger ne floor X se DIRECTION D me jaane ke liye bulaya —
//  kaunsa elevator bhejein?"
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PURANA CODE SIRF DISTANCE DEKHTA THA — aur wo kaafi nahi hai         │
//  │                                                                          │
//  │  Pehle scoring aisi thi:                                                 │
//  │      score = |elevator.floor − requestFloor|                             │
//  │      if (!elevator.isIdle()) score += 2;                                 │
//  │                                                                          │
//  │  Isme DIRECTION ka koi hisaab nahi tha! Ye galat elevator chun sakta:    │
//  │                                                                          │
//  │      Tum floor 5 pe ho, UP jaana hai.                                    │
//  │        Elevator A: floor 6 pe, UPAR ja raha hai   -> distance 1          │
//  │        Elevator B: floor 3 pe, UPAR ja raha hai   -> distance 2          │
//  │                                                                          │
//  │      Purana code A chunta (distance 1 kam hai). Par A tumse UPAR hai aur │
//  │      aur UPAR ja raha hai — wo tumhe chhod ke nikal gaya! Usko poora     │
//  │      chakkar lagana padega. B tumhari taraf aa raha hai — wo behtar hai. │
//  │                                                                          │
//  │  Requirement #22 saaf kehta hai: "based on WHERE the elevators are AND   │
//  │  WHERE THEY'RE GOING" — yaani position AUR direction, dono.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ NAYI SOCH — teen tarah ke elevator, teen priority:
//
//     1. IDLE elevator          -> sabse achha (free hai, turant aa sakta hai)
//     2. TUMHARI TARAF aa raha  -> achha (raaste me tumhe le lega)
//        aur SAME direction me jaana hai jo tum chahte ho
//     3. ULTI taraf ja raha     -> sabse bura (pehle apna kaam nipta ke aayega)
//
//  Ise "scoring" se karte hain: har elevator ko ek number do (jitna KAM utna
//  behtar), aur sabse kam wala chun lo.
//
//  📌 Ye SCHEDULER alag class kyun hai? (Strategy pattern ki taiyari)
//     Kyunki "kaunsa elevator bhejein" ka faisla kai tarah se ho sakta hai —
//     nearest-first, least-busy, energy-saving, VIP-priority. Aaj ek hai, kal
//     aur ho sakte hain. Alag class me hone se `ElevatorSystemController` ko
//     chhue bina naya scheduler plug kiya ja sakta hai.
//
//     (Abhi ye ek concrete class hai. Poora Strategy banane ke liye isko ek
//      interface bana ke controller me INJECT karna hoga — bilkul waise jaise
//      L33/L34 me kiya tha. Wo ek accha next-step exercise hai.)
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H
#define ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H

#include <bits/stdc++.h>

#include "../models/ElevatorCar.h"

using namespace std;

namespace elevator_lld {

class ElevatorScheduler {
public:
    // ---- Best elevator chuno (Requirement #22) -----------------------------
    //
    // `requestDirection` -> passenger ko KIS taraf jaana hai (UP/DOWN).
    // (Purane code me ye param tha hi nahi — isi liye wo direction ignore
    //  karta tha. Ab yahi is function ka dil hai.)
    //
    // Return: sabse achha elevator, ya `nullptr` agar koi hai hi nahi.
    ElevatorCar *selectBestElevator(const vector<ElevatorCar *> &elevators, int requestFloor,
                                    Direction requestDirection) {
        ElevatorCar *best = nullptr;
        int bestScore = numeric_limits<int>::max();

        for (ElevatorCar *elevator : elevators) {
            // ⭐ FULL elevator ko bhejna bekaar hai — koi chadh hi nahi payega
            // (Requirement #20). Isko skip kar do.
            if (elevator->isFull()) {
                continue;
            }

            int score = scoreFor(elevator, requestFloor, requestDirection);

            if (score < bestScore) {
                bestScore = score;
                best = elevator;
            }
        }
        return best; // saare full? -> nullptr (caller handle karega)
    }

private:
    // ---- Ek elevator ko score do — jitna KAM utna BEHTAR --------------------
    //
    // Score = distance + penalty
    //   distance -> kitni door hai (base cost)
    //   penalty  -> direction ke hisaab se extra "saza"
    int scoreFor(const ElevatorCar *elevator, int requestFloor, Direction requestDirection) const {
        const int elevatorFloor = elevator->getCurrentFloor();
        const Direction elevatorDir = elevator->getDirection();
        const int distance = abs(elevatorFloor - requestFloor);

        // ---- CASE 1: IDLE elevator — sabse achha ----
        // Free khada hai, koi commitment nahi. Bas distance ka cost.
        if (elevator->isIdle()) {
            return distance;
        }

        // ---- CASE 2: Elevator TUMHARI TARAF aa raha hai, aur uski direction
        //              wahi hai jo tum chahte ho -> raaste me tumhe le lega ----
        //
        // Do shart hain, dono zaroori:
        //   (a) uski direction == tumhari direction, AUR
        //   (b) wo tumhare "peeche" hai (yaani tum uske raaste me aate ho)
        //
        // UP wala case  : elevator floor 3 pe, UP ja raha, tum floor 5 pe UP
        //                 chahte ho -> wo 5 se hoke guzrega -> tumhe le lega ✅
        // DOWN wala case: elevator floor 9 pe, DOWN ja raha, tum floor 5 pe
        //                 DOWN chahte ho -> wo 5 se guzrega -> le lega ✅
        const bool comingTowardsUp = (elevatorDir == Direction::UP && requestDirection == Direction::UP &&
                                      elevatorFloor <= requestFloor);
        const bool comingTowardsDown = (elevatorDir == Direction::DOWN && requestDirection == Direction::DOWN &&
                                        elevatorFloor >= requestFloor);

        if (comingTowardsUp || comingTowardsDown) {
            // Chhoti si penalty (+1) — kyunki busy to hai, par raaste me hai.
            // IDLE se thoda kharab, ulti taraf wale se bahut behtar.
            return distance + 1;
        }

        // ---- CASE 3: Baaki sab — ULTI taraf ja raha, ya nikal chuka hai ----
        //
        // Isko bada penalty milega, kyunki usko pehle apna current kaam poora
        // karna padega, PHIR mudke tumhare paas aana padega.
        //
        // ⭐ Penalty distance se BADI honi chahiye (yahan +10), warna wo
        // distance ke fayde se dab jaayegi. Socho: ulti taraf ka elevator
        // distance 1 pe hai, sahi taraf wala distance 5 pe. Agar penalty sirf
        // +2 hoti to: 1+2=3 vs 5+1=6 -> galat wala jeet jaata! +10 se: 1+10=11
        // vs 5+1=6 -> sahi wala jeeta. ✅
        //
        // 📌 Penalty tuning me yahi asli baat hai: number ka SIZE mayne rakhta
        //    hai. Bahut chhota -> asar hi nahi. Bahut bada -> distance bekaar
        //    ho jaata. +10 is building (15 floors) ke liye theek hai.
        return distance + 10;
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H
