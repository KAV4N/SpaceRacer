#pragma once
#include "StrikeEngine.h"
#include <random>

class LevelGenerator : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    void spawnSegment(const glm::vec3& pos);
    void spawnObstacles(const glm::vec3& segmentPos);
    void cleanupBehindShip();
    float getSegmentLength() const;

    Strike::Entity spawnRockFromModel(const char* modelId, const glm::vec3& pos, float yRot, float scale);

private:
    Strike::Entity mSpaceship;

    static constexpr float kSpawnAhead      = 500.0f;
    static constexpr float kDestroyBehind   = 20.0f;
    static constexpr float kLaneWidth       = 40.0f;
    static constexpr int   kObstaclesPerSeg = 2;
    static constexpr float kObstacleChance  = 0.65f;
    static constexpr float kRockY           = 0.0f;

    float mSegmentLength = 1.0f;
    float mNextSpawnZ    = 0.0f;

    static inline std::mt19937 s_rng{ std::random_device{}() };
    static inline std::uniform_int_distribution<int>    s_dist{0, 3};
    static inline std::uniform_int_distribution<int>    s_rockVariant{0, 6};  // 7 rock variants total
    static inline std::uniform_real_distribution<float> s_xPos{ -kLaneWidth * 0.5f, kLaneWidth * 0.5f };
    static inline std::uniform_real_distribution<float> s_yRot{ 0.0f, 360.0f };
    static inline std::uniform_real_distribution<float> s_scale{ 0.8f, 2.2f };
    static inline std::uniform_real_distribution<float> s_chance{ 0.0f, 1.0f };

    std::vector<Strike::Entity> mSegments;
    std::vector<Strike::Entity> mObstacles;
};