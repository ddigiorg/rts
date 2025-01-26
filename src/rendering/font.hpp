// https://learnopengl.com/In-Practice/Text-Rendering
// signed distance fields (SDFs):
// - https://www.youtube.com/watch?v=1b5hIMqz_wM
// - https://www.youtube.com/watch?v=d8cfgcJR9Tk
// TODO: look into stb_truetype.h:
// - https://github.com/nothings/stb/blob/master/stb_truetype.h
// - https://github.com/0xc0dec/demos/blob/master/demos/stb-truetype/gl/Main.cpp
#ifndef FONT_HPP
#define FONT_HPP

#include "rendering/shader.hpp"

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <iostream>
#include <map>

#define FONT_POINTS_PER_INCH 72
#define FONT_PIXELS_PER_INCH 96
#define FONT_PIXEL_PER_POINT ((float)FONT_PIXELS_PER_INCH / (float)FONT_POINTS_PER_INCH)
#define FONT_GLYPH_PIXEL_HEIGHT 64

#define FONT_VERT_FILEPATH "data/shaders/font_vert.glsl"
#define FONT_FRAG_FILEPATH "data/shaders/font_frag.glsl"
#define FONT_MONOTYPE_FILEPATH "data/fonts/CommitMono-400-Regular.otf"

struct Character {
    GLuint texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

class Font {
public:
    Font();
    ~Font();
    void render(
        std::string& text,
        float x,
        float y,
        unsigned int pt,
        glm::vec3& color,
        glm::mat4x4& projMat
    );

private:
    GLuint vao;
    GLuint vbo;
    Shader shader;
    std::map<char, Character> characters;
};

Font::Font() : shader(FONT_VERT_FILEPATH, FONT_FRAG_FILEPATH) {
    FT_Library ft;
    FT_Error error;

    // setup FreeType
    error = FT_Init_FreeType(&ft);
    if (error) {
        std::cout << "Error: FT_Init_FreeType() failed." << std::endl;
        exit(1);
    }

    // load font face
    FT_Face face;
    if (FT_New_Face(ft, FONT_MONOTYPE_FILEPATH, 0, &face))
    {
        std::cout << "Error: FT_New_Face() failed to load font: " << FONT_MONOTYPE_FILEPATH << std::endl;  
        exit(1);
    }

    // set the pixel font size
    FT_Set_Pixel_Sizes(face, 0, FONT_GLYPH_PIXEL_HEIGHT);  

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_GlyphSlot slot = face->glyph; // for SDFs

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) {

        // load character glyph
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (error) {
            std::cout << "Warning:: FT_Load_Char() failed to load character glyph: " << c << std::endl;  
            continue;
        }

        // setup signed distance field (SDF) mode
        FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x),
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // clear FreeType's resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // setup opengl buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Font::~Font() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Font::render(
        std::string& text,
        float x,
        float y,
        unsigned int pt,
        glm::vec3& color,
        // glm::mat4x4& view,
        glm::mat4x4& projMat
    ) {

    // calculate scale based on font point size
    // TODO: this scale might not be entirely accurate due to:
    // - glyph pixel height might have some unknown padding?
    // - SDF calculations in the shader?
    // Anyway, it works for now...
    float scale =  ((float)pt * FONT_PIXEL_PER_POINT) / (float)FONT_GLYPH_PIXEL_HEIGHT;

    // TODO: dynamically choose appropriate edge and widths based on pt size (or scale)
    // - smaller widths for smaller texts
    // - larger edges for smaller texts
    // - see: https://www.youtube.com/watch?v=d8cfgcJR9Tk at ~8:15

    // activate corresponding render state
    shader.use();
    shader.setUniformMatrix4fv("projection", 1, projMat);
    shader.setUniform3f("u_charColor", color.x, color.y, color.z);
    shader.setUniform1f("u_charWidth", 0.4f);
    shader.setUniform1f("u_charEdge", 0.2f);
    shader.setUniform3f("u_borderColor", 0.0f, 0.0f, 0.0f);
    shader.setUniform2f("u_borderOffset", 0.0f, 0.0f);
    shader.setUniform1f("u_borderWidth", 0.6f);
    shader.setUniform1f("u_borderEdge", 0.2f);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];

        float vx = x + ch.bearing.x * scale;
        float vy = y - (ch.size.y - ch.bearing.y) * scale;
        float vw = ch.size.x * scale;
        float vh = ch.size.y * scale;

        // update vertices
        float vertices[6][4] = {
            // triangle 0
            { vx,      vy + vh, 0.0f, 0.0f },
            { vx,      vy,      0.0f, 1.0f },
            { vx + vw, vy,      1.0f, 1.0f },
            // triangle 1
            { vx,      vy + vh, 0.0f, 0.0f },
            { vx + vw, vy,      1.0f, 1.0f },
            { vx + vw, vy + vh, 1.0f, 0.0f },
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // FONT_HPP