# Cache Eviction Policies — Code Karke Samjho 🗑️

> [08_Caching_and_Distributed_Caching.md](../08_Caching_and_Distributed_Caching.md) me theory hai.
> Ye folder wahi theory **chala kar** dikhata hai — 6 policies, 5 workloads,
> aur har baar hit rate **naap kar**.

---

## 🎯 Is folder ka ek hi point

> **"Sabse achhi eviction policy" naam ki koi cheez hoti hi nahi.**
> Har policy ek **andaaza** lagati hai ki aage kya maanga jaayega. Andaaza aapke
> traffic se mila to policy achhi, nahi mila to buri. Bas itni si baat hai.

Neeche wali table isi ka saboot hai — **har row ka winner alag hai.**

---

## 📑 Is folder me

| File | Kya sikhata hai |
|---|---|
| [cache_common.h](cache_common.h) | 5 workloads + runner + **Belady's optimal** |
| [policies/](policies/) | LRU, LFU, FIFO, Random, CLOCK, ARC |
| [01_lru.cpp](01_lru.cpp) | LRU ki taakat + **scan** aur **loop** wale andhe dhabbe |
| [02_lfu.cpp](02_lfu.cpp) | LFU stable pe best, shifting pe **barbaad** |
| [03_fifo_random_clock.cpp](03_fifo_random_clock.cpp) | Sasti policies — CLOCK ka jugaad, Random ka upset |
| [04_arc.cpp](04_arc.cpp) | ARC khud seekhta hai + **scan-resistant** |
| [05_compare_all.cpp](05_compare_all.cpp) | 6 policies × 5 workloads × Belady |

```bash
./compile.sh && ./05_compare_all_demo
```

---

## 📊 Poora natija (cache size 100)

| workload | LRU | LFU | FIFO | CLOCK | Random | ARC | **OPTIMAL** |
|---|---|---|---|---|---|---|---|
| Zipf (normal traffic) | 43.2 | **54.3** | 38.1 | 41.8 | 38.3 | 53.2 | 62.5 |
| Shifting popularity | 74.5 | 13.2 💀 | 69.5 | 73.4 | 69.2 | **75.5** | 87.4 |
| Working set + scan | 87.2 | **91.2** | 87.2 | 87.2 | 79.4 | **91.2** | 91.2 |
| Loop (120 keys) | 0.0 | 0.0 | 0.0 | 0.0 | **68.1** 🤯 | 0.0 | 82.8 |
| Uniform (no locality) | 4.9 | 4.9 | 4.9 | 4.9 | 5.0 | **5.1** | 28.7 |

> ⭐ **Winner har baar alag:** Zipf → LFU, Shifting → ARC, Scan → LFU/ARC, Loop → **Random**.

---

## 🧠 Chaar baatein jo naap ke nikli

### 1️⃣ LRU ka scan problem — hit rate mat dekho, **MISS rate** dekho

Application 100 keys maang raha hai, cache bhi 100 ka. Beech me analytics job 200 keys scan karta hai:

| | hit rate | **miss rate** |
|---|---|---|
| LRU, bina scan | 99.50% | 0.50% |
| LRU, scan ke saath | 95.00% | **5.00%** |

> Hit rate me 4.5 point ka farak "chhota" lagta hai. Par **miss rate 10x** ho gaya —
> aur har miss ek DB query hai. Yaani database pe **10 guna load**.
> Cache me hamesha miss rate dekho, hit rate dhoka de deta hai.

ARC isi test me **99.45%** deta hai (miss 0.55%, sirf 1.1x) — scan ka asar lagbhag zero.

### 2️⃣ LFU: ek hi policy, +11 aur −63 points

| | LRU | LFU | farak |
|---|---|---|---|
| Zipf (popularity stable) | 43.2% | 54.4% | LFU **+11.1** ⭐ |
| Shifting popularity | 74.7% | 11.6% | LFU **−63.1** 💀 |

> Wajah: phase 1 ke keys ka count 500+ ho jaata hai. Phase 2 ka naya content count 1-2-3 se
> shuru hota hai aur LFU ke liye wo hamesha "kam keemti" rehta hai. Cache me **murda content**
> bhara reh jaata hai.
>
> Isi liye plain LFU production me kam milta hai — **LFU+aging** ya **TinyLFU** milta hai.

### 3️⃣ Random ka upset — "bewakoofi" hi bachav ban gayi

Loop workload (120 keys, cache 100): LRU, LFU, FIFO, CLOCK, ARC — **sab 0.0%**. Random **68.1%**.

