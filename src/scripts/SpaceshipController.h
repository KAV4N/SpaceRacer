#pragma once
#include "StrikeEngine.h"

class GameManager;

class SpaceshipController : public Strike::Script {
public:
    void onStart()               override;
    void onUpdate(float deltaTime) override;



private:
    const float kMaxX         = 60.0f;
    const float kMaxTiltAngle = 25.0f;

    float mCurrentTilt       = 0.0f;
    float mSmoothedAmplitude = 0.0f;

    float mMinSpeed      = 40.0f;
    float mMaxSpeed      = 200.0f;

    float mMinTurnSpeed  = 40.0f;
    float mMaxTurnSpeed  = 120.0f;

    float mMinTiltSpeed  = 4.0f;
    float mMaxTiltSpeed  = 20.0f;

    float mPeakRMS       = 0.6f;
    float mSmoothAttack  = 100.0f;
    float mSmoothRelease = 30.0f;

    GameManager* mGameManager = nullptr;
};