#include "Bom.h"

namespace {
    struct BomSequence {
        Bom bom;
        std::vector<char> sequence;
    };

    static const std::vector<BomSequence> boms {
        // Long sequences first
        {Bom::Utf32Be, {'\x00','\x00','\xFE','\xFF'}},
        {Bom::Utf32Le, {'\xFF','\xFE','\x00','\x00'}},
        {Bom::Utf8, {'\xEF','\xBB','\xBF'}},
        {Bom::Utf16Be, {'\xFE','\xFF'}},
        {Bom::Utf16Le, {'\xFF','\xFE'}},
    };
}

Bom findBom(const std::vector<char>& data)
{
    auto find = [&data](const std::vector<char>& f) ->bool {
        if (f.empty()) {
            return false;
        }

        if (f.size() > data.size()) {
            return false;
        }

        for (std::size_t i = 0; i < f.size(); i++) {
            if (data[i] != f[i]) {
                return false;
            }
        }
        return true;
    };

    for(const auto& bom: boms) {
        if (find(bom.sequence)) {
            return bom.bom;
        }
    }
    return Bom::Unknown;
}

std::size_t bomSize(Bom bom)
{
    for(const auto& b: boms) {
        if (b.bom == bom) {
            return b.sequence.size();
        }
    }
    return 0;
}