> Har deterministic policy ek **niyam** se chalti hai. Cyclic loop us niyam ko theek ulta
> pakad leta hai — policy har baar bilkul wahi key nikaalti hai jo agli maangi jaane wali thi.
> Random ke paas koi niyam hai hi nahi, isliye use ulta pakadna namumkin hai.
>
> ⭐ Isi liye Redis ka default **approximated LRU** hai (5 random keys uthao, unme se purani
> nikalo) — thoda kam accurate, par aise pathological patterns se mehfooz.

### 4️⃣ ARC khud seekhta hai — bina kisi config ke

`p` = T1 (recency) ka target size. Humne kuch set nahi kiya:

| workload | final p | T1 | T2 | ARC ne kya chuna |
|---|---|---|---|---|
| Working set 100 (fits) | 0.0 | 0 | 100 | FREQUENCY |
| Zipf 5000 | 4.0 | 5 | 95 | FREQUENCY |
| Working set + scan | 0.0 | 0 | 100 | FREQUENCY |
| Uniform 400 | 52.3 | 53 | 47 | **RECENCY** |
| Uniform 1000 | 49.4 | 50 | 50 | **RECENCY** |

> Jab key-space cache se 4-10x bada ho, koi key dobara hit hone se pehle nikal jaati hai —
> yaani "frequency" ka matlab hi khatam. ARC ko ye **ghost list B1 ke hits** se pata chalta
> hai aur wo T1 bada kar deta hai.

---

## ⭐ Belady's Optimal — is folder ka sabse kaam ka tool

1966 ka result: sabse behtareen policy hai *"us cheez ko nikalo jo sabse der baad dobara
maangi jaayegi"*. Iske liye **bhavishya** pata hona chahiye — asli system me impossible.
Par simulation me poora trace pehle se hai, to hum ye nikaal sakte hain.

**Iska faayda:** ye ek **ceiling** deta hai.

- LRU 43.2% de raha hai, optimal 62.5% → 19 points ki gunjaish hai, behtar policy dhoondho
- LFU 91.2% de raha hai, optimal bhi 91.2% → **policy perfect hai**, aage mehnat bekaar;
  ab sirf cache SIZE badhane se faayda hoga

> Bina is number ke aap ye jaan hi nahi sakte ki 43% "achha" hai ya "bura".

---

## ⚖️ Kab kya use karein

| Situation | Policy |
|---|---|
| Pata nahi workload kaisa hai | **LRU** — safe default |
| Popularity stable (catalog, config) | **LFU + aging** / TinyLFU |
| Scan / analytics chalte rehte hain | **ARC** ya 2Q |
| Bahut high throughput, lock se bachna hai | **CLOCK** |
| Baseline chahiye | **Random** — isse behtar nahi ho to policy bekaar hai |

---

## 💬 Interview me kaise bolna

**"Kaunsi eviction policy use karoge?"**
> Pehle access pattern poochhunga. LRU safe default hai, par uske do andhe dhabbe hain —
> sequential scan aur cyclic loop. Maine test kiya tha: 100-key working set pe ek 200-key
> scan LRU ka **miss rate 0.5% se 5% kar deta hai**, yaani DB pe 10x load. Agar aise scan
> aam hain to ARC ya 2Q lunga, jo scan-resistant hai.

**"LFU LRU se behtar hai?"**
> Depend karta hai, aur farak bahut bada hai. Stable popularity pe LFU LRU se ~11 points
> aage tha, par jab popularity badalti hai to wo **63 points peeche** gir gaya — purana
> popular content count ki wajah se cache me chipak jaata hai. Isi liye plain LFU kam
> use hota hai, LFU+aging ya TinyLFU use hota hai.

**"Cache hit rate 40% hai, kaise improve karein?"**
> Pehle Belady's optimal nikalunga us trace pe. Agar optimal bhi 42% hai to policy me kuch
> nahi bacha — problem cache SIZE ya access pattern me hai. Agar optimal 70% hai tab policy
> badalne ka matlab hai. Bina us ceiling ke tuning andhere me teer chalana hai.

---

## 🔗 Aage padho
- [08_Caching_and_Distributed_Caching.md](../08_Caching_and_Distributed_Caching.md) — caching theory
- [LRU_Cache_LLD](../../LLD/LRU_Cache_LLD/) · [LFU_Cache_LLD](../../LLD/LFU_Cache_LLD/) — inka OOP design (value store karke)
- [Probabilistic_Data_Structures/](../Probabilistic_Data_Structures/README.md) — TinyLFU ka Count-Min Sketch yahan hai
- [Consistent_Hashing/](../Consistent_Hashing/README.md) — cache ko kai nodes me baantna
- [System_Design_Case_Studies/03_Distributed_Cache.md](../System_Design_Case_Studies/03_Distributed_Cache.md) — poora design
