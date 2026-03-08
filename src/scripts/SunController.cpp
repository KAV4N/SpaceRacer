#include "SunController.h"
#include "GameManager.h"

static const struct {
    float      t;
    glm::uvec3 color;
    float      intensity;
} kKeyframes[] = {
    { 0.00f, {   0,   0,   0 }, 0.00f },   //   0° - pitch black, silence
    { 0.40f, {  80,  20,   0 }, 0.03f },   //  52° - barely visible deep red
    { 0.65f, { 180,  60,  10 }, 0.08f },   //  84° - faint blood orange glow
    { 0.80f, { 255,  80,  20 }, 0.18f },   // 104° - sunrise color, still dim
    { 0.90f, { 255, 160,  60 }, 0.45f },   // 117° - warm orange, getting loud
    { 0.95f, { 255, 220, 120 }, 0.75f },   // 123° - golden, almost peak
    { 1.00f, { 255, 255, 255 }, 1.00f },   // 130° - pure white noon, max volume
};

static constexpr int kKeyframeCount = 7;

glm::uvec3 SunController::lerpColor(const glm::uvec3& a,
                                    const glm::uvec3& b, float t) const {
    return glm::uvec3(
        static_cast<unsigned int>(glm::mix(float(a.r), float(b.r), t)),
        static_cast<unsigned int>(glm::mix(float(a.g), float(b.g), t)),
        static_cast<unsigned int>(glm::mix(float(a.b), float(b.b), t))
    );
}

SunController::SkyState SunController::evaluateSky(float t) const {
    t = glm::clamp(t, 0.0f, 1.0f);

    t = glm::pow(t, 1.5f);  // between linear and square

    int hi = 1;
    while (hi < kKeyframeCount - 1 && kKeyframes[hi].t < t) ++hi;
    int lo = hi - 1;

    float span  = kKeyframes[hi].t - kKeyframes[lo].t;
    float local = (span > 0.0f) ? (t - kKeyframes[lo].t) / span : 0.0f;

    SkyState s;
    s.sunAngle  = t * 130.0f;
    s.intensity = glm::clamp(
        glm::mix(kKeyframes[lo].intensity, kKeyframes[hi].intensity, local),
        0.0f, 1.0f
    );
    s.sunColor = lerpColor(kKeyframes[lo].color, kKeyframes[hi].color, local);
    return s;
}

void SunController::onStart() {
    Strike::Entity gmEntity = getEntity().getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("SunController: GameManager not found");
    }
}

void SunController::onUpdate(float deltaTime) {
    if (mGameManager) {
        float raw         = mGameManager->getRawAmplitude();
        float smoothSpeed = (raw > mSmoothedAmplitude) ? mSmoothAttack : mSmoothRelease;
        mSmoothedAmplitude = glm::mix(mSmoothedAmplitude, raw,
                                      glm::clamp(smoothSpeed * deltaTime, 0.0f, 1.0f));
    }

    float t = glm::clamp(mSmoothedAmplitude / glm::max(mPeakRMS, 0.001f), 0.0f, 1.0f);

    SkyState sky = evaluateSky(t);

    auto* scene = getEntity().getScene();
    if (!scene) return;

    Strike::Sun& sun = scene->getSun();
    sun.setRotationEuler(glm::vec3(sky.sunAngle, -30.0f, 0.0f));
    sun.setColor(sky.sunColor);
    sun.setIntensity(sky.intensity);
}

REGISTER_SCRIPT(SunController)