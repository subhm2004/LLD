# Problem Statement: Consistent Hashing System

Design an in-memory consistent hashing ring that decides which node owns a given key, so that adding
or removing nodes moves as few keys as possible.

The system should support:

1. Adding nodes to the ring (with an optional capacity weight).
2. Removing nodes from the ring.
3. Marking node health state (`ACTIVE` / `DOWN`) without changing ring positions.
4. Looking up the owner node for a key.
5. Returning a replica set (preference list) of distinct physical nodes for a key.
6. Swapping the hash function used to place nodes and keys on the ring.
7. Notifying interested components whenever the ring topology changes.

Design should be modular so new hash functions and new topology-change listeners can be added easily.
