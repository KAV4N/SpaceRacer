#include "LevelGenerator.h"
#include "CoinSpin.h"

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
    mGameMusic = scriptEntity.getScene()->getEntity("GameMusic");

    if (!mGameMusic) {
        STRIKE_WARN("LevelGenerator: could not find 'GameMusic' entity");
    }

    mSegmentLength = getSegmentLength();

    float shipZ = mSpaceship.isValid() ? mSpaceship.getWorldPosition().z : 0.0f;
    mNextSpawnZ = shipZ + kDestroyBehind;

    mSuppressObstacles = true;
    while (mNextSpawnZ > shipZ - kSpawnAhead) {
        spawnSegment(glm::vec3(0.0f, 0.0f, mNextSpawnZ));
        mNextSpawnZ -= mSegmentLength;
    }
    mSuppressObstacles = false;
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

    updateObstacleReaction(deltaTime);
    cleanupBehindShip();
}

void LevelGenerator::onDestroy() {
    for (auto& seg : mSegments) {
        if (seg.isValid()) seg.destroy();
    }
    mSegments.clear();

    for (auto& obs : mObstacles) {
        if (obs.isValid()) obs.destroy();
    }
    mObstacles.clear();
    mObstacleBaseScaleY.clear();

    for (auto& coin : mCoins) {
        if (coin.isValid()) coin.destroy();
    }
    mCoins.clear();

    mRawAmplitude      = 0.0f;
    mSmoothedAmplitude = 0.0f;
    mNextSpawnZ        = 0.0f;
}

void LevelGenerator::updateObstacleReaction(float dt) {
    if (!mGameMusic || mObstacles.empty()) return;

    mRawAmplitude = Strike::Application::get().getAudioAmplitude(mGameMusic);

    float speed = (mRawAmplitude > mSmoothedAmplitude)
        ? mSmoothAttack
        : mSmoothRelease;

    mSmoothedAmplitude = glm::mix(
        mSmoothedAmplitude,
        mRawAmplitude,
        glm::clamp(speed * dt, 0.0f, 1.0f)
    );

    float t = glm::clamp(
        mSmoothedAmplitude / glm::max(mPeakRMS, 0.001f),
        0.0f, 1.0f
    );

    for (size_t i = 0; i < mObstacles.size(); ++i) {
        Strike::Entity& obs = mObstacles[i];
        if (!obs.isValid()) continue;

        glm::vec3 scale = obs.getScale();
        scale.y = mObstacleBaseScaleY[i] * glm::mix(1.0f, mMaxYMultiplier, t);
        obs.setScale(scale);
    }
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
        if (turns % 2 == 0) scale.x = 100.0f;
        else                 scale.z = 100.0f;
        seg.setScale(scale);

        mSegments.push_back(seg);
    }

    if (!mSuppressObstacles) {
        spawnObstacleInGrid(pos);
        spawnCoinInGrid(pos);
    }
}

Strike::Entity LevelGenerator::spawnRockFromModel(const char* modelId, const glm::vec3& pos,
                                                   float yRot,
                                                   float desiredSizeX, float desiredSizeZ,
                                                   float desiredSizeY)
{
    auto* scene = getEntity().getScene();
    if (!scene) return Strike::Entity{};

    Strike::Entity rock = scene->createEntity();
    rock.setTag("Obstacle");

    auto& renderer = rock.addComponent<Strike::RendererComponent>();
    renderer.setModel(modelId);

    auto model = Strike::AssetManager::get().getAsset<Strike::Model>(modelId);

    glm::vec3 scale(1.0f);
    if (model && model->isReady()) {
        glm::vec3 boundsSize = model->getBounds().getSize();

        float xFactor = (boundsSize.x > 0.0f) ? (desiredSizeX / boundsSize.x) : 1.0f;
        float zFactor = (boundsSize.z > 0.0f) ? (desiredSizeZ / boundsSize.z) : 1.0f;
        float yFactor = (boundsSize.y > 0.0f) ? (desiredSizeY / boundsSize.y) : 1.0f;

        scale = glm::vec3(xFactor, yFactor, zFactor);
    }

    static std::uniform_int_distribution<int> s_blueDist(100, 255);
    static std::uniform_int_distribution<int> s_dimDist(0, 60);

    int r = s_dimDist(s_rng);
    int g = s_dimDist(s_rng);
    int b = s_blueDist(s_rng);
    renderer.setColor(glm::vec3(r, g, b));

    rock.setWorldPosition(pos);
    rock.setEulerAngles(glm::vec3(0.0f, yRot, 0.0f));
    rock.setScale(scale);

    return rock;
}

