#include "CoinSpin.h"

void CoinSpin::onStart() {
    auto children = scriptEntity.getChildren();
    if (children.empty()) return;

    mTop = children[0];

    for (auto& child : mTop.getChildren()) {
        if (child.getTag() == "Centre_Material") {
            mCentre = child;
            break;
        }
    }
}

void CoinSpin::onUpdate(float deltaTime) {
    if (mTop.isValid()) {
        mTop.rotate(glm::vec3(0.0f, mTopSpeed * deltaTime, 0.0f));
    }

    if (mCentre.isValid()) {
        mCentre.rotate(glm::vec3(0.0f, mCentreSpeed * deltaTime, 0.0f));
    }
}

REGISTER_SCRIPT(CoinSpin)