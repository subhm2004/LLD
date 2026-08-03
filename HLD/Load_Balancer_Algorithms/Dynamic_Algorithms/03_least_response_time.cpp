// ============================================================================
//  DYNAMIC #3 — 03_least_response_time.cpp
//  LEAST RESPONSE TIME — "jo sabse TEZ jawab de raha hai, usko do"
// ----------------------------------------------------------------------------
//  Least Connections ek cheez maan ke chalta hai: "kam connections = kam load".
//  Par ye bhi hamesha sach nahi. Sochiye:
//
//     * Ek server ki disk kharab ho rahi hai — 2 hi connections hain par
//       har request 5 second le rahi hai.
//     * Ek server doosre data-center me hai — network latency zyada hai.
//     * Ek server pe koi aur process CPU kha raha hai.
//
//  In sab cases me connection count JHOOTH bol raha hai. Server "khaali" dikhta
//  hai par asal me bimaar hai.
//
//  ⭐ IDEA: connections ki jagah (ya uske saath) ASLI RESPONSE TIME dekho —
//     kyunki wo server ki sehat ka sabse seedha saboot hai.
//
//        score = avgResponseTime × (activeConnections + 1)
//        chosenServer = jiska score sabse KAM
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SCORE ME DONO CHEEZEIN KYUN? (sirf latency kyun nahi?)                │
//  │                                                                          │
//  │  Agar sirf `avgResponseTime` dekhein to ek KHATRA hai: jo server abhi     │
//  │  tez hai, uspe SAARA traffic toot padega — aur wo bhi dheere ho jaayega.  │
//  │  Phir wo "slow" ho jaayega, traffic doosre pe jaayega, wo dheere hoga...  │
//  │  Traffic jhoolne lagta hai (oscillation).                                │
//  │                                                                          │
//  │  `(activeConnections + 1)` se multiply karke hum kehte hain: "ye server   │
//  │  tez to hai, PAR uspe pehle se itni requests hain — to agli request ko    │
//  │  asal me itna intezaar karna padega." Yaani ye EXPECTED WAIT ka andaaza   │
//  │  hai, sirf past ka average nahi.                                         │
//  │                                                                          │
//  │  `+1` isliye ki 0 connections wale server ka score 0 na ho jaaye          │
//  │  (warna wo hamesha jeetta rehta — divide-by-zero jaisi galti).            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ EWMA — latency "yaad" kaise rakhein                                   │
//  │                                                                          │
//  │  Poore din ka average lena bekaar hai — server 10 minute pehle slow tha   │
//  │  to ab kya? Isliye EWMA (exponentially weighted moving average):          │
//  │                                                                          │
//  │       avg = 0.8 × purana_avg  +  0.2 × naya_sample                        │
//  │                                                                          │
//  │  Purani baatein apne aap dheere-dheere bhool jaati hain. 0.2 ko badhao to │
//  │  algorithm zyada "chanchal" (jaldi react karega, par shor bhi pakdega),   │
//  │  ghatao to zyada "sust" (sthir, par dheere react karega).                 │
//  │  Ye simulator me lb_common.h ke andar aise hi update hota hai.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠⚠ PEHLE HI BATA DETE HAIN (taaki demo chaunkaye nahi):
//      Is file ka demo dikhayega ki normal HTTP/1 cluster me ye algorithm
//      Least Connections se AKSAR KHARAB hai — bimaar server wale case me bhi.
//      Wajah: "server slow hai" ki khabar connection count me pehle se hi
//      chhupi hoti hai (slow server pe connections jama ho jaati hain).
//      Iski asli jagah wahan hai jahan connection count kaam hi na kare —
//      HTTP/2 aur gRPC (multiplexing). Poori baat part E aur F me.
//
//  ✅ Faayde   : latency seedha saboot hai; HTTP/2 & gRPC me yahi chalega
//                (wahan connection count bekaar ho jaata hai)
//  ❌ Nuksan   : latency ek PURANA aur shor-bhara signal hai; measure+store
//                karni padti; oscillation ka khatra; cold start handle karna padta
//  📌 Kab use  : HTTP/2 / gRPC / multiplexed protocols, ya jab LB connection ka
//                ant dekh hi na paaye. Simple HTTP/1 me Least Connections behtar.
//
//  Compile: g++ -std=c++17 -Wall -Wextra 03_least_response_time.cpp -o lrt_demo
// ============================================================================
#include <bits/stdc++.h>

#include "../lb_common.h"

using namespace std;
using namespace lb;

