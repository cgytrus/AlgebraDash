//#include <GL/glew.h>
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <glew_add.h>

// TODO: a lot of addresses but is this even needed on mac?
#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/CCEGLView.hpp>

// sorry 500000000 year old hardware users 😢
constexpr int major = 3;
constexpr int minor = 2;
constexpr bool compat = false;
constexpr bool debug = true;

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char *message, const void *userParam) {
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    auto text = fmt::format("OpenGL message ({}): {}", id, message);
    TracyMessage(text.c_str(), text.size());

    auto logFunc = &log::debug<const char*>;
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: logFunc = &log::error; break;
        case GL_DEBUG_SEVERITY_MEDIUM: logFunc = &log::warn; break;
        case GL_DEBUG_SEVERITY_LOW: logFunc = &log::info; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: logFunc = &log::debug; break;
    }

    logFunc(text.c_str());

    switch(source) {
        case GL_DEBUG_SOURCE_API: logFunc("  Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: logFunc("  Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: logFunc("  Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: logFunc("  Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION: logFunc("  Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER: logFunc("  Source: Other"); break;
    }

    switch(type) {
        case GL_DEBUG_TYPE_ERROR: logFunc("  Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logFunc("  Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: logFunc("  Type: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY: logFunc("  Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE: logFunc("  Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER: logFunc("  Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP: logFunc("  Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP: logFunc("  Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER: logFunc("  Type: Other"); break;
    }

    logFunc("");
}

void vaoDrawPrepare();

struct CustomCCEGLView : geode::Modify<CustomCCEGLView, CCEGLView> {
    void setupWindow(CCRect rect) {
        ZoneScoped;

        auto _glfwWindowHint = reinterpret_cast<void(*)(int, int)>(base::getCocos() + 0x1116f0);
        _glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        _glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        _glfwWindowHint(GLFW_OPENGL_PROFILE, compat ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);
        _glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug ? GL_TRUE : GL_FALSE);

        log::debug("requesting {}.{} {} {}context", major, minor, compat ? "compat" : "core", debug ? "debug " : "");

        CCEGLView::setupWindow(rect);
    }

    bool initGlew() {
        log::debug("initializing glew");
        glewExperimental = GL_TRUE;
        if(!CCEGLView::initGlew())
            return false;

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            auto glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glewGetProcAddress((const GLubyte*)"glDebugMessageCallback");
            auto glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)glewGetProcAddress((const GLubyte*)"glDebugMessageControl");

            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        return true;
    }
};

// TODO:
// CCSpriteWithHue::draw // robtop class, seems to be used in GJShopLayer but i dont get any errors?
// unused by gd but can be used by mods:
// ccDrawPoint
// ccDrawPoints
// CCTexture2D::drawAtPoint
// CCTexture2D::drawInRect
// CCProgressTimer::draw
// ccDrawCardinalSpline
// ccDrawCubicBezier
// ccDrawQuadBezier
// CCGrid3D::blit
// CCTiledGrid3D::blit

struct ccDrawData0_t {
    bool s_bInitialized;
    CCGLProgram* s_pShader;
};
struct ccDrawData1_t {
    int s_nColorLocation;
    ccColor4F s_tColor;
    int s_nPointSizeLocation;
    GLfloat s_fPointSize;
};
static ccDrawData0_t* data0;
static ccDrawData1_t* data1;
void(__cdecl* lazy_init)();

struct ccDrawVaoVbo_t {
    GLuint vao = 0;
    GLuint vbo = 0;

private:
    GLsizeiptr bufferSize;
    const void* bufferData;

public:
    ccDrawVaoVbo_t(GLsizeiptr size, const void* data) : bufferSize(size), bufferData(data) {}

    void setup() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ccVertex2F), (GLvoid*)0);
    }

    void updateData() { updateData(bufferSize, bufferData); }
    void updateData(GLsizeiptr size, const void* data) {
        if(size > bufferSize) {
            bufferSize = size;
            bufferData = data;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return;
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
        void* buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(buf, data, size);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};

static int ccDrawCircleLastSegments = 0;
static GLfloat* ccDrawCicleVertices = nullptr;
static ccDrawVaoVbo_t ccDrawCircleObj{0, ccDrawCicleVertices};

static ccVertex2F ccDrawLineVertices[2] = { {0.f, 0.f}, {0.f, 0.f} };
static ccDrawVaoVbo_t ccDrawLineObj{sizeof(ccVertex2F) * 2, ccDrawLineVertices};
static ccDrawVaoVbo_t ccDrawLinesObj{0, nullptr};

static ccDrawVaoVbo_t ccDrawPolyObj{0, nullptr};

void vaoDrawInit_() {
    ZoneScoped;

    if(!data0->s_bInitialized) {
        // TODO: use other opengl shenanigans for optimization

        ccDrawCircleObj.setup();
        ccDrawLineObj.setup();
        ccDrawLinesObj.setup();
        ccDrawPolyObj.setup();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
void vaoDrawInit() {
    ZoneScoped;
    vaoDrawInit_();
    ccDrawInit();
}
void vaoLazyInit() {
    ZoneScoped;
    vaoDrawInit_();
    lazy_init();
}
void vaoDrawFree() {
    ZoneScoped;

    if(data0->s_bInitialized) {
        ccDrawCircleObj.cleanup();
        ccDrawLineObj.cleanup();
        ccDrawLinesObj.cleanup();
        ccDrawPolyObj.cleanup();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ccDrawFree();
}

void vaoDrawCircle(const CCPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY) {
    ZoneScoped;
    lazy_init();

    if(radius == 0.f)
        return;

    // TODO: cache vaos of different segment counts and scale them with a uniform

    if(ccDrawCircleLastSegments == 0 || ccDrawCircleLastSegments != segments) {
        delete[] ccDrawCicleVertices;
        ccDrawCicleVertices = new GLfloat[2 * (segments + 2)];
        ccDrawCircleLastSegments = segments;
    }

    const float coef = 2.0f * (float)M_PI / segments;
    for(unsigned int i = 0; i <= segments; i++) {
        float rads = i * coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;

        ccDrawCicleVertices[i * 2] = j;
        ccDrawCicleVertices[i * 2 + 1] = k;
    }
    ccDrawCicleVertices[(segments + 1) * 2] = center.x;
    ccDrawCicleVertices[(segments + 1) * 2 + 1] = center.y;

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&data1->s_tColor.r, 1);

    TracyGpuZone("ccDrawCircle");
    ccDrawCircleObj.updateData(sizeof(ccDrawCicleVertices[0]) * 2 * (segments + 2), ccDrawCicleVertices);
    glBindVertexArray(ccDrawCircleObj.vao);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)segments + 1 + drawLineToCenter);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}
void vaoDrawFilledCircle(const CCPoint& center, float radius, float angle, unsigned int segments) {
    ZoneScoped;
    lazy_init();

    if(radius == 0.f)
        return;

    // TODO: cache vaos of different segment counts and scale them with a uniform

    if(ccDrawCircleLastSegments == 0 || ccDrawCircleLastSegments != segments) {
        delete[] ccDrawCicleVertices;
        ccDrawCicleVertices = new GLfloat[2 * (segments + 2)];
        ccDrawCircleLastSegments = segments;
    }

    const float coef = 2.0f * (float)M_PI / segments;
    for(unsigned int i = 0; i <= segments; i++) {
        float rads = i * coef;
        GLfloat j = radius * cosf(rads + angle) + center.x;
        GLfloat k = radius * sinf(rads + angle) + center.y;

        ccDrawCicleVertices[i * 2] = j;
        ccDrawCicleVertices[i * 2 + 1] = k;
    }

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&data1->s_tColor.r, 1);

    TracyGpuZone("ccDrawFilledCircle");
    ccDrawCircleObj.updateData(sizeof(ccDrawCicleVertices[0]) * 2 * (segments + 2), ccDrawCicleVertices);
    glBindVertexArray(ccDrawCircleObj.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)segments + 1);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}

void vaoDrawLine(const CCPoint& origin, const CCPoint& destination) {
    ZoneScoped;
    lazy_init();

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&data1->s_tColor.r, 1);

    ccDrawLineVertices[0] = {origin.x, origin.y};
    ccDrawLineVertices[1] = {destination.x, destination.y};

    TracyGpuZone("ccDrawLine");
    ccDrawLineObj.updateData();
    glBindVertexArray(ccDrawLineObj.vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}
void vaoDrawLines(const CCPoint* lines, unsigned int numberOfLines) {
    ZoneScoped;
    lazy_init();

    if(numberOfLines == 0)
        return;

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&data1->s_tColor.r, 1);
    data0->s_pShader->setUniformLocationWith1f(data1->s_nPointSizeLocation, data1->s_fPointSize);

    TracyGpuZone("ccDrawLines");
    ccDrawLinesObj.updateData(sizeof(ccVertex2F) * numberOfLines, lines);
    glBindVertexArray(ccDrawLinesObj.vao);
    glDrawArrays(GL_LINES, 0, numberOfLines);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}

