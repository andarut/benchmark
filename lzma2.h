#include "utils.h"

extern "C" {
#include "Lzma2Enc.h"
#include "Lzma2Dec.h"
#include "Alloc.h"
#include "7zTypes.h"
}

class LZMA2 {
public:
    LZMA2() = default;

    static blob encode(blob input, int level, unsigned dictSize, int lc, int lp, int pb, int fb, int numThreads);
    static blob decode(blob encoded);

    /* presets */
    static blob encodeDefault(blob input) {
        return encode(input, 5, 1 << 24, 3, 0, 2, 32, 2); // default params from LzmaLib.h
    }

    static blob encodeMax(blob input) {
        return encode(input, 9, 1 << 30, 8, 0, 2, 64, 16); // trying maximum gain
    }
};