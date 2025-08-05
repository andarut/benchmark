#include "lzma.h"

blob LZMA::encode(blob input, int level, unsigned dictSize, int lc, int lp, int pb, int fb, int numThreads) {
    blob encoded(input.size() * 2 + LZMA_PROPS_SIZE);

    auto encodedSize = encoded.size();

    unsigned char props[LZMA_PROPS_SIZE];
    size_t propsSize = LZMA_PROPS_SIZE;

    auto res = LzmaCompress(
        encoded.data() + LZMA_PROPS_SIZE, &encodedSize, // dest
        reinterpret_cast<const unsigned char*>(input.data()), input.size(), // src
        props, &propsSize, 
        level, // compression level
        dictSize, // dict size (default 16 KB)
        lc, // number of literal context bits
        lp, // number of literal pos bits
        pb, // number of pos bits
        fb, // word size
        numThreads // number of threads
    );

    if (res != SZ_OK) {
        std::cerr << "Compressor failed: " << res << "\n";
        return {};
    }

    memcpy(encoded.data(), props, LZMA_PROPS_SIZE);

    encoded.resize(encodedSize + LZMA_PROPS_SIZE);

    return encoded;
}

blob LZMA::decode(blob encoded) {
    /* reallocate when SZ_ERROR_OUTPUT_BUF */
    blob decoded(190 * 1024 * 1024);
    
    size_t decodedSize = decoded.size();
    size_t encodedSize = encoded.size() - LZMA_PROPS_SIZE;

    unsigned char props[LZMA_PROPS_SIZE];
    size_t propsSize = LZMA_PROPS_SIZE;

    memcpy(props, encoded.data(), LZMA_PROPS_SIZE);

    auto res = LzmaUncompress(
        decoded.data(), &decodedSize,
        encoded.data() + LZMA_PROPS_SIZE, &encodedSize,
        props, propsSize
    );
    
    if (res != SZ_OK && res != SZ_ERROR_INPUT_EOF) {
        std::cerr << "Decoder failed: " << res << "\n";
        return {}; 
    }

    decoded.resize(decodedSize);

    return decoded;
}