#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Opponent
{
    int x, y;         // Opponent position
    int prevX, prevY; // Previous position

    Opponent() {}

    void Update(int x, int y)
    {
        prevX = this->x;
        prevY = this->y;
        this->x = x;
        this->y = y;
    }
};

class Pod
{
public:
    int x, y;                     // Current position
    int checkpointX, checkpointY; // Next checkpoint position
    int checkpointDist;           // Distance to next checkpoint
    int checkpointAngle;          // Angle to next checkpoint
    int prevX, prevY;             // Previous position
    bool boostUsed = false;       // Has the boost been used?

    // Define thresholds as constants
    static constexpr int AGGRESSIVE_DISTANCE_THRESHOLD = 3000;
    static constexpr int DEFENSIVE_DISTANCE_THRESHOLD = 800;

    void Update(int x, int y, int checkpointX, int checkpointY,
                int checkpointDist, int checkpointAngle)
    {
        prevX = this->x;
        prevY = this->y;
        this->x = x;
        this->y = y;
        this->checkpointX = checkpointX;
        this->checkpointY = checkpointY;
        this->checkpointDist = checkpointDist;
        this->checkpointAngle = checkpointAngle;
    }

    std::string Move(const Opponent &opponent)
    {
        // Calculate the distance and angle to the opponent
        int opponentDist =
            std::sqrt(std::pow(opponent.x - x, 2) + std::pow(opponent.y - y, 2));
        int opponentAngle =
            std::atan2(opponent.y - y, opponent.x - x) * (180 / M_PI) -
            checkpointAngle;
        if (opponentAngle > 180)
            opponentAngle -= 360;
        if (opponentAngle < -180)
            opponentAngle += 360;

        // Determine my speed
        double speedY = (y - prevY) / 0.075;
        double speedX = (x - prevX) / 0.075;

        // Predict my position
        double futureX = x + speedX * 0.075;
        double futureY = y + speedY * 0.075;

        // Determine opponent distance to checkpoint
        int opponentDistToCheckpoint =
            std::sqrt(std::pow(opponent.x - checkpointX, 2) +
                      std::pow(opponent.y - checkpointY, 2));

        // Determine opponent speed
        double opponentSpeedY = (opponent.y - opponent.prevY) / 0.075;
        double opponentSpeedX = (opponent.x - opponent.prevX) / 0.075;

        // Predict opponent position
        double opponentFutureX = opponent.x + opponentSpeedX * 0.075;
        double opponentFutureY = opponent.y + opponentSpeedY * 0.075;

        // Calculate opponent future distance from me
        int opponentFutureDist = std::sqrt(std::pow(opponentFutureX - futureX, 2) +
                                           std::pow(opponentFutureY - futureY, 2));

        std::string action = "100"; // Default thrust

        if (ShouldBoost(
                opponentDistToCheckpoint,
                opponentDist)) // check for conditions for an aggressive move
        {
            action = "BOOST";
            boostUsed = true;
        }
        else if (AvoidCollision(opponentDist, opponentAngle,
                                opponentFutureDist)) // check for conditions for a
                                                     // defensive move
        {
            // action = "SHIELD";
            action = std::to_string(DetermineThrust());
        }
        else
        {
            action = std::to_string(DetermineThrust());
        }

        return GetOutput(action);
    }

private:
    // Method for determining if we should ram enemy pod
    bool ShouldBoost(int opponentDistToCheckpoint, int opponentDist)
    {
        return !boostUsed && opponentDist < AGGRESSIVE_DISTANCE_THRESHOLD &&
               checkpointDist > 5000 && std::abs(checkpointAngle) < 5;
    }

    // Method for determining if we should use shield to block enemy attack
    bool AvoidCollision(int opponentDist, int opponentAngle,
                        int opponentFutureDist)
    {
        return opponentFutureDist < DEFENSIVE_DISTANCE_THRESHOLD &&
               std::abs(opponentAngle) > 135;
    }

    int DetermineThrust()
    {
        double factor = 1;
        if (checkpointDist < 3000)
        {
            factor = checkpointDist / 3000.0;
        }
        cerr << "Factor: " << factor << endl;
        if (std::abs(checkpointAngle) < 5)
        {
            return 100 * factor;
        }
        else if (std::abs(checkpointAngle) < 22)
        {
            return 75;
        }
        else if (std::abs(checkpointAngle) < 44)
        {
            return 50;
        }
        else if (std::abs(checkpointAngle) < 68)
        {
            return 25;
        }
        else if (std::abs(checkpointAngle) > 90)
        {
            return 0;
        }
        else
        {
            return 100;
        }
    }

    std::string GetOutput(const std::string &action)
    {
        return std::to_string(checkpointX) + " " + std::to_string(checkpointY) +
               " " + action;
    }
};

int main()
{
    Pod myPod;
    Opponent opponent;

    while (1)
    {
        int x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist,
            next_checkpoint_angle;
        cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >>
            next_checkpoint_dist >> next_checkpoint_angle;
        cin.ignore();
        int opponent_x, opponent_y;
        cin >> opponent_x >> opponent_y;
        cin.ignore();

        opponent.Update(opponent_x, opponent_y);
        myPod.Update(x, y, next_checkpoint_x, next_checkpoint_y,
                     next_checkpoint_dist, next_checkpoint_angle);

        cout << myPod.Move(opponent) << endl;
    }
}
