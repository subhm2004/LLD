# 06 — Memory Order Basics

**Source:** `../06_memory_order_basics.cpp`

Producer: `payload` (relaxed) then `ready` (release).  
Consumer: `ready` (acquire) then read `payload`.

Default `compare_exchange` uses `seq_cst` unless orders passed.
