#pragma once
#include "StrikeEngine.h"


class CoinSpin : public Strike::Script {
public:
    void onStart()          override;
    void onUpdate(float dt) override;

    float mTopSpeed    =  90.0f;   // degrees per second
    float mCentreSpeed = -150.0f;  // opposite direction, faster
    float mBottomSpeed =  120.0f;  // same dir as top, different speed

private:
    Strike::Entity mTop;
    Strike::Entity mCentre;
    Strike::Entity mBottom;
};