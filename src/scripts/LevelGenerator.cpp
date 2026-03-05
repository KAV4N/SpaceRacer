#include "LevelGenerator.h"
#include "GameManager.h"
#include "CoinSpin.h"

float LevelGenerator::getSegmentLength() const {
    auto groundModel = Strike::AssetManager::get().getAsset<Strike::Model>("ground");
    if (groundModel && groundModel->isReady()) {
        float sizeZ = groundModel->getBounds().getSize().z / 10.0f;
        if (sizeZ > 0.0f) return sizeZ;
    }
    return 1.0f;
}

void LevelGenerator::onStart() {
    mSpaceship = scriptEntity.getScene()->getEntity("Spaceship");

    Strike::Entity gmEntity = scriptEntity.getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("LevelGenerator: GameManager not found");
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

    float shipZ          = mSpaceship.getWorldPosition().z;
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

    mSmoothedAmplitude = 0.0f;
    mNextSpawnZ        = 0.0f;
}

void LevelGenerator::updateObstacleReaction(float deltaTime) {
    if (!mGameManager || mObstacles.empty()) return;

    // Read cached raw value from GameManager, smooth with own tuning params
    float raw         = mGameManager->getRawAmplitude();
    float smoothSpeed = (raw > mSmoothedAmplitude) ? mSmoothAttack : mSmoothRelease;
    mSmoothedAmplitude = glm::mix(mSmoothedAmplitude, raw,
                                  glm::clamp(smoothSpeed * deltaTime, 0.0f, 1.0f));

    float t = glm::clamp(mSmoothedAmplitude / glm::max(mPeakRMS, 0.001f), 0.0f, 1.0f);

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

    auto groundTmpl = Strike::AssetManager::get().getAsset<Strike::Template>("ground_tmpl");
    if (groundTmpl && groundTmpl->isReady()) {
        Strike::Entity seg = scene->createEntity();
        seg.setTag("GroundSegment");
        groundTmpl->instantiate(seg);
        seg.setWorldPosition(pos);

        int   turns = s_rotSteps(s_rng);
        float yRot  = static_cast<float>(turns * 90);
        seg.setEulerAngles(glm::vec3(0.0f, yRot, 0.0f));

        glm::vec3 scale(1.0f);
        if (turns % 2 == 0) scale.x = 100.0f;
        else                 scale.z = 100.0f;
        seg.setScale(scale);

        mSegments.push_back(seg);
    }

    if (!mSuppressObstacles) {
        static std::uniform_int_distribution<int> s_spawnChoice{ 0, 1 };
        if (s_spawnChoice(s_rng) == 0)
            spawnObstacleInGrid(pos);
        else
            spawnCoinInGrid(pos);
    }
}

Strike::Entity LevelGenerator::spawnRockFromModel(const char* modelId, const glm::vec3& pos,
                                                   float desiredSizeY) {
    auto* scene = getEntity().getScene();
    if (!scene) return Strike::Entity{};

    float cellWidth = kLaneWidth     / static_cast<float>(kGridCols);
    float cellDepth = mSegmentLength / static_cast<float>(kGridRows);

    Strike::Entity rock = scene->createEntity();
    rock.setTag("Obstacle");

    auto& renderer = rock.addComponent<Strike::RendererComponent>();
    renderer.setModel(modelId);

    auto model = Strike::AssetManager::get().getAsset<Strike::Model>(modelId);

    glm::vec3 scale(1.0f);
    if (model && model->isReady()) {
        glm::vec3 boundsSize = model->getBounds().getSize();
        scale.x = (boundsSize.x > 0.0f) ? (cellWidth / boundsSize.x) : 1.0f;
        scale.z = (boundsSize.z > 0.0f) ? (cellDepth / boundsSize.z) : 1.0f;
        scale.y = (boundsSize.y > 0.0f) ? (desiredSizeY / boundsSize.y) : 1.0f;
    }

    static std::uniform_int_distribution<int> s_blueDist(100, 255);
    static std::uniform_int_distribution<int> s_dimDist(0, 60);

    renderer.setColor(glm::vec3(s_dimDist(s_rng), s_dimDist(s_rng), s_blueDist(s_rng)));

    rock.setWorldPosition(pos);
    rock.setScale(scale);

    return rock;
}

