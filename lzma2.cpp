#include "lzma2.h"

void* MyISzAlloc(ISzAllocPtr, size_t size) {
    return MyAlloc(size);  // call 1-arg SDK version
}

void MyISzFree(ISzAllocPtr, void* address) {
    MyFree(address);  // call 1-arg SDK version
}

#define LZMA2_PROPS_SIZE 1

blob LZMA2::encode(blob input, int level, int dictSize, int lc, int lp, int pb, int fb, int blockSize, int numTotalThreads, int numBlockThreads_Max, int numBlockThreads_Reduced) {
    ISzAlloc alloc = { MyISzAlloc, MyISzFree };

    CLzma2EncProps props;
    Lzma2EncProps_Init(&props);

    /* dictionary is not shared between blocks, but input.size() / threads is not (1 << N), not power of 2 */
    props.blockSize = blockSize;
    props.numTotalThreads = numTotalThreads;
    props.numBlockThreads_Max = numBlockThreads_Max;
    props.numBlockThreads_Reduced = numBlockThreads_Reduced;

    props.lzmaProps.level = level;
    props.lzmaProps.dictSize = dictSize;
    props.lzmaProps.lc = lc;
    props.lzmaProps.lp = lp;
    props.lzmaProps.pb = pb;
    props.lzmaProps.fb = fb;

    CLzma2EncHandle enc = Lzma2Enc_Create(&alloc, &alloc);
    if (!enc) {
        throw std::runtime_error("LZMA2 encoder creation failed");
    }

    Lzma2Enc_SetProps(enc, &props);

    std::vector<unsigned char> propsData(LZMA2_PROPS_SIZE);
    size_t propsSize = LZMA2_PROPS_SIZE;
    propsData[0] = Lzma2Enc_WriteProperties(enc);

    blob encoded(input.size() + LZMA2_PROPS_SIZE);
    auto encodedSize = encoded.size();

    ICompressProgress progress;

    int res = Lzma2Enc_Encode2(
        enc,
        nullptr,
        encoded.data() + LZMA2_PROPS_SIZE, &encodedSize,
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
            encoded.data() + LZMA2_PROPS_SIZE, &encodedSize,
            nullptr,               
            input.data(), input.size(),
            nullptr
        );
    }

    Lzma2Enc_Destroy(enc);

    memcpy(encoded.data(), &propsData[0], LZMA2_PROPS_SIZE);

    encoded.resize(encodedSize + LZMA2_PROPS_SIZE);

    return encoded;
}

blob LZMA2::decode(blob encoded) {
    unsigned char propsByte = encoded[0];

    CLzma2Dec decoder;
    Lzma2Dec_Construct(&decoder);
    SRes res = Lzma2Dec_Allocate(&decoder, propsByte, &g_Alloc);
    if (res != SZ_OK)
        throw std::runtime_error("Decoder allocate failed");

    Lzma2Dec_Init(&decoder);

    const Byte* inBuf = encoded.data() + LZMA2_PROPS_SIZE;
    size_t inSize = encoded.size() - LZMA2_PROPS_SIZE;

    size_t outBufSize = 10 * encoded.size();
    blob outBuf(outBufSize);

    size_t inPos = 0;
    size_t outPos = 0;

    ELzmaStatus status;

    while (true) {
        SizeT inProcessed = inSize - inPos;
        SizeT outProcessed = outBufSize - outPos;

        res = Lzma2Dec_DecodeToBuf(&decoder,
            outBuf.data() + outPos, &outProcessed,
            inBuf + inPos, &inProcessed,
            LZMA_FINISH_ANY,
            &status);

        if (res != SZ_OK)
            throw std::runtime_error("Decoding failed");

        inPos += inProcessed;
        outPos += outProcessed;

        if (status == LZMA_STATUS_FINISHED_WITH_MARK) {
            break;
        }

        if (inProcessed == 0 && outProcessed == 0) {
            /* TODO: handle no progress*/
            outBufSize *= 2;
            outBuf.resize(outBufSize);
        }
    }

    Lzma2Dec_Free(&decoder, &g_Alloc);

    outBuf.resize(outPos);

    return outBuf;
}