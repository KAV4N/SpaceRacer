#pragma once
#include "StrikeEngine.h"

class CoinSpin : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

    float mTopSpeed    =  90.0f;
    float mCentreSpeed = -150.0f;

private:
    Strike::Entity mTop;
    Strike::Entity mCentre;
};