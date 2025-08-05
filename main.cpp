#include "utils.h"

#include "lzma.h"
#include "lzma2.h"
#include "zip.h"

/* RANDOM DATA */
// auto g_inputSize = 1024 * 1024; // 1 MB
// auto g_input = randomData(inputSize);

/* REAL DATA */
// auto g_input = readAllFilesRecursively("./../FileStore");
auto g_input = readFile("./../test.bin");

auto g_inputSize = g_input.size();

void testCase(const std::string& title, std::function<blob(blob)> encoder, std::function<blob(blob)> decoder) {
    std::cout << "========================= " << title << " =========================" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    auto encoding_end = start;
    auto decoding_end = start;

    auto encoded = encoder(g_input);
    encoding_end = std::chrono::high_resolution_clock::now();
    std::cout << "ENCODED_SIZE = " << encoded.size() << " bytes" << std::endl;
    auto encoding_duration = std::chrono::duration_cast<std::chrono::seconds>(encoding_end - start);
    std::cout << "ENCODING_TIME: " << encoding_duration.count() << " s\n";

    auto decoded = decoder(encoded);
    decoding_end = std::chrono::high_resolution_clock::now();
    std::cout << "DECODED_SIZE = " << decoded.size() << " bytes" << std::endl;
    auto decoding_duration = std::chrono::duration_cast<std::chrono::seconds>(decoding_end - encoding_end);
    std::cout << "DECODING_TIME: " << decoding_duration.count() << " s\n";

    if (g_input != decoded) {
        std::cerr << "NO LOSSLESS" << std::endl;
        return;
    }

    double ratio = ((double)g_input.size() / (double)encoded.size());
    std::cout << "RATIO = " << std::setprecision(2) << ratio << std::endl;
    
}

int main() {

    std::cout << "INPUT SIZE = " << g_input.size() << " bytes" << std::endl;

    // std::ofstream file("test.bin", std::ios::binary);
    // file.write(reinterpret_cast<const char*>(g_input.data()), g_input.size());

    /* LZMA */
    testCase("LZMA DEFAULT", LZMA::encodeDefault, LZMA::decode);
    testCase("LZMA MAX", LZMA::encodeMax, LZMA::decode);

    /* LZMA2 */
    testCase("LZMA2 DEFAULT", LZMA2::encodeDefault, LZMA2::decode);
    testCase("LZMA2 MAIN", LZMA2::encodeMain, LZMA2::decode);
    testCase("LZMA2 MAX", LZMA2::encodeMax, LZMA2::decode);

    /* ZLIB */
    testCase("ZIP DEFAULT", ZIP::encodeDefault, ZIP::decode);
    testCase("ZIP MAX", ZIP::encodeMax, ZIP::decode);

    return 0;
}
