#include "StrikeEngine.h"

int main(int argc, char** argv) {
    try {
        Strike::Application app;
        Strike::Window& window = app.getWindow();
        window.setWindowTitle("SpaceRacer");
        window.setSize(1280, 720);

        Strike::World::get().loadScene("assets/scenes/menu.xml");

        app.run();
    }
    catch (const std::runtime_error& e) {
        return 1;
    }

    return 0;
}


