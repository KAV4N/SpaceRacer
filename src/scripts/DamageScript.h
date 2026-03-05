#pragma once
#include "StrikeEngine.h"

class GameManager;

class DamageScript : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

    float mInvulnerabilityDuration = 2.0f;
    float mBlinkInterval           = 0.125f;

private:
    void collectRenderers();
    void applyDamage();
    void applyBlinkColor(const glm::uvec3& color);
    void restoreOriginalColors();

    static void collectRenderersRecursive(Strike::Entity entity,
                                          std::vector<Strike::Entity>& out);

    GameManager* mGameManager    = nullptr;
    bool         mIsInvulnerable = false;
    bool         mBlinkState     = false;

    std::vector<Strike::Entity> mRenderEntities;
    std::vector<glm::uvec3>     mOriginalColors;
};