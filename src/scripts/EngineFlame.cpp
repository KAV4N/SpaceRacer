#include "EngineFlame.h"

void EngineFlame::onStart() {
    mFlameTime = 0.0f;
}

glm::uvec3 EngineFlame::flameColor(float t) const {
    struct Stop { float pos; glm::vec3 color; };
    static const Stop stops[] = {
        { 0.00f, { 255.f, 255.f, 200.f } },
        { 0.33f, { 255.f, 220.f,   0.f } },
        { 0.66f, { 255.f, 100.f,   0.f } },
        { 1.00f, { 180.f,   0.f,   0.f } },
    };

    t = glm::clamp(t, 0.0f, 1.0f);

    for (int i = 0; i < 3; ++i) {
        if (t <= stops[i + 1].pos) {
            float local = (t - stops[i].pos) / (stops[i + 1].pos - stops[i].pos);
            glm::vec3 c = glm::mix(stops[i].color, stops[i + 1].color, local);
            return glm::uvec3(
                static_cast<unsigned int>(c.r),
                static_cast<unsigned int>(c.g),
                static_cast<unsigned int>(c.b)
            );
        }
    }
    return glm::uvec3(180, 0, 0);
}

void EngineFlame::onUpdate(float deltaTime) {
    mFlameTime += deltaTime * kFlameSpeed;
    float scroll = glm::fract(mFlameTime);

    if (hasComponent<Strike::RendererComponent>())
        getComponent<Strike::RendererComponent>().setColor(flameColor(glm::fract(scroll)));

    auto children = scriptEntity.getChildren();
    if (children.empty()) return;
    Strike::Entity mid = children[0];
    if (mid.isValid() && mid.hasComponent<Strike::RendererComponent>())
        mid.getComponent<Strike::RendererComponent>().setColor(flameColor(glm::fract(scroll + kDepthStep)));

    auto grandchildren = mid.getChildren();
    if (grandchildren.empty()) return;
    Strike::Entity inner = grandchildren[0];
    if (inner.isValid() && inner.hasComponent<Strike::RendererComponent>())
        inner.getComponent<Strike::RendererComponent>().setColor(flameColor(glm::fract(scroll + 2.0f * kDepthStep)));
}

REGISTER_SCRIPT(EngineFlame)