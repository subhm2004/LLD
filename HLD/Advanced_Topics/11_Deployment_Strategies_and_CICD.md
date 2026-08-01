# 🚀 Deployment Strategies & CI/CD — Blue-Green, Canary, Rolling, Feature Flags

> **Deployment strategy** = naya code production me kaise le jaayein taaki **users ko downtime na ho**
> aur agar bug ho to **jaldi wapas** aa sakein. **CI/CD** = code ko commit se production tak automate
> karna. System design me ye "reliability + velocity dono kaise" ka jawaab hai — kharaab deploy hi
> outages ka #1 kaaran hota hai.

---

## 1. CI/CD — pehle base samajh lo

```mermaid
flowchart LR
    Dev[Code push] --> CI[CI: build + test + lint]
    CI --> ART[Artifact / Docker image]
    ART --> CD[CD: deploy to env]
    CD --> ST[Staging: auto tests]
    ST --> PR[Production: chosen strategy]
```

| Term | Full | Matlab |
|---|---|---|
| **CI** | Continuous **Integration** | Har commit pe auto build + test → bugs jaldi pakdo, `main` hamesha green |
| **CD** | Continuous **Delivery** | Har green build **deployable**; production push ek button/approval |
| **CD** | Continuous **Deployment** | Wahi, par production tak **fully automatic** (no manual button) |

> **CI ki jaan:** har change automatically test ho → integration problems chhoti + jaldi pakdi jaayein
> ("merge hell" nahi). CD ki jaan: release **boring/routine** ban jaaye (chhote, frequent, safe).

---

## 2. Deployment strategies — overview

Problem: v1 chal raha, v2 laana hai. Kaise switch karein?

| Strategy | Downtime | Rollback | Risk | Cost |
|---|---|---|---|---|
| **Recreate** | ❌ Haan (v1 band, v2 start) | Slow | High | Sasta |
| **Rolling** | ✅ No | Medium | Medium | Sasta |
| **Blue-Green** | ✅ No | ⚡ Instant | Low | Mehnga (2x infra) |
| **Canary** | ✅ No | ⚡ Fast | ⚡ Lowest | Medium |

---

## 3. Recreate (naive) — mat karo production me
v1 band → v2 start. Beech me **downtime**. Sirf dev/non-critical ke liye.

---

## 4. Rolling Deployment — thoda-thoda replace

Instances ko **batch me** ek-ek/kuch-kuch karke v2 se replace karo. Kuch v1, kuch v2 saath chalte hain
transition me.

```mermaid
flowchart TB
    S["Start: 4x v1"] --> R1["1 v2, 3 v1"]
    R1 --> R2["2 v2, 2 v1"]
    R2 --> R3["3 v2, 1 v1"]
    R3 --> D["4x v2 done"]
```

- ✅ No downtime, extra infra nahi (same pool me replace), K8s ka default.
- ❌ Rollback slow (wapas rolling karna); transition me **do versions saath** → backward-compatible
  hona zaroori (API + DB dono).

---

## 5. ⭐ Blue-Green Deployment — instant switch & rollback

Do **poore identical** environments: **Blue** (abhi live), **Green** (naya v2). Green ready + tested →
load balancer/DNS **poora traffic** Green pe switch. Bug aaya? → turant wapas Blue.

```mermaid
flowchart LR
    LB[Load balancer / DNS] -->|"100% abhi"| B[Blue v1 live]
    LB -.->|"test only"| G[Green v2 ready]
    G ==>|"switch: 100% traffic"| LB2[LB -> Green]
    LB2 -.->|"rollback = wapas Blue"| B
```

- ✅ **Zero downtime**, **instant rollback** (bas traffic wapas Blue pe — Blue chalu rehta thodi der).
  Green pe production-jaisa test switch se pehle.
- ❌ **2x infra** (dono environments chahiye) → mehnga. **DB migration** dono versions ke saath chalni
  chahiye (shared DB) — ye asli challenge.

---

## 6. ⭐ Canary Deployment — sabse safe

Naya v2 pehle **thode users** (jaise 1-5%) ko do, **metrics dekho** (error rate, latency). Sab theek →
dheere-dheere % badhao (5→25→50→100). Kharaab → us 5% ko wapas v1, blast radius chhota.

```mermaid
flowchart LR
    U[100% traffic] --> LB{Traffic split}
    LB -->|"95%"| V1[v1 stable]
    LB -->|"5% canary"| V2[v2 new]
    V2 --> M[Monitor: errors, latency, business metrics]
    M -->|"achha"| INC["% badhao: 25 -> 50 -> 100"]
    M -->|"kharaab"| RB["rollback: 5% wapas v1"]
```

- ✅ **Lowest risk** — bug sirf chhote % ko affect; real production traffic pe test; automated (metrics
  se auto-promote/rollback).
- ❌ Complex (traffic splitting + strong monitoring chahiye — dekho [Observability](./02_Observability_Monitoring_Logging_Tracing.md));
  dono versions saath (backward compat).

> **Canary vs Blue-Green:** Blue-Green = **100% ek saath** switch (fast, par sabko ek saath risk).
> Canary = **gradual %** (safest, par slow + monitoring-heavy). Bade risk-averse systems canary use karte.

---

## 7. Feature Flags (Feature Toggles) — deploy ≠ release

Code deploy karo par feature **flag ke peeche** (band). Chahe jab flag on karke **release** karo — bina
naye deploy ke. Deploy aur release ko **alag** kar deta.

