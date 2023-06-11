#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCImage.hpp>

// ported from https://github.com/cgytrus/GeometryCache

#include <ghc/filesystem.hpp>
#include <qoixx.hpp>

const CCImage::EImageFormat kFmtQoi = (CCImage::EImageFormat)69;

struct QoiCachedImage : geode::Modify<QoiCachedImage, CCImage> {
    void cacheImage(ghc::filesystem::file_time_type qoiWriteTime, ghc::filesystem::path qoiPath) {
        ZoneScoped;

        qoixx::qoi::desc desc {
            .width = m_nWidth,
            .height = m_nHeight,
            .channels = m_bHasAlpha ? (uint8_t)4u : (uint8_t)3u,
            .colorspace = m_bPreMulti ? qoixx::qoi::colorspace::linear : qoixx::qoi::colorspace::srgb
        };

        try {
            auto encoded = qoixx::qoi::encode<std::vector<char>>(m_pData, desc.width * desc.height * desc.channels, desc);

            std::ofstream qoiFile;
            qoiFile.open(qoiPath, std::ios::out | std::ios::trunc | std::ios::binary);
            qoiFile.write(encoded.data(), encoded.size());
            qoiFile.close();

            // used instead of actual hash for better performance
            ghc::filesystem::last_write_time(qoiPath, qoiWriteTime);
        }
        catch(...) { }
    }

    bool initWithImageFile(const char* strPath, CCImage::EImageFormat imageType) {
        ZoneScoped;

        try {
            auto qoiWriteTime = ghc::filesystem::last_write_time(strPath);
            auto qoiPath = ghc::filesystem::path(strPath).replace_extension("qoi");

            if(ghc::filesystem::exists(qoiPath) && ghc::filesystem::last_write_time(qoiPath) == qoiWriteTime)
                return CCImage::initWithImageFile(qoiPath.string().c_str(), kFmtQoi);

            auto success = CCImage::initWithImageFile(strPath, imageType);
            if(success)
                cacheImage(qoiWriteTime, qoiPath);
            return success;
        }
        catch(...) {
            log::error("Error when caching {}", strPath);
            return CCImage::initWithImageFile(strPath, imageType);
        }
    }

    bool initWithImageFileThreadSafe(const char* strPath, CCImage::EImageFormat imageType) {
        ZoneScoped;

        try {
            auto qoiWriteTime = ghc::filesystem::last_write_time(strPath);
            auto qoiPath = ghc::filesystem::path(strPath).replace_extension("qoi");

            if(ghc::filesystem::exists(qoiPath) && ghc::filesystem::last_write_time(qoiPath) == qoiWriteTime)
                return CCImage::initWithImageFileThreadSafe(qoiPath.string().c_str(), kFmtQoi);

            auto success = CCImage::initWithImageFileThreadSafe(strPath, imageType);
            if(success)
                cacheImage(qoiWriteTime, qoiPath);
            return success;
        }
        catch(...) {
            log::error("Error when caching {}", strPath);
            return CCImage::initWithImageFileThreadSafe(strPath, imageType);
        }
    }

    bool initWithQoiData(void* pData, int nDataLen) {
        ZoneScoped;

        try {
            auto decoded = qoixx::qoi::decode<std::vector<unsigned char>>((unsigned char*)pData, nDataLen);

            m_nWidth = decoded.second.width;
            m_nHeight = decoded.second.height;
            m_nBitsPerComponent = 8;
            m_bHasAlpha = decoded.second.channels >= 4u;
            // actually is premultiplied alpha but the linear colorspace field is not used so why not use it for this instead
            // this may make some (or all) images display weirdly in proper qoi viewers tho
            m_bPreMulti = decoded.second.colorspace == qoixx::qoi::colorspace::linear;

            // copy decoded image to CCImage
            m_pData = new unsigned char[decoded.first.size()];
            for(size_t i = 0; i < decoded.first.size(); ++i)
                m_pData[i] = decoded.first[i];

            return true;
        }
        catch(...) {
            log::error("Error when loading QOI image");
            return false;
        }
    }

    bool initWithImageData(void* pData, int nDataLen, CCImage::EImageFormat eFmt, int nWidth, int nHeight, int nBitsPerComponent) {
        ZoneScoped;
        return eFmt == kFmtQoi ?
            pData && nDataLen > 0 && initWithQoiData(pData, nDataLen) :
            CCImage::initWithImageData(pData, nDataLen, eFmt, nWidth, nHeight, nBitsPerComponent);
    }
};
