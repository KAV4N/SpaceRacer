#pragma once
#include "StrikeEngine.h"

class GameManager;

class SunController : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    struct SkyState {
        float      sunAngle;
        glm::uvec3 sunColor;
        float      intensity;
    };

    glm::uvec3 lerpColor(const glm::uvec3& a, const glm::uvec3& b, float t) const;
    SkyState   evaluateSky(float t) const;

    GameManager* mGameManager       = nullptr;
    float        mSmoothedAmplitude = 0.0f;

public:
    float mPeakRMS       = 0.4f;
    float mSmoothAttack  = 1.f;
    float mSmoothRelease = 0.3f;
};