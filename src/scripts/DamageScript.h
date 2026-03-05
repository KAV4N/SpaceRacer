#pragma once
#include "StrikeEngine.h"
#include "GameManager.h"

class DamageScript : public Strike::Script {
public:
    void onStart()          override;
    void onUpdate(float dt) override;

    float mInvulnerabilityDuration = 2.0f;
    float mBlinkFrequency          = 8.0f;

    float mShakeDuration   = 0.15f;
    float mShakeMagnitude  = 5.f;
    float mShakeFrequency  = 100.0f;

    
    bool  isShaking      = false;
    glm::vec3 shakeBase  = glm::vec3(0.0f);
private:
    void applyDamage();
    void collectRenderers();
    void applyBlinkColor(const glm::uvec3& color);
    void restoreOriginalColors();
    void updateShake(float dt);

    GameManager* mGameManager = nullptr;

    bool  mIsInvulnerable    = false;
    float mInvulnerableTimer = 0.0f;
    float mBlinkTimer        = 0.0f;
    bool  mBlinkState        = false;

    float mShakeTimer     = 0.0f;
    float mShakePhase     = 0.0f;
    

    std::vector<Strike::Entity> mRenderEntities;
    std::vector<glm::uvec3>     mOriginalColors;
};