#include "LevelGenerator.h"

float LevelGenerator::getSegmentLength() const {
    auto groundModel = Strike::AssetManager::get().getAsset<Strike::Model>("ground");
    if (groundModel && groundModel->isReady()) {
        float sizeZ = groundModel->getBounds().getSize().z / 10.f;
        if (sizeZ > 0.0f) return sizeZ;
    }
    return 1.0f;
}

void LevelGenerator::onStart() {
    mSpaceship = scriptEntity.getScene()->getEntity("Spaceship");

    mSegmentLength = getSegmentLength();

    float shipZ = mSpaceship.isValid() ? mSpaceship.getWorldPosition().z : 0.0f;
    mNextSpawnZ = shipZ + kDestroyBehind;

    while (mNextSpawnZ > shipZ - kSpawnAhead) {
        spawnSegment(glm::vec3(0.0f, 0.0f, mNextSpawnZ));
        mNextSpawnZ -= mSegmentLength;
    }
}

void LevelGenerator::onUpdate(float deltaTime) {
    if (!mSpaceship.isValid()) return;

    mSegmentLength = getSegmentLength();

    float shipZ = mSpaceship.getWorldPosition().z;
    float spawnThreshold = shipZ - kSpawnAhead;

    while (mNextSpawnZ > spawnThreshold) {
        spawnSegment(glm::vec3(0.0f, 0.0f, mNextSpawnZ));
        mNextSpawnZ -= mSegmentLength;
    }

    cleanupBehindShip();
}

void LevelGenerator::spawnSegment(const glm::vec3& pos) {
    auto* scene = getEntity().getScene();
    if (!scene) return;

    auto& assetMngr = Strike::AssetManager::get();
    auto groundTmpl = assetMngr.getAsset<Strike::Template>("ground_tmpl");

    if (groundTmpl && groundTmpl->isReady()) {
        Strike::Entity seg = scene->createEntity();
        seg.setTag("GroundSegment");
        groundTmpl->instantiate(seg);
        seg.setWorldPosition(pos);

        int turns = s_dist(s_rng);
        seg.setEulerAngles(glm::vec3(0.0f, static_cast<float>(turns * 90), 0.0f));

        glm::vec3 scale(1.f);
        if (turns % 2 == 0) scale.x = 10.0f;
        else                 scale.z = 10.0f;
        seg.setScale(scale);

        mSegments.push_back(seg);
    }

    spawnObstacles(pos);
}


Strike::Entity LevelGenerator::spawnRockFromModel(const char* modelId, const glm::vec3& pos, float yRot, float scale) {
    auto* scene = getEntity().getScene();
    auto model = Strike::AssetManager::get().getAsset<Strike::Model>(modelId);
    if (!model || !model->isReady()) return Strike::Entity{};

    Strike::Entity rock = scene->createEntity();
    rock.setTag("Obstacle");

    auto& renderer = rock.addComponent<Strike::RendererComponent>();
    renderer.setModel(modelId);

    rock.setWorldPosition(pos);
    rock.setEulerAngles(glm::vec3(0.0f, yRot, 0.0f));
    rock.setScale(glm::vec3(scale));


    return rock;
}

void LevelGenerator::spawnObstacles(const glm::vec3& segmentPos) {
    for (int i = 0; i < kObstaclesPerSeg; ++i) {
        if (s_chance(s_rng) > kObstacleChance)
            continue;

        float x     = s_xPos(s_rng);
        float yRot  = s_yRot(s_rng);
        float scale = s_scale(s_rng);
        glm::vec3 pos(x, kRockY, segmentPos.z);

        int variant = s_rockVariant(s_rng);
        Strike::Entity rock{};

        switch (variant) {
            case 0: rock = spawnRockFromModel("rock1_tmpl",  pos, yRot, scale); break;
            case 1: rock = spawnRockFromModel("rock6_tmpl",  pos, yRot, scale); break;
            case 2: rock = spawnRockFromModel("rock2_model",    pos, yRot, scale); break;
            case 3: rock = spawnRockFromModel("rock3_model",    pos, yRot, scale); break;
            case 4: rock = spawnRockFromModel("rock4_model",    pos, yRot, scale); break;
            case 5: rock = spawnRockFromModel("rock5_model",    pos, yRot, scale); break;
            case 6: rock = spawnRockFromModel("box_model",      pos, yRot, scale); break;
        }

        if (!rock.isValid()) continue;
        rock.setScale(glm::vec3(0.4f));

        auto& physics = rock.addComponent<Strike::PhysicsComponent>();
        physics.setAnchored(true);
        physics.setCanCollide(true);

        mObstacles.push_back(rock);
    }
}

void LevelGenerator::cleanupBehindShip() {
    if (!mSpaceship.isValid()) return;

    float shipZ = mSpaceship.getWorldPosition().z;
    float destroyThreshold = shipZ + kDestroyBehind;

    while (!mSegments.empty()) {
        Strike::Entity& seg = mSegments.front();
        if (!seg.isValid()) { mSegments.erase(mSegments.begin()); continue; }
        if (seg.getWorldPosition().z >= destroyThreshold) {
            seg.destroy();
            mSegments.erase(mSegments.begin());
        } else { break; }
    }

    while (!mObstacles.empty()) {
        Strike::Entity& obs = mObstacles.front();
        if (!obs.isValid()) { mObstacles.erase(mObstacles.begin()); continue; }
        if (obs.getWorldPosition().z >= destroyThreshold) {
            obs.destroy();
            mObstacles.erase(mObstacles.begin());
        } else { break; }
    }
}

REGISTER_SCRIPT(LevelGenerator)