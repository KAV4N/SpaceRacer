#pragma once
#include "StrikeEngine.h"
#include <random>

class GameManager;

class LevelGenerator : public Strike::Script {
public:
    void onStart()               override;
    void onUpdate(float deltaTime) override;
    void onDestroy()             override;

    float mMaxYMultiplier = 3.0f;
    float mPeakRMS        = 0.6f;
    float mSmoothAttack   = 100.0f;
    float mSmoothRelease  = 15.0f;

private:
    void  spawnSegment(const glm::vec3& pos);
    void  spawnObstacleInGrid(const glm::vec3& segmentPos);
    void  spawnCoinInGrid(const glm::vec3& segmentPos);
    void  cleanupBehindShip();
    void  updateObstacleReaction(float deltaTime);
    float getSegmentLength() const;

    Strike::Entity spawnRockFromModel(const char* modelId, const glm::vec3& pos,
                                      float desiredSizeY);

    glm::vec3 randomCellPosition(const glm::vec3& segmentPos) const;

    Strike::Entity mSpaceship;
    GameManager*   mGameManager = nullptr;

    static constexpr float kSpawnAhead    = 500.0f;
    static constexpr float kDestroyBehind =  20.0f;
    static constexpr float kLaneWidth     = 150.0f;
    static constexpr int   kGridCols      =     4;
    static constexpr int   kGridRows      =     3;
    static constexpr float kRockYMin      =  25.0f;
    static constexpr float kRockYMax      =  45.0f;

    float mSegmentLength     = 1.0f;
    float mNextSpawnZ        = 0.0f;
    bool  mSuppressObstacles = false;

    float mSmoothedAmplitude = 0.0f;

    std::vector<float>          mObstacleBaseScaleY;
    std::vector<Strike::Entity> mSegments;
    std::vector<Strike::Entity> mObstacles;
    std::vector<Strike::Entity> mCoins;

    static inline std::mt19937                           s_rng{ std::random_device{}() };
    static inline std::uniform_int_distribution<int>    s_rockVariant{ 0, 5 };
    static inline std::uniform_real_distribution<float> s_yScale{ kRockYMin, kRockYMax };
    static inline std::uniform_int_distribution<int>    s_rotSteps{ 0, 3 };
};