void vaoDrawPoly(const CCPoint* vertices, unsigned int numOfVertices, bool closePolygon) {
    ZoneScoped;
    lazy_init();

    if(numOfVertices == 0)
        return;

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&data1->s_tColor.r, 1);

    TracyGpuZone("ccDrawPoly");
    ccDrawPolyObj.updateData(sizeof(ccVertex2F) * numOfVertices, vertices);
    glBindVertexArray(ccDrawPolyObj.vao);
    glDrawArrays(closePolygon ? GL_LINE_LOOP : GL_LINE_STRIP, 0, numOfVertices);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}
void vaoDrawSolidPoly(const CCPoint* poli, unsigned int numberOfPoints, ccColor4F color) {
    ZoneScoped;
    lazy_init();

    if(numberOfPoints == 0)
        return;

    data0->s_pShader->use();
    data0->s_pShader->setUniformsForBuiltins();
    data0->s_pShader->setUniformLocationWith4fv(data1->s_nColorLocation, (GLfloat*)&color.r, 1);

    TracyGpuZone("ccDrawSolidPoly");
    ccDrawPolyObj.updateData(sizeof(ccVertex2F) * numberOfPoints, poli);
    glBindVertexArray(ccDrawPolyObj.vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numberOfPoints);
    glBindVertexArray(0);

    CC_INCREMENT_GL_DRAWS(1);
}