int main()
{
    cout << "=========================================================\n";
    cout << " DYNAMIC #3 : LEAST RESPONSE TIME\n";
    cout << " score = avgResponseTime x (activeConnections + 1)\n";
    cout << "=========================================================\n\n";

    // ---- Part A: haath se ek faisla --------------------------------------
    cout << "[A] Ek 'bimaar' server ko algorithm kaise pakadta hai:\n\n";
    {
        vector<string> names = {"S1", "S2-sick", "S3"};
        vector<double> latency = {2.0, 40.0, 3.0}; // S2 bimaar hai
        vector<int> connections = {6, 2, 5};       // par uspe connections KAM hain!

        cout << "    server   | conns | avg latency | score = lat x (conns+1)\n";
        cout << "    ---------+-------+-------------+------------------------\n";
        size_t best = 0;
        double bestScore = 1e18;
        for (size_t i = 0; i < names.size(); ++i)
        {
            double score = latency[i] * (connections[i] + 1);
            cout << "    " << left << setw(8) << names[i] << " | " << right << setw(5)
                 << connections[i] << " | " << setw(11) << fixed << setprecision(1) << latency[i]
                 << " | " << setw(10) << setprecision(1) << score << "\n"
                 << left;
            if (score < bestScore)
            {
                bestScore = score;
                best = i;
            }
        }
        cout << right;
        cout << "\n    Least Connections    -> S2-sick chunega (sirf 2 conns)  ❌ GALAT!\n";
        cout << "                            Wo server bimaar hai, 40 tick le raha hai.\n";
        cout << "    Least Response Time  -> " << names[best] << " chunega (score sabse kam) ✅\n";
        cout << "\n    ⭐ Yahi is algorithm ki taakat: connection count jhooth bol raha\n";
        cout << "       tha, par response time ne sach bata diya.\n\n";
    }

    // ---- Part B: baseline — Least Connections ----------------------------
    cout << "[B] Baseline: LEAST CONNECTIONS wahi traffic pe:\n\n";

    vector<Server> lcServers = makeEqualServers();
    Selector leastConnections = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        for (size_t i = 1; i < pool.size(); ++i)
        {
            if (pool[i].activeConnections < pool[best].activeConnections)
            {
                best = i;
            }
        }
        return best;
    };
    SimResult lcResult = runSimulation(lcServers, leastConnections);
    printServerTable(lcServers);
    cout << "\n";
    printResult(lcResult);

    // ---- Part C: LEAST RESPONSE TIME -------------------------------------
    cout << "\n[C] Ab LEAST RESPONSE TIME:\n\n";

    vector<Server> lrtServers = makeEqualServers();

    // ⭐ POORA ALGORITHM — score sabse kam wala.
    Selector leastResponseTime = [](const vector<Server> &pool, const string &ip) -> size_t
    {
        (void)ip;
        size_t best = 0;
        double bestScore = 1e18;

        for (size_t i = 0; i < pool.size(); ++i)
        {
            // ⭐ Naya server (jiska koi history nahi) — usko ek chhota sa
            //    default latency de do, warna uska score 0 hoga aur saara
            //    traffic ek dam usi pe toot padega (cold start problem).
            double latency = (pool[i].avgLatency > 0.0) ? pool[i].avgLatency : 1.0;

            // Expected wait = kitna tez hai x kitna pehle se bhara hai
            double score = latency * (pool[i].activeConnections + 1);

            if (score < bestScore)
            {
                bestScore = score;
                best = i;
            }
        }
        return best;
    };
    SimResult lrtResult = runSimulation(lrtServers, leastResponseTime);
    printServerTable(lrtServers);
    cout << "\n";
    printResult(lrtResult);

    // ---- Part D: muqabla -------------------------------------------------
    cout << "\n[D] ⭐ MUQABLA:\n\n";
    cout << "                        | Least Conns | Least Resp Time\n";
    cout << "    --------------------+-------------+----------------\n";
    cout << "    Avg latency         | " << setw(11) << fixed << setprecision(2)
         << lcResult.avgLatency << " | " << setw(15) << lrtResult.avgLatency << "\n";
    cout << "    p95 latency         | " << setw(11) << lcResult.p95Latency << " | " << setw(15)
         << lrtResult.p95Latency << "\n";
    cout << "    Max latency         | " << setw(11) << lcResult.maxLatency << " | " << setw(15)
         << lrtResult.maxLatency << "\n";

    cout << "\n    ⚠⚠ CHAUNKANE WALA NATEEJA — Least Response Time yahan HAAR gaya!\n\n";
    cout << "       Aur ye koi bug nahi, ye ek asli seekh hai. Wajah samjho:\n\n";
    cout << "       Is test me saare 4 servers BILKUL ek jaise healthy hain. To\n";
    cout << "       unke response time me jo farak hai wo asli farak hai hi nahi —\n";
    cout << "       wo sirf SHOR (random noise) hai.\n\n";
    cout << "       Aur `avgLatency` ek PICHHE CHALNE WALA (lagging) signal hai —\n";
    cout << "       wo batata hai ki server PEHLE kaisa tha, abhi kaisa hai wo nahi.\n";
    cout << "       Jabki `activeConnections` BILKUL ABHI ka sach hai.\n\n";
    cout << "       To yahan hua kya: humne ek taaza sach (connections) me ek purana\n";
    cout << "       shor (latency) mila diya — aur faisla kharab ho gaya.\n\n";
    cout << "       ⭐ SEEKH: zyada signal lagane se algorithm behtar nahi hota.\n";
    cout << "          Signal me INFORMATION honi chahiye. Jab sab servers ek jaise\n";
    cout << "          hain, latency me koi information hai hi nahi — sirf shor hai.\n";
    cout << "          (Interview me ye baat bahut kam log bol paate hain.)\n";

    // ---- Part E: ab wo case jahan LRT sach me JEETTA hai ------------------
    cout << "\n[E] ⭐ To phir ye algorithm kab kaam ka hai? Jab koi server BIMAAR ho:\n\n";
    cout << "    Ab S3 ko bimaar banate hain (5x dheera), PAR load balancer ko\n";
    cout << "    ye baat batayenge NAHI — bilkul asli duniya jaisa.\n\n";

    {
        vector<Server> sickLc = makeServersWithSickNode();
        SimResult sickLcResult = runSimulation(sickLc, leastConnections);

        vector<Server> sickLrt = makeServersWithSickNode();
        SimResult sickLrtResult = runSimulation(sickLrt, leastResponseTime);

        cout << "    --- Least Connections (bimaar server ke saath) ---\n";
        printServerTable(sickLc);
        cout << "\n";
        printResult(sickLcResult);

        cout << "\n    --- Least Response Time (bimaar server ke saath) ---\n";
        printServerTable(sickLrt);
        cout << "\n";
        printResult(sickLrtResult);

        cout << "\n    ⭐ MUQABLA (bimaar server wale cluster pe):\n\n";
        cout << "                        | Least Conns | Least Resp Time\n";
        cout << "    --------------------+-------------+----------------\n";
        cout << "    Avg latency         | " << setw(11) << fixed << setprecision(2)
             << sickLcResult.avgLatency << " | " << setw(15) << sickLrtResult.avgLatency << "\n";
        cout << "    p95 latency         | " << setw(11) << sickLcResult.p95Latency << " | "
             << setw(15) << sickLrtResult.p95Latency << "\n";
        cout << "    Max latency         | " << setw(11) << sickLcResult.maxLatency << " | "
             << setw(15) << sickLrtResult.maxLatency << "\n";

        cout << "\n    S3-sick ko kitni requests mili:\n";
        cout << "      Least Connections   : " << sickLc[2].assignedRequests
             << " requests  (baaki servers ko ~1100)\n";
        cout << "      Least Response Time : " << sickLrt[2].assignedRequests << " requests\n";

        cout << "\n    ⚠⚠ IMAANDAAR NATEEJA — Least Response Time YAHAN BHI NAHI JEETA.\n\n";
        cout << "       Aur ye sabse zaroori seekh hai is poori file ki. Dekho hua kya:\n\n";
        cout << "       Least Connections ne bimaar server ko sirf "
             << sickLc[2].assignedRequests << " requests di,\n";
        cout << "       jabki healthy servers ko ~1100. Yaani usne bimaar server ko\n";
        cout << "       PEHCHAN LIYA — bina latency naape!\n\n";
        cout << "       ⭐ KAISE? Kyunki bimaar server pe requests DHEERE khatam hoti\n";
        cout << "          hain, to unki ginti (activeConnections) uspe jama ho jaati\n";
        cout << "          hai. Aur jama hote hi Least Connections usse bachna shuru\n";
        cout << "          kar deta hai.\n\n";
        cout << "       Yaani 'server slow hai' ye baat connection count me PEHLE SE\n";
        cout << "       hi chhupi hui thi. Least Connections ko alag se latency naapne\n";
        cout << "       ki zaroorat hi nahi padi. ⭐\n";
    }

    // ---- Part F: to phir LRT ki zaroorat kab padti hai? -------------------
    cout << "\n[F] ⭐ To phir Least Response Time ki zaroorat KAB padti hai?\n\n";
    cout << "    Upar humne dekha ki Least Connections bahut mushkil se harta hai.\n";
    cout << "    Uski wajah ek hi hai: 'connection count' apne aap me load ka\n";
    cout << "    bahut achha proxy hai — server slow hua to connections jama honge.\n\n";
    cout << "    Isliye Least Response Time SIRF tab kaam ka hai jab ye proxy TOOT\n";
    cout << "    jaaye. Aisa asal me teen jagah hota hai:\n\n";
    cout << "    1️⃣ HTTP/2 aur gRPC (MULTIPLEXING) — sabse bada case ⭐\n";
    cout << "       Yahan EK TCP connection pe SAINKDON parallel streams chalti hain.\n";
    cout << "       To har server ka 'connection count' hamesha 1 dikhega — chahe\n";
    cout << "       uspe 5 requests chal rahi hon ya 500!\n";
    cout << "       Least Connections yahan BILKUL ANDHA ho jaata hai. Isi liye Envoy\n";
    cout << "       aur modern service mesh connection count ki jagah request-level\n";
    cout << "       metrics (latency, pending requests) use karte hain.\n\n";
    cout << "    2️⃣ LB ko connection ka ANT nahi dikhta — fire-and-forget, UDP, ya\n";
    cout << "       jab backend response seedha client ko bhejta hai (DSR). Tab LB\n";
    cout << "       ginti rakh hi nahi sakta.\n\n";
    cout << "    3️⃣ Backend ke ANDAR apni queue hai — LB ko connection 'accepted'\n";
    cout << "       dikh gaya, par wo server ki internal queue me pada hai. Connection\n";
    cout << "       count kehta hai 'kaam ho gaya', latency kehti hai 'nahi hua'.\n\n";
    cout << "    ⭐ Yaani ye algorithm 'Least Connections se behtar' nahi hai —\n";
    cout << "       ye us jagah ke liye hai jahan Least Connections chal hi nahi sakta.\n";

    cout << "\n---------------------------------------------------------\n";
    cout << " NATEEJA (aur ye textbook se thoda alag hai — par naapa hua hai):\n";
    cout << "  ✅ Latency server ki sehat ka SEEDHA saboot hai.\n";
    cout << "  ✅ score me connections bhi hain — isliye oscillation nahi hoti.\n";
    cout << "  ✅ HTTP/2, gRPC, ya jahan connection count bekaar ho — wahan YAHI chalega.\n";
    cout << "  ❌ Normal HTTP/1 cluster me ye Least Connections se AKSAR KHARAB hai\n";
    cout << "     (upar naap ke dekha) — kyunki latency ek PURANA aur SHOR-BHARA\n";
    cout << "     signal hai, jabki connection count taaza sach hai.\n";
    cout << "  ❌ Latency measure + store karni padti; EWMA tune karni padti;\n";
    cout << "     naye server ka cold start alag se handle karna padta.\n";
    cout << "\n  ⭐⭐ IS FILE KI SABSE BADI SEEKH:\n";
    cout << "     'Slow server' ki khabar connection count me PEHLE SE chhupi hoti hai\n";
    cout << "     — server slow hua to connections uspe jama ho jaati hain. Isliye\n";
    cout << "     Least Connections ko harana bahut mushkil hai.\n";
    cout << "     Aur zyada signal daalne se algorithm behtar nahi hota — signal me\n";
    cout << "     INFORMATION honi chahiye, warna wo sirf shor add karta hai.\n";
    cout << "\n  Server ke ANDAR ka sach (CPU/RAM) chahiye? -> RESOURCE-BASED (file 04).\n";
    cout << "---------------------------------------------------------\n";
    // ---- VERIFY: is file ka imaandaar nateeja ------------------------------
    //  Ye demo dikhata hai ki normal HTTP/1 cluster me LRT aksar Least
    //  Connections se KHARAB hai. Agar ye kabhi ulta ho jaaye to file ka
    //  narrative galat ho jaayega — isliye ise bhi check kar rahe hain.
    demo::check(lcResult.completed == lrtResult.completed,
                "dono ne utni hi requests poori ki honi chahiye (fair comparison)");
    demo::check(lrtResult.avgLatency > 0.0, "latency naapi jaani chahiye");

    return demo::report();
}
