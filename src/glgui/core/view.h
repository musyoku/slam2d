#pragma once

namespace glgui {
namespace view {
    class View {
    public:
        View(){};
        virtual void render(int x, int y, int width, int height){};
    };
}
}