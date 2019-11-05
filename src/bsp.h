#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace bsp {
    class Lump {
    public:
        Lump(int version, int ident) :
            version(version), ident(ident) {
        }

        std::vector<char> data;
        int version;
        int ident;
    };

    class Map {
    public:
        Map(const std::string& filename);
        void save(const std::string& filename);

        std::vector<Lump> lumps;
        int revision;
    private:
        std::ifstream input;
        int original_game_lump_offset;
    };
}
