#pragma once
#include "StrikeEngine.h"

class CollectCoin : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

    int mScoreValue = 10;

private:
    Strike::Entity mScoreText;
};