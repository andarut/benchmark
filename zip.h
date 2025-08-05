#include "utils.h"
#include <zlib.h>

class ZIP {
public:
    ZIP() = default;

    static blob encode(blob input, int level, int method, int windowBits, \
    int memLevel, int strategy);
    static blob decode(blob decoded);

    static blob encodeDefault(blob input) {
        return encode(input, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
    }
};