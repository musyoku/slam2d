#pragma once

namespace glgui {
namespace header {
    class Parameters {
    public:
        int _num_beams;
        float _speed;
        Parameters(int num_beams, float speed);
        ~Parameters();
        void render();
    };
}
}