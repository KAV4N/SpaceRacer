#include "CoinSpin.h"

void CoinSpin::onStart() {
    mTop = scriptEntity.getChildren()[0];

    auto children = mTop.getChildren();
    for (auto& child : children) {
        const std::string& tag = child.getTag();
        if (tag == "Bottom_Material") mBottom = child;
        else if (tag == "Centre_Material") mCentre = child;
    }
}

void CoinSpin::onUpdate(float dt) {
    // Top rotates around its own Y axis
    if (mTop.isValid()) {
        mTop.rotate(glm::vec3(0.0f, mTopSpeed * dt, 0.0f));
    }
    // Centre spins the opposite direction, faster
    if (mCentre.isValid()) {
        mCentre.rotate(glm::vec3(0.0f, mCentreSpeed * dt, 0.0f));
    }
}

REGISTER_SCRIPT(CoinSpin)