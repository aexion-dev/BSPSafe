#include "bsp.h"
#include "util.h"

#include <fstream>
#include <cassert>
#include <cstddef>
#include <utility> 

static_assert(sizeof(int) == 4, "BSP structures assume 4-byte integers");

constexpr int VBSP_IDENT = 0x50534256;
constexpr std::size_t HEADER_LUMPS = 64;

#pragma pack(push, 1)
struct raw_lump {
    int offset;
    int length;
    int version;
    int ident;
};

struct raw_bsp_header {
    int	ident;
    int	version;
    raw_lump lumps[HEADER_LUMPS];
    int revision;
};
#pragma pack(pop)

static_assert(sizeof(raw_bsp_header) == 1036, "bad BSP header size");

bsp::Map::Map(const std::string& filename) {
    std::ifstream file{filename, std::ios_base::in | std::ios_base::binary};
    if (!file.good()) fatal_error("could not open file '" + filename + "' for reading");
    char header[sizeof(raw_bsp_header)];
    file.read(header, sizeof(raw_bsp_header));
    auto raw_bsp = reinterpret_cast<raw_bsp_header*>(header);
    if (raw_bsp->ident != VBSP_IDENT) fatal_error("BSP file must start with 'VBSP'");
    if (raw_bsp->version != 21) fatal_error("only BSP version 21 (CS:GO) is supported");
    revision = raw_bsp->revision;
    for (std::size_t i = 0; i < HEADER_LUMPS; ++i) {
        raw_lump* raw_lump = &raw_bsp->lumps[i];
        Lump lump{raw_lump->version, raw_lump->ident};
        lump.data.resize(raw_lump->length);
        file.seekg(raw_lump->offset, std::ios_base::beg);
        file.read(lump.data.data(), raw_lump->length);
        lumps.push_back(std::move(lump));
        if (i == 35) original_game_lump_offset = raw_lump->offset;
    }
    input = std::move(file);
}

void bsp::Map::save(const std::string& filename) {
    std::ofstream file{filename, std::ios_base::out | std::ios_base::binary};
    if (!file.good()) fatal_error("could not open file '" + filename + "' for writing");
    
    raw_bsp_header raw_bsp;
    raw_bsp.ident = VBSP_IDENT;
    raw_bsp.version = 21;
    raw_bsp.revision = revision;
    std::size_t running_offset = sizeof(raw_bsp_header);
    for (std::size_t i = 0; i < HEADER_LUMPS; ++i) {
        auto& lump = raw_bsp.lumps[i];
        lump.ident = lumps[i].ident;
        lump.version = lumps[i].version;
        lump.length = lumps[i].data.size();
        lump.offset = running_offset;
        running_offset += lump.length;
    }
    // fix game lump
    auto& game_lump = lumps[35];
    int game_lump_count = *reinterpret_cast<int*>(game_lump.data.data());
    for (int i = 0; i < game_lump_count; ++i) {
        int offset_field_offset = 4 + (16 * i) + 8;
        int* offset = reinterpret_cast<int*>(game_lump.data.data() + offset_field_offset);
        *offset -= original_game_lump_offset;
        *offset += raw_bsp.lumps[35].offset;
    }
    // write header
    file.write(reinterpret_cast<char*>(&raw_bsp), sizeof(raw_bsp_header));
    // write lumps
    for (std::size_t i = 0; i < HEADER_LUMPS; ++i) {
        file.write(lumps[i].data.data(), lumps[i].data.size());
    }
}