$execute {
    data0 = reinterpret_cast<ccDrawData0_t*>(base::getCocos() + 0x19d8f8);
    data1 = reinterpret_cast<ccDrawData1_t*>(base::getCocos() + 0x198b34);
    lazy_init = reinterpret_cast<void(__cdecl*)()>(base::getCocos() + 0x6b980);

    bool success = true;
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawInit)),
        &vaoDrawInit,
        "cocos2d::ccDrawInit",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(lazy_init)),
        &vaoLazyInit,
        "lazy_init",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawFree)),
        &vaoDrawFree,
        "cocos2d::ccDrawFree",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(
                geode::modifier::Resolve<const CCPoint&, float, float, unsigned int, bool, float, float>::func(&ccDrawCircle)
            )
        ),
        &vaoDrawCircle,
        "cocos2d::ccDrawCircle",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawFilledCircle)),
        &vaoDrawFilledCircle,
        "cocos2d::ccDrawFilledCircle",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawLine)),
        &vaoDrawLine,
        "cocos2d::ccDrawLine",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawLines)),
        &vaoDrawLines,
        "cocos2d::ccDrawLines",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawPoly)),
        &vaoDrawPoly,
        "cocos2d::ccDrawPoly",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();
    success = success && Mod::get()->addHook(
        reinterpret_cast<void*>(geode::addresser::getNonVirtual(&ccDrawSolidPoly)),
        &vaoDrawSolidPoly,
        "cocos2d::ccDrawSolidPoly",
        tulip::hook::TulipConvention::Cdecl
    ).isOk();

    if(!success)
        log::error("failed to hook some ccDraw* stuff");
}

