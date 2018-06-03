#pragma once

namespace glgui {
namespace header {
    class Parameters {
    public:
        int _num_beams;
        Parameters(int num_beams);
        ~Parameters();
        void render();
    };
}
}