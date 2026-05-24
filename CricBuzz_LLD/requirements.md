# Requirements — CricBuzz Live Cricket Scoring

## Functional Requirements

1. **Team & Player Registry**
   - System shall allow registering teams with name and short code.
   - System shall allow registering players linked to a team.
   - System shall reject unknown team/player references.

2. **Match Scheduling**
   - System shall schedule a match between two distinct teams with venue and format (T20, ODI, TEST).
   - System shall initialize match status as SCHEDULED.

3. **Toss & Match Start**
   - System shall record toss winner and decision (BAT or BOWL).
   - System shall start match only after toss is conducted.
   - System shall set first innings batting team based on toss decision.
   - System shall transition match status to LIVE on start.

4. **Ball-by-Ball Scoring**
   - System shall record ball events with batsman, bowler, and outcome.
   - System shall support outcomes: DOT, RUNS (0-3), FOUR, SIX, WIDE, NO_BALL, WICKET.
   - System shall validate batsman belongs to batting team and bowler to bowling team.
   - System shall count legal deliveries toward overs (WIDE/NO_BALL are extras, not legal balls).
   - System shall cap wickets at 10 per innings.
   - System shall end innings automatically when max overs for format reached or all out.

5. **Innings & Match Flow**
   - System shall allow manually marking innings complete and starting second innings.
   - System shall swap batting/bowling teams for second innings.
   - System shall complete match with a result summary string.

6. **Live Scoreboard**
   - System shall expose runs, wickets, overs.balls, run rate for current innings.
   - System shall show chase target info during second innings.
   - System shall include last ball commentary on scoreboard.

7. **Commentary Feed**
   - System shall generate human-readable commentary per ball via pluggable generator (Strategy).
   - System shall store ball-by-ball feed retrievable by match id.

8. **Live Match Listing**
   - System shall list match ids currently in LIVE status.

## Non-Functional Requirements

1. **In-memory** — no persistence layer in this LLD.
2. **Single-threaded** demo; production adds per-match locking.
3. **Extensible** — swap `ICommentaryGenerator` for localized/ML commentary.

## Interview Extensions

- Push notifications on wicket/six (Observer)
- Player stats aggregator (runs, wickets career)
- DLS par score calculator
- API gateway + Redis pub/sub for millions of concurrent viewers
