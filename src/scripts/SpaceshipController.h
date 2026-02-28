#pragma once
#include "StrikeEngine.h"

class SpaceshipController : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    float mFwdSpeed  = 30.0f;
    float mTurnSpeed = 30.0f;
    const float kMaxX = 50.0f;

    float mCurrentTilt        = 0.0f;
    const float kMaxTiltAngle = 25.0f;
    const float kTiltSpeed    = 8.0f;

    float mHoverTime            = 0.0f;
    const float kHoverAmplitude = 0.4f;
    const float kHoverFrequency = 1.5f;

    float mBaseY            = 0.0f;
    bool  mBaseYInitialized = false;
};