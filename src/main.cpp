#include "bsp.h"

#include <cstring>
#include <iostream>
#include <climits>

// this makes two modifications to entity data to stop bspsrc:
// 1. an invalid info_overlay_accessor entity is added to trigger an exception in bspsrc
// 2. invalid braces are added to exploit a difference in the way bspsrc and the game
//    parse braces with this structure: { both-alive { both-ignore } } { alive-only-in-game } { both-alive }
void mangle_entity_data(bsp::Map& map) {
    auto& entity_lump = map.lumps[0];
    std::string raw_ents(entity_lump.data.data(), entity_lump.data.size());
    std::string result = "{\"classname\" \"info_overlay_accessor\"\n\"OverlayID\" \"don't steal maps\"}";
    bool in_string = false;
    for (char c : raw_ents) {
        if (c == '"') in_string = !in_string;
        else if (c == '{' && !in_string) {
            result += "{ \"classname\" \".\" { \"comment\" \"hey Nico please don't patch this one, people are stealing maps ):\" } }";
        }
        result += c;
    }
    entity_lump.data.resize(result.size());
    std::memcpy(entity_lump.data.data(), result.data(), result.size());
}

#pragma pack(push, 1)
struct raw_brush {
    int first_side;
    int num_sides;
    int contents;
};
#pragma pack(pop)

static_assert(sizeof(raw_brush) == 12, "bad brush size");

// this marks the map's non-solid brushes with the bevel flag which
// causes bspsrc to ignore them, and marks other brushes as detail
// which makes them disappear due to entity mangling
void mangle_brushes(bsp::Map& map) {
    auto& brush_lump = map.lumps[18];
    int num_brushes = brush_lump.data.size() / 12;
    for (int i = 0; i < num_brushes; ++i) {
        raw_brush* brush = reinterpret_cast<raw_brush*>(brush_lump.data.data() + i * sizeof(raw_brush));
        if ((brush->contents & 1) == 0 && (brush->contents & 0x10000) == 0) {
            // brush is not solid nor playerclip
            auto& bside_lump = map.lumps[19];
            int num_bsides = bside_lump.data.size() / 8;
            for (int j = brush->first_side; j < brush->first_side + brush->num_sides; ++j) {
                int bevel_offset = j * 8 + 6;
                short* bevel = reinterpret_cast<short*>(bside_lump.data.data() + bevel_offset);
                *bevel = 1;
            }
        } else {
            brush->contents |= 0x8000000; // make solids func_detail
        }
    }
}

// bspsource dies if the compression method is wrong,
// but the game doesn't seem to care
void mangle_pakfile(bsp::Map& map) {
    auto& pak_lump = map.lumps[40];
    if (pak_lump.data.empty()) return;
    unsigned short* compression_method = reinterpret_cast<unsigned short*>(pak_lump.data.data() + 8);
    *compression_method = USHRT_MAX;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "This program cannot be run by double-clicking it.\n";
        std::cerr << "Drag a .bsp file onto this program's icon in Windows explorer.\n";
        std::cerr << "\n<press enter to dismiss this message>\n";
        std::cin.get();
        return 1;
    }
    bsp::Map map{std::string(argv[1])};
    mangle_entity_data(map);
    mangle_brushes(map);
    mangle_pakfile(map);
    map.save(std::string(argv[1]) + "_protected.bsp");
}