```mermaid
flowchart LR
    D[v2 deployed, feature OFF] --> F{Flag on?}
    F -->|"5% users"| NEW[Naya feature]
    F -->|"baaki"| OLD[Purana behavior]
    F -.->|"bug! flag off"| OLD
```

- ✅ **Instant kill switch** (bug → flag off, no rollback deploy), gradual rollout (% users),
  **A/B testing**, "dark launch" (code live but hidden), trunk-based dev (incomplete feature flag ke peeche merge).
- ❌ **Flag debt** (purane flags hataao warna code me `if` ka jungle), testing combinations badh jaati.
- Tools: LaunchDarkly, Unleash, ya khud ka config service.

---

## 8. ⭐ Database migrations — deployment ka sabse tricky part

Zero-downtime deploys ka **asli** challenge code nahi, **DB schema** hai. Transition me **do code
versions** (v1, v2) **ek hi DB** pe chalte hain — schema dono ke saath compatible hona chahiye.

### Expand-Contract (Parallel Change) pattern
Column rename `name` → `full_name` ko safe karne ke liye **kabhi ek saath mat** karo:

```mermaid
flowchart TB
    E["1. EXPAND: naya column full_name add karo (dono rakho)"] --> M["2. MIGRATE: code dono me likhe; purana data copy"]
    M --> R["3. code sirf full_name padhe/likhe (v2 fully live)"]
    R --> C["4. CONTRACT: purana column name drop karo (baad me)"]
```

1. **Expand** — naya column add (purana bhi rakho); backward-compatible.
2. **Migrate** — code dono columns me likhe; purana data backfill.
3. **Switch** — sab v2, sirf naya column use.
4. **Contract** — purana column baad me (jab koi v1 nahi) drop.

> **Golden rule:** schema changes **backward-compatible** rakho. Additive (add column/table) safe;
> destructive (drop/rename) ko multi-step expand-contract me todo. Warna rollback pe purana code naye
> schema pe crash karega.

---

## 9. Rollback vs Roll-forward
- **Rollback** — pichle stable version pe wapas (fast, jab naya version toota).
- **Roll-forward** — aage ek fix deploy (jab rollback mushkil, jaise DB already migrated).
- **Immutable deployments** (naya image, purana untouched) rollback ko trivial banate.

---

## 10. Full picture: modern pipeline

```mermaid
flowchart LR
    C[Commit] --> CI[CI: build+test]
    CI --> IMG[Docker image tag]
    IMG --> STG[Staging deploy + smoke tests]
    STG --> CAN[Canary 5% prod]
    CAN -->|metrics OK| ROLL[Progressive 25-50-100%]
    CAN -->|metrics bad| RB[Auto rollback]
    ROLL --> FF[Feature flags for release control]
```

---

## ✅ Best Practices

- **CI:** har commit test; `main` hamesha releasable; small frequent changes.
- **Zero-downtime:** rolling/blue-green/canary — never recreate in prod.
- **Canary + automated metric gates** for risky changes.
- **Feature flags** — deploy ≠ release, instant kill switch.
- **Backward-compatible DB migrations** (expand-contract).
- **Immutable, versioned artifacts** → easy rollback.
- **Automate rollback** on SLO breach.

---

## 🎤 Interview Q&A

**Q: Blue-green vs canary?**
Blue-green = 100% ek saath naye env pe switch (instant rollback, 2x infra). Canary = gradual % (safest, blast radius chhota, monitoring-heavy).

**Q: Rolling deployment downside?**
Rollback slow + transition me do versions saath → backward-compatible code + schema zaroori.

**Q: Zero-downtime deploy ka sabse bada challenge?**
DB schema migration — do code versions ek DB pe; expand-contract (additive pehle, destructive baad me).

**Q: Feature flag kya deta?**
Deploy ko release se alag; instant kill switch (bug→off, no redeploy), gradual rollout, A/B test, dark launch.

**Q: CI vs CD?**
CI = har commit auto build+test (integrate early). CD = green build deployable (delivery) / auto-to-prod (deployment).

**Q: Canary me kya monitor karte, decision kaise?**
Error rate, latency (p99), business metrics; threshold theek → % badhao, breach → auto rollback.

**Q: Column rename safely kaise?**
Expand-contract: naya column add → dono me likho + backfill → v2 read/write naya → purana column baad me drop.

**Q: Rollback vs roll-forward?**
Rollback = pichle version pe (fast); roll-forward = fix aage deploy (jab rollback mushkil, e.g. DB migrated).

---

## Summary
- **CI/CD** = commit→prod automate; CI har commit test, CD deployable/auto-deploy.
- **Rolling** (batch replace, no extra infra), **Blue-Green** (2 env, instant switch + rollback, 2x cost), **Canary** (gradual %, safest, monitoring-driven) — production me kabhi **recreate** nahi.
- **Feature flags** = deploy ≠ release, instant kill switch + gradual rollout + A/B.
- **Zero-downtime ka asli challenge = DB migrations**: backward-compatible **expand-contract**.
- **Immutable artifacts + automated rollback** on metric breach.

> **Related:** [Resilience & Fault Tolerance](./07_Resilience_and_Fault_Tolerance.md) · [Observability](./02_Observability_Monitoring_Logging_Tracing.md) · [Service Discovery & Mesh](./10_Service_Discovery_and_Service_Mesh.md) · [Monolithic vs Microservices](../01_Monolithic_and_Microservices.md) · [Database Replication](../Database_Replication.md)
