#include "lzma2.h"

void* MyISzAlloc(ISzAllocPtr, size_t size) {
    return MyAlloc(size);  // call 1-arg SDK version
}

void MyISzFree(ISzAllocPtr, void* address) {
    MyFree(address);  // call 1-arg SDK version
}

blob LZMA2::encode(blob input, int level, int dictSize, int lc, int lp, int pb, int fb, int numThreads) {
    ISzAlloc alloc = { MyISzAlloc, MyISzFree };

    CLzma2EncProps props;
    Lzma2EncProps_Init(&props);

    props.blockSize = dictSize; /* dictionary is not shared between blocks */
    props.numTotalThreads = numThreads;
    props.numBlockThreads_Max = numThreads/2;
    props.numBlockThreads_Reduced = 2;


    props.lzmaProps.level = level;
    props.lzmaProps.dictSize = dictSize;
    props.lzmaProps.lc = lc;
    props.lzmaProps.lp = lp;
    props.lzmaProps.pb = pb;
    props.lzmaProps.fb = fb;
    props.lzmaProps.numThreads = 2;

    CLzma2EncHandle enc = Lzma2Enc_Create(&alloc, &alloc);
    if (!enc) {
        throw std::runtime_error("LZMA2 encoder creation failed");
    }

    Lzma2Enc_SetProps(enc, &props);

    std::vector<unsigned char> propsData(1);
    size_t propsSize = propsData.size();
    propsData[0] = Lzma2Enc_WriteProperties(enc);

    blob encoded(input.size());
    auto encodedSize = encoded.size();

    ICompressProgress progress;

    int res = Lzma2Enc_Encode2(
        enc,
        nullptr,
        encoded.data(), &encodedSize,
        nullptr,               
        input.data(), input.size(),
        nullptr
    );
    while (res == SZ_ERROR_OUTPUT_EOF) {
        encodedSize = encoded.size() * 2;
        std::cout << "SZ_ERROR_OUTPUT_EOF -> resizing to " << encodedSize << std::endl;
        encoded.resize(encodedSize);
            res = Lzma2Enc_Encode2(
            enc,
            nullptr,
            encoded.data(), &encodedSize,
            nullptr,               
            input.data(), input.size(),
            nullptr
        );
    }

    Lzma2Enc_Destroy(enc);

    encoded.resize(encodedSize);

    return encoded;
}

blob LZMA2::decode(blob encoded) {
    return encoded;
}