constexpr auto GEODE_NODE_METADATA_TAG = 0xB324ABC;

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSprite) {
    GLuint m_vao = 0;
    GLuint m_vbo = 0;

    void draw() {
        ZoneScoped;

        if(m_bDontDraw || _realOpacity == 0)
            return;

        TracyGpuZone("CCSprite::draw");

        if(m_fields->m_vao == 0) {
            glGenVertexArrays(1, &m_fields->m_vao);
            glBindVertexArray(m_fields->m_vao);

            glGenBuffers(1, &m_fields->m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_sQuad), &m_sQuad, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(kCCVertexAttrib_Position);
            glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, vertices));

            glEnableVertexAttribArray(kCCVertexAttrib_Color);
            glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, colors));

            glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
            glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, texCoords));

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // TODO: only upload when quad changes
        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_sQuad), &m_sQuad);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CC_NODE_DRAW_SETUP();

        ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);
        ccGLBindTexture2D(m_pobTexture->getName());

        glBindVertexArray(m_fields->m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        CC_INCREMENT_GL_DRAWS(1);
    }

    void destructor() {
        if(m_pUserObject && m_pUserObject->getTag() == GEODE_NODE_METADATA_TAG) {
            glDeleteVertexArrays(1, &m_fields->m_vao);
            glDeleteBuffers(1, &m_fields->m_vbo);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        CCSprite::~CCSprite();
    }
};

std::unordered_map<CCTextureAtlas*, GLuint> textureAtlasVaos;
#include <Geode/modify/CCTextureAtlas.hpp>
class $modify(CCTextureAtlas) {
    // setupVBO is inlined on windows
    void mapBuffers() {
        ZoneScoped;

        glDeleteBuffers(2, &m_pBuffersVBO[0]);
        glDeleteVertexArrays(1, &textureAtlasVaos[this]);
        glBindVertexArray(0);

        glGenVertexArrays(1, &textureAtlasVaos[this]);
        glBindVertexArray(textureAtlasVaos[this]);

        glGenBuffers(2, &m_pBuffersVBO[0]);

        glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uCapacity, m_pQuads, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, vertices));

        glEnableVertexAttribArray(kCCVertexAttrib_Color);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, colors));

        glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, texCoords));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uCapacity * 6, m_pIndices, GL_STATIC_DRAW);

        // Must unbind the VAO before changing the element buffer.
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void drawNumberOfQuads(unsigned int n, unsigned int start) {
        ZoneScoped;

        if(n == 0)
            return;

        TracyGpuZone("CCTextureAtlas::drawNumberOfQuads");

        ccGLBindTexture2D(m_pTexture->getName());

        if(m_bDirty) {
            glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
            // option 1: subdata
            //glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * start, sizeof(m_pQuads[0]) * n, &m_pQuads[start]);

            // option 2: data
            //glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * (n - start), &quads_[start], GL_DYNAMIC_DRAW);

            // option 3: orphaning + glMapBuffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * (n - start), nullptr, GL_DYNAMIC_DRAW);
            void* buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            memcpy(buf, m_pQuads, sizeof(m_pQuads[0]) * (n - start));
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            m_bDirty = false;
        }

        glBindVertexArray(textureAtlasVaos[this]);
        glDrawElements(GL_TRIANGLES, (GLsizei)n * 6, GL_UNSIGNED_SHORT, (GLvoid*)(start * 6 * sizeof(m_pIndices[0])));
        glBindVertexArray(0);

        CC_INCREMENT_GL_DRAWS(1);
    }

    void destructor() {
        glDeleteVertexArrays(1, &textureAtlasVaos[this]);
        glBindVertexArray(0);
        textureAtlasVaos.erase(this);
        CCTextureAtlas::~CCTextureAtlas();
    }
};

