#pragma once
#include "StrikeEngine.h"

class MenuManager : public Strike::Script {
public:
    void onStart()                 override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& event) override;

private:
    Strike::Entity mAmbientMusic;
    Strike::Entity mHighScoreText;
    Strike::Entity mResultText;
};