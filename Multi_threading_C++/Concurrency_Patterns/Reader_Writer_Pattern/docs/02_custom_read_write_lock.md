# 02 — Custom ReadWriteLock

**Source:** `../02_custom_read_write_lock.cpp` + `../ReadWriteLock.h`  
**Binary:** `../bin/02_custom_read_write_lock`

---

## Kyun custom?

Interview mein `shared_mutex` ka internal logic puch sakte hain — `ReadWriteLock.h` dikhata hai **readers count + writer flag + CV**.

---

## Flow

1. Readers: `lock_shared()` → wait if `writer_active_`
2. Writer: `lock()` → wait `readers_==0 && !writer_active_`
3. Unlock paths `notify_all` when last reader leaves

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Learn signaling | Same as Signaling pattern CV |
| Custom policy | Writer-preference variant add kiya (demo 06) |
| Whiteboard ready | ~30 lines explainable |

---

## Run

```bash
./bin/02_custom_read_write_lock
```

Writer sets `data=999` exclusive.

---

## vs std

Production → `shared_mutex`. Learning / interview → custom class.