#include <Geode/modify/CCParticleSystemQuad.hpp>
class $modify(CCParticleSystemQuad) {
    // TODO: geode bug? have to put these here from smooth particles for fields to not clash
    bool m_firstTime;
    CCPoint m_prevPosition;
    size_t m_newCount;
    size_t m_newIndex;
    float m_xDiff;
    float m_yDiff;

    GLuint m_vao = 0;

    // hook constructor to force m_fields creation to prevent issues in the destructor
    CCParticleSystemQuad* constructor() { this->CCParticleSystemQuad::CCParticleSystemQuad(); return this; }

    void setupVBO() {
        ZoneScoped;

        glDeleteBuffers(2, &m_pBuffersVBO[0]);
        glDeleteVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(0);

        glGenVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(m_fields->m_vao);

        glGenBuffers(2, &m_pBuffersVBO[0]);

        glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ccV3F_C4B_T2F_Quad) * m_uTotalParticles, m_pQuads, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, vertices));

        glEnableVertexAttribArray(kCCVertexAttrib_Color);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, colors));

        glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (GLvoid*)offsetof(ccV3F_C4B_T2F, texCoords));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_uTotalParticles * 6, m_pIndices, GL_STATIC_DRAW);

        // Must unbind the VAO before changing the element buffer.
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw() {
        ZoneScoped;

        if(!m_pTexture || m_uParticleIdx <= 0)
            return;

        TracyGpuZone("CCParticleSystemQuad::draw");

        CC_NODE_DRAW_SETUP();

        ccGLBindTexture2D(m_pTexture->getName());
        ccGLBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);

        glBindVertexArray(m_fields->m_vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_uParticleIdx * 6, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        CC_INCREMENT_GL_DRAWS(1);
    }

    void destructor() {
        if(m_pUserObject && m_pUserObject->getTag() == GEODE_NODE_METADATA_TAG) {
            glDeleteVertexArrays(1, &m_fields->m_vao);
            glBindVertexArray(0);
        }
        CCParticleSystemQuad::~CCParticleSystemQuad();
    }
};

#include <Geode/modify/CCMotionStreak.hpp>
class $modify(CCMotionStreak) {
    GLuint m_vao = 0;
    GLuint m_vbo = 0;

    GLsizeiptr vertSize() { return sizeof(ccVertex2F) * m_uMaxPoints * 2; }
    GLsizeiptr colSize() { return sizeof(GLubyte) * m_uMaxPoints * 2 * 4; }
    GLsizeiptr uvSize() { return sizeof(ccTex2F) * m_uMaxPoints * 2; }

    bool initWithFade(float fade, float minSeg, float stroke, ccColor3B const& color, CCTexture2D* texture) {
        ZoneScoped;

        if(!CCMotionStreak::initWithFade(fade, minSeg, stroke, color, texture))
            return false;

        glDeleteBuffers(1, &m_fields->m_vbo);
        glDeleteVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(0);

        glGenVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(m_fields->m_vao);

        glGenBuffers(1, &m_fields->m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertSize() + colSize() + uvSize(), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ccVertex2F), 0);

