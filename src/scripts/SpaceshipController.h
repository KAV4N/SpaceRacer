#pragma once
#include "StrikeEngine.h"

class SpaceshipController : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    const float kMaxX = 60.0f;

    float mCurrentTilt        = 0.0f;
    const float kMaxTiltAngle = 25.0f;
    const float kTiltSpeed    = 8.0f;

    float mCurrentSpeed      = 60.0f;
    const float kMinSpeed    = 60.0f;
    const float kMaxSpeed    = 125.0f;
    const float kAccelRate   = 0.5f;

    float mCurrentTurnSpeed   = 50.0f;
    const float kMinTurnSpeed = 50.0f;
    const float kMaxTurnSpeed = 120.0f;
};