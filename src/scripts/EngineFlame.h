#pragma once
#include "StrikeEngine.h"

class EngineFlame : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    float mFlameTime        = 0.0f;
    const float kFlameSpeed = 3.0f;
    const float kDepthStep  = 0.4f;

    glm::uvec3 flameColor(float t) const;
};