        glEnableVertexAttribArray(kCCVertexAttrib_Color);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLubyte) * 4, (GLvoid*)vertSize());

        glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccTex2F), (GLvoid*)(vertSize() + colSize()));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    void draw() {
        ZoneScoped;

        if(m_uNuPoints <= 1)
            return;

        TracyGpuZone("CCMotionStreak::draw");

        // TODO: only upload when changed
        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize(), m_pVertices);
        glBufferSubData(GL_ARRAY_BUFFER, vertSize(), colSize(), m_pColorPointer);
        glBufferSubData(GL_ARRAY_BUFFER, vertSize() + colSize(), uvSize(), m_pTexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CC_NODE_DRAW_SETUP();

        ccGLBindTexture2D(m_pTexture->getName());
        ccGLBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);

        glBindVertexArray(m_fields->m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)m_uNuPoints * 2);
        glBindVertexArray(0);

        CC_INCREMENT_GL_DRAWS(1);
    }

    void destructor() {
        if(m_pUserObject && m_pUserObject->getTag() == GEODE_NODE_METADATA_TAG) {
            glDeleteVertexArrays(1, &m_fields->m_vao);
            glDeleteBuffers(1, &m_fields->m_vbo);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        CCMotionStreak::~CCMotionStreak();
    }
};

// CCDrawNode actually already has the vao field and all the code there is basically already
// tailoed to use vaos but vaos were disabled during compilation, so we basically just
// add the code that was compiled out back
#include <Geode/modify/CCDrawNode.hpp>
class $modify(CCDrawNode) {
    bool init() {
        ZoneScoped;
        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
        bool ret = CCDrawNode::init();
        glBindVertexArray(0);
        return ret;
    }

    void draw() {
        ZoneScoped;
        // for some reason vanilla doesnt have a check for this
        if(m_nBufferCount <= 0)
            return;
        TracyGpuZone("CCDrawNode::draw");
        glBindVertexArray(m_uVao);
        CCDrawNode::draw();
        glBindVertexArray(0);
    }

    void destructor() {
        glDeleteVertexArrays(1, &m_uVao);
        glBindVertexArray(0);
        CCDrawNode::~CCDrawNode();
    }
};

#include <Geode/modify/CCLayerColor.hpp>
class $modify(CCLayerColor) {
    GLuint m_vao = 0;
    GLuint m_vbo = 0;

    // hook constructor to force m_fields creation to prevent issues in the destructor
    CCLayerColor* constructor() { this->CCLayerColor::CCLayerColor(); return this; }

    GLsizeiptr vertSize() { return sizeof(ccVertex2F) * 4; }
    GLsizeiptr colSize() { return sizeof(ccColor4F) * 4; }

    bool initWithColor(const ccColor4B& color, GLfloat width, GLfloat height) {
        ZoneScoped;

        glDeleteBuffers(1, &m_fields->m_vbo);
        glDeleteVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(0);

        glGenVertexArrays(1, &m_fields->m_vao);
        glBindVertexArray(m_fields->m_vao);

        glGenBuffers(1, &m_fields->m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertSize() + colSize(), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ccVertex2F), 0);

        glEnableVertexAttribArray(kCCVertexAttrib_Color);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, sizeof(ccColor4F), (GLvoid*)vertSize());

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return CCLayerColor::initWithColor(color, width, height);
    }

    void setContentSize(const CCSize& size) {
        ZoneScoped;
        CCLayerColor::setContentSize(size);
        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize(), m_pSquareVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //void updateColor() {
    //    ZoneScoped;
    //    CCLayerColor::updateColor();
    //    glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
    //    glBufferSubData(GL_ARRAY_BUFFER, vertSize(), colSize(), m_pSquareColors);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //}

    void draw() {
        ZoneScoped;
        TracyGpuZone("CCLayerColor::draw");

        glBindBuffer(GL_ARRAY_BUFFER, m_fields->m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, vertSize(), colSize(), m_pSquareColors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CC_NODE_DRAW_SETUP();

        ccGLBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);

        glBindVertexArray(m_fields->m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        CC_INCREMENT_GL_DRAWS(1);
    }

    void destructor() {
        if(m_pUserObject && m_pUserObject->getTag() == GEODE_NODE_METADATA_TAG) {
            glDeleteVertexArrays(1, &m_fields->m_vao);
            glDeleteBuffers(1, &m_fields->m_vbo);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        CCLayerColor::~CCLayerColor();
    }
};

#endif
