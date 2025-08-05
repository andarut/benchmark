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

    static blob encode(blob input, int level, int dictSize, int lc, int lp, int pb, int fb, int blockSize, int numTotalThreads, int numBlockThreads_Max, int numBlockThreads_Reduced);
    static blob decode(blob encoded);

    /* presets */
    static blob encodeDefault(blob input) {
        return encode(input, 5, 1 << 24, 3, 0, 2, 32, 1 << 25, 2, 2, 2); // default params
    }

    static blob encodeMain(blob input) {
        int n = std::thread::hardware_concurrency();
        return encode(input, 5, 1 << 24, 3, 0, 2, 32, 1 << 25, n, n/2, n/4); // main params
    }

    static blob encodeMax(blob input) {
        return encode(input, 9, 1 << 30, 8, 0, 2, 64, 1 << 25, 16, 8, 4); // trying maximum gain
    }
};