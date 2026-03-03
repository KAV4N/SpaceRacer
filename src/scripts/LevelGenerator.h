#pragma once
#include "StrikeEngine.h"
#include <random>

class LevelGenerator : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

    // Audio reactor tuning
    float mMaxYMultiplier = 3.0f;
    float mPeakRMS        = 0.6f;
    float mSmoothAttack   = 100.0f;
    float mSmoothRelease  =  15.0f;

private:
    void spawnSegment(const glm::vec3& pos);
    void spawnObstaclesInGrid(const glm::vec3& segmentPos);
    void spawnCoinsInGrid(const glm::vec3& segmentPos);
    void cleanupBehindShip();
    float getSegmentLength() const;

    Strike::Entity spawnRockFromModel(const char* modelId, const glm::vec3& pos,
                                      float yRot,
                                      float scaleX, float scaleZ, float scaleY);

    void updateObstacleReaction(float dt);

private:
    Strike::Entity mSpaceship;
    Strike::Entity mGameMusic;

    static constexpr float kSpawnAhead    = 500.0f;
    static constexpr float kDestroyBehind =  20.0f;
    static constexpr float kLaneWidth     = 150.0f;
    static constexpr float kRockY         =   0.0f;
    static constexpr float kCoinY         =  10.0f;  // hover height above ground
    static constexpr int   kGridCols      =     4;
    static constexpr int   kGridRows      =     4;
    static constexpr float kRockYMin      =  25.0f;
    static constexpr float kRockYMax      =  45.0f;

    // How many coins to scatter per segment
    static constexpr int   kCoinsPerSegment = 3;

    float mSegmentLength = 1.0f;
    float mNextSpawnZ    = 0.0f;
    bool  mSuppressObstacles = false;

    std::vector<float>          mObstacleBaseScaleY;
    float mRawAmplitude      = 0.0f;
    float mSmoothedAmplitude = 0.0f;

    static inline std::mt19937 s_rng{ std::random_device{}() };
    static inline std::uniform_int_distribution<int>    s_rockVariant{ 0, 5 };
    static inline std::uniform_real_distribution<float> s_yRot{ 0.0f, 360.0f };
    static inline std::uniform_real_distribution<float> s_yScale{ kRockYMin, kRockYMax };
    static inline std::uniform_int_distribution<int>    s_dist{ 0, 3 };

    std::vector<Strike::Entity> mSegments;
    std::vector<Strike::Entity> mObstacles;
    std::vector<Strike::Entity> mCoins;
};