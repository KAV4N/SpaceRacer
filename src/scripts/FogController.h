#pragma once
#include "StrikeEngine.h"

class GameManager;

class FogController : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

    float mFogDensityQuiet =  0.025f;
    float mFogDensityLoud  =  0.001f;

    float mFogStartQuiet   =  20.0f;
    float mFogStartLoud    = 400.0f;
    float mFogEndQuiet     = 120.0f;
    float mFogEndLoud      = 600.0f;

    glm::uvec3 mFogColor   = glm::uvec3(140, 155, 175);

    float mPeakRMS         = 0.4f;
    float mSmoothAttack    = 0.5f;
    float mSmoothRelease   = 0.3f;

private:
    GameManager* mGameManager       = nullptr;
    float        mSmoothedAmplitude = 0.0f;
};