void LevelGenerator::spawnObstacleInGrid(const glm::vec3& segmentPos) {
    float yRot   = s_yRot(s_rng);
    float scaleY = s_yScale(s_rng);

    glm::vec3 pos = calculateCell(segmentPos);

    int variant = s_rockVariant(s_rng);
    Strike::Entity rock{};
    switch (variant) {
        case 0: rock = spawnRockFromModel("rock1_model", pos, yRot, 33.f, 2.5f, scaleY); break;
        case 2: rock = spawnRockFromModel("rock2_model", pos, yRot, 33.f, 2.5f, scaleY); break;
        case 3: rock = spawnRockFromModel("rock3_model", pos, yRot, 33.f, 2.5f, scaleY); break;
        case 4: rock = spawnRockFromModel("rock4_model", pos, yRot, 33.f, 2.5f, scaleY); break;
        case 5: rock = spawnRockFromModel("rock5_model", pos, yRot, 33.f, 2.5f, scaleY); break;
    }

    if (!rock.isValid()) return;

    auto& physics = rock.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(true);
    physics.setCanCollide(true);

    mObstacleBaseScaleY.push_back(rock.getScale().y);
    mObstacles.push_back(rock);
}

glm::vec3 LevelGenerator::calculateCell(const glm::vec3& pos) const {
    float cellWidth = kLaneWidth     / static_cast<float>(kGridCols);
    float cellDepth = mSegmentLength / static_cast<float>(kGridRows);

    std::uniform_int_distribution<int> colDist(0, kGridCols - 1);
    std::uniform_int_distribution<int> rowDist(0, kGridRows - 1);

    int col = colDist(s_rng);
    int row = rowDist(s_rng);

    float x = -kLaneWidth * 0.5f + (col + 0.5f) * cellWidth;
    float z =  pos.z - (row + 0.5f) * cellDepth;
    return glm::vec3(x, 0, z);
}

void LevelGenerator::spawnCoinInGrid(const glm::vec3& segmentPos) {
    auto* scene = getEntity().getScene();
    if (!scene) return;

    auto coinTmpl = Strike::AssetManager::get().getAsset<Strike::Template>("coin_tmpl");
    if (!coinTmpl || !coinTmpl->isReady()) return;

    Strike::Entity coin = scene->createEntity();
    coin.setTag("Coin");
    if (coinTmpl->instantiate(coin)) {
        glm::vec3 pos = calculateCell(segmentPos);
        coin.setWorldPosition(glm::vec3(pos.x, 10.f, pos.z));
        coin.setScale(glm::vec3(0.1f));

        auto& physics = coin.addComponent<Strike::PhysicsComponent>();
        glm::vec3 size = Strike::AssetManager::get().getAsset<Strike::Model>("coin")->getBounds().getSize();
        physics.setSize(glm::vec3(size.x, size.z, size.y) * 0.1f);
        physics.setAnchored(true);
        physics.setCanCollide(true);

        auto& logic = coin.getOrAddComponent<Strike::LogicComponent>();
        logic.addScript<CoinSpin>();
    }

    mCoins.push_back(coin);
}

void LevelGenerator::cleanupBehindShip() {
    if (!mSpaceship.isValid()) return;

    float shipZ = mSpaceship.getWorldPosition().z;
    float destroyThreshold = shipZ + kDestroyBehind;

    while (!mSegments.empty()) {
        Strike::Entity& seg = mSegments.front();
        if (!seg.isValid()) {
            mSegments.erase(mSegments.begin());
            continue;
        }
        if (seg.getWorldPosition().z >= destroyThreshold) {
            seg.destroy();
            mSegments.erase(mSegments.begin());
        } else { break; }
    }

    while (!mObstacles.empty()) {
        Strike::Entity& obs = mObstacles.front();
        if (!obs.isValid()) {
            mObstacles.erase(mObstacles.begin());
            mObstacleBaseScaleY.erase(mObstacleBaseScaleY.begin());
            continue;
        }
        if (obs.getWorldPosition().z >= destroyThreshold) {
            obs.destroy();
            mObstacles.erase(mObstacles.begin());
            mObstacleBaseScaleY.erase(mObstacleBaseScaleY.begin());
        } else { break; }
    }

    while (!mCoins.empty()) {
        Strike::Entity& coin = mCoins.front();
        if (!coin.isValid()) {
            mCoins.erase(mCoins.begin());
            continue;
        }
        if (coin.getWorldPosition().z >= destroyThreshold) {
            coin.destroy();
            mCoins.erase(mCoins.begin());
        } else { break; }
    }
}

REGISTER_SCRIPT(LevelGenerator)