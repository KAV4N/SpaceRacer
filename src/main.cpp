#include "StrikeEngine.h"

#include "scripts/GameManager.h"
#include "scripts/SpaceshipController.h"
#include "scripts/CameraFollow.h"
#include "scripts/LevelGenerator.h"

int main(int argc, char** argv) {
    Strike::Application app;
    Strike::Window& window = app.getWindow();
    window.setWindowTitle("SpaceRacer");
    window.setSize(1280, 720);
    window.setVSync(true);

    Strike::World::get().loadScene("assets/scenes/game.xml");

    app.run();

    return 0;
}
