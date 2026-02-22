#include "StrikeEngine.h"


int main(int argc, char** argv) {
    Strike::Application app;
    Strike::Window& window = app.getWindow();
    window.setSize(1920,1080);

    app.run();

    return 0;
}