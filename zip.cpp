#include "zip.h"

blob ZIP::encode(blob input, int level, int method, int windowBits, \
    int memLevel, int strategy) {
    z_stream stream{};
    if (deflateInit2(&stream, level, method, windowBits, memLevel, strategy) != Z_OK) {
        throw std::runtime_error("deflateInit2 failed");
    }

    blob encoded;
    encoded.resize(compressBound(input.size()));

    stream.avail_in = input.size();
    stream.next_in = const_cast<Bytef*>(input.data());

    stream.avail_out = encoded.size();
    stream.next_out = encoded.data();

    int res = deflate(&stream, Z_FINISH);
    if (res != Z_STREAM_END) {
        deflateEnd(&stream);
        throw std::runtime_error("deflate failed");
    }

    size_t compressedSize = stream.total_out;
    deflateEnd(&stream);

    encoded.resize(compressedSize);
    return encoded;
}

blob ZIP::decode(blob encoded) {
    z_stream stream{};
    if (inflateInit(&stream) != Z_OK) {
        throw std::runtime_error("inflateInit failed");
    }

    uLong decodedSize = encoded.size() * 4;
    blob decoded(decodedSize);

    stream.avail_in = encoded.size();
    stream.next_in = const_cast<Bytef*>(encoded.data());

    stream.avail_out = decoded.size();
    stream.next_out = decoded.data();

    int res = inflate(&stream, Z_FINISH);
    while (res == Z_BUF_ERROR) {
        decodedSize *= 2;
        decoded.resize(decodedSize);
        stream.avail_out = decoded.size() - stream.total_out;
        stream.next_out = decoded.data() + stream.total_out;
        res = inflate(&stream, Z_FINISH);
    }

    if (res != Z_STREAM_END) {
        inflateEnd(&stream);
        throw std::runtime_error("inflate failed with code " + std::to_string(res));
    }

    decoded.resize(stream.total_out);
    inflateEnd(&stream);

    return decoded;
}
