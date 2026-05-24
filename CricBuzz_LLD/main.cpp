#include <algorithm>
#include <iostream>
#include <string>

#include "core/CricBuzzSystem.h"
#include "enums/BallOutcome.h"
#include "enums/MatchFormat.h"
#include "enums/TossDecision.h"

using namespace std;
using namespace cricbuzz_lld;

static void printScoreboard(const LiveScoreboard& board) {
    cout << "  " << board.battingTeamName << " " << board.runs << "/" << board.wickets;
    cout << " (" << board.overs << "." << board.ballsInOver << " ov)";
    cout << "  RR " << board.runRate;
    if (!board.targetInfo.empty()) {
        cout << "  |  " << board.targetInfo;
    }
    cout << "\n";
    if (!board.lastBallSummary.empty()) {
        cout << "  Last ball: " << board.lastBallSummary << "\n";
    }
}

int main() {
    cout << "====== CricBuzz Live Cricket Scoring LLD Demo ======\n\n";

    CricBuzzSystem cricbuzz;

    const string indiaId = cricbuzz.registerTeam("India", "IND");
    const string ausId = cricbuzz.registerTeam("Australia", "AUS");

    const string rohitId = cricbuzz.registerPlayer(indiaId, "Rohit Sharma");
    const string kohliId = cricbuzz.registerPlayer(indiaId, "Virat Kohli");
    const string bumrahId = cricbuzz.registerPlayer(indiaId, "Jasprit Bumrah");
    const string warnerId = cricbuzz.registerPlayer(ausId, "David Warner");
    const string smithId = cricbuzz.registerPlayer(ausId, "Steve Smith");
    const string starcId = cricbuzz.registerPlayer(ausId, "Mitchell Starc");

    cout << "--- Schedule T20: IND vs AUS @ Wankhede ---\n";
    const string matchId =
        cricbuzz.scheduleMatch(indiaId, ausId, "Wankhede Stadium", MatchFormat::T20);
    cout << "  Match " << matchId << " scheduled\n";

    cout << "\n--- Toss: India wins, elects to BAT ---\n";
    cricbuzz.conductToss(matchId, indiaId, TossDecision::BAT);
    cricbuzz.startMatch(matchId);
    printScoreboard(cricbuzz.getLiveScore(matchId));

    cout << "\n--- 1st innings ball-by-ball ---\n";
    cricbuzz.recordBall(matchId, rohitId, starcId, BallOutcome::FOUR);
    cricbuzz.recordBall(matchId, rohitId, starcId, BallOutcome::DOT);
    cricbuzz.recordBall(matchId, kohliId, starcId, BallOutcome::SIX);
    cricbuzz.recordBall(matchId, kohliId, starcId, BallOutcome::RUNS, 2);
    cricbuzz.recordBall(matchId, rohitId, starcId, BallOutcome::WICKET, 0, "caught");
    printScoreboard(cricbuzz.getLiveScore(matchId));

    cout << "\n--- Commentary feed (last 3 balls) ---\n";
    for (const BallEvent& ball : cricbuzz.getCommentaryFeed(matchId, 3)) {
        cout << "  " << ball.overNumber << "." << ball.ballInOver << "  " << ball.commentary
             << "  (+" << ball.runsScored << ")\n";
    }

    cout << "\n--- End 1st innings, start chase ---\n";
    cricbuzz.endInnings(matchId);
    cricbuzz.startSecondInnings(matchId);
    printScoreboard(cricbuzz.getLiveScore(matchId));

    cout << "\n--- 2nd innings ---\n";
    cricbuzz.recordBall(matchId, warnerId, bumrahId, BallOutcome::RUNS, 1);
    cricbuzz.recordBall(matchId, warnerId, bumrahId, BallOutcome::FOUR);
    cricbuzz.recordBall(matchId, smithId, bumrahId, BallOutcome::WIDE);
    printScoreboard(cricbuzz.getLiveScore(matchId));

    const Match matchState = cricbuzz.getMatch(matchId);
    const int firstInningsRuns = matchState.innings[0].runs;
    const LiveScoreboard chase = cricbuzz.getLiveScore(matchId);
    const int margin = max(1, (firstInningsRuns + 1) - chase.runs);
    cricbuzz.endMatch(matchId, cricbuzz.getTeam(indiaId).name + " wins by " + to_string(margin) + " runs");
    printScoreboard(cricbuzz.getLiveScore(matchId));

    cout << "\n--- Live matches list ---\n";
    const vector<string> live = cricbuzz.listLiveMatches();
    cout << "  LIVE count: " << live.size() << " (0 after match completed)\n";

    cout << "\nDemo complete.\n";
    return 0;
}
