#include <threepp/threepp.hpp>
#include "logic/Game.h"

#include <chrono>

using namespace threepp;

int main() {
    Canvas canvas("Parking Quest");
    GLRenderer renderer(canvas.size());

    Game game(canvas, renderer);

    using clock = std::chrono::steady_clock;
    auto last = clock::now();

    canvas.animate([&] {
        float dt = std::chrono::duration<float>(clock::now() - last).count();
        last = clock::now();
        if (dt > 0.1f) dt = 0.1f;

        game.update(dt);
        game.render();
    });

    return 0;
}