glm::vec3 LevelGenerator::randomCellPosition(const glm::vec3& segmentPos) const {
    float cellWidth = kLaneWidth     / static_cast<float>(kGridCols);
    float cellDepth = mSegmentLength / static_cast<float>(kGridRows);

    std::uniform_int_distribution<int> colDist(0, kGridCols - 1);
    std::uniform_int_distribution<int> rowDist(0, kGridRows - 1);

    int col = colDist(s_rng);
    int row = rowDist(s_rng);

    float x = -kLaneWidth * 0.5f + (col + 0.5f) * cellWidth;
    float z =  segmentPos.z - (row + 0.5f) * cellDepth;

    return glm::vec3(x, 0.0f, z);
}

void LevelGenerator::spawnObstacleInGrid(const glm::vec3& segmentPos) {
    float     scaleY = s_yScale(s_rng);
    glm::vec3 pos    = randomCellPosition(segmentPos);

    int variant = s_rockVariant(s_rng);

    Strike::Entity rock;
    switch (variant) {
        case 1: rock = spawnRockFromModel("rock1_model", pos, scaleY); break;
        case 2: rock = spawnRockFromModel("rock2_model", pos, scaleY); break;
        case 3: rock = spawnRockFromModel("rock3_model", pos, scaleY); break;
        case 4: rock = spawnRockFromModel("rock4_model", pos, scaleY); break;
        case 5: rock = spawnRockFromModel("rock5_model", pos, scaleY); break;
        default: return;
    }

    if (!rock.isValid()) return;

    auto& physics = rock.addComponent<Strike::PhysicsComponent>();
    physics.setAnchored(true);
    physics.setCanCollide(true);

    mObstacleBaseScaleY.push_back(rock.getScale().y);
    mObstacles.push_back(rock);
}

void LevelGenerator::spawnCoinInGrid(const glm::vec3& segmentPos) {
    auto* scene = getEntity().getScene();
    if (!scene) return;

    auto coinTmpl = Strike::AssetManager::get().getAsset<Strike::Template>("coin_tmpl");
    if (!coinTmpl || !coinTmpl->isReady()) return;

    Strike::Entity coin = scene->createEntity();
    coin.setTag("Coin");

    if (!coinTmpl->instantiate(coin)) return;

    glm::vec3 pos = randomCellPosition(segmentPos);
    coin.setWorldPosition(glm::vec3(pos.x, 10.0f, pos.z));
    coin.setScale(glm::vec3(0.1f));

    auto coinModel = Strike::AssetManager::get().getAsset<Strike::Model>("coin");
    if (coinModel && coinModel->isReady()) {
        auto& physics  = coin.addComponent<Strike::PhysicsComponent>();
        glm::vec3 size = coinModel->getBounds().getSize();
        physics.setSize(glm::vec3(size.x, size.z, size.y) * 0.1f);
        physics.setAnchored(true);
        physics.setCanCollide(true);
    }

    coin.getOrAddComponent<Strike::LogicComponent>().addScript<CoinSpin>();

    mCoins.push_back(coin);
}

void LevelGenerator::cleanupBehindShip() {
    if (!mSpaceship.isValid()) return;

    float destroyThreshold = mSpaceship.getWorldPosition().z + kDestroyBehind;

    auto destroyFront = [&](std::vector<Strike::Entity>& list) {
        while (!list.empty()) {
            Strike::Entity& entity = list.front();
            if (!entity.isValid()) {
                list.erase(list.begin());
                continue;
            }
            if (entity.getWorldPosition().z < destroyThreshold) break;
            entity.destroy();
            list.erase(list.begin());
        }
    };

    destroyFront(mSegments);
    destroyFront(mCoins);

    while (!mObstacles.empty()) {
        Strike::Entity& obs = mObstacles.front();
        if (!obs.isValid()) {
            mObstacles.erase(mObstacles.begin());
            mObstacleBaseScaleY.erase(mObstacleBaseScaleY.begin());
            continue;
        }
        if (obs.getWorldPosition().z < destroyThreshold) break;
        obs.destroy();
        mObstacles.erase(mObstacles.begin());
        mObstacleBaseScaleY.erase(mObstacleBaseScaleY.begin());
    }
}

REGISTER_SCRIPT(LevelGenerator)