#ifndef FLASCHENTYPE_HPP
#define FLASCHENTYPE_HPP
#include <ft2build.h>
#include FT_FREETYPE_H
#include <array>
#include <memory>
#include <cassert>
#include <unordered_map>
struct Character {
    std::array<int, 2>   Size;      // Size of glyph
    std::array<int, 2>   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
    std::unique_ptr<char[]> buffer;
};
struct text_image{
    int w;
    int h;
    std::unique_ptr<char[]> buffer;
    text_image(int _w, int _h) : w(_w), h(_h), buffer(std::make_unique<char[]>(w * h)){
        std::fill(buffer.get(), buffer.get() + w * h, 0);
    }
    char& operator()(int i, int j){
        assert(i < h);
        assert(j < w);
        return buffer[i * w + j];
    }
    const char& operator()(int i, int j) const {
        assert(i < h);
        assert(j < w);
        return buffer[i * w + j];
    }
};
void draw_glyph_at(text_image& img, int x, int baseline_descent, const Character& c);
int GetCodepointNext(const char *text, int *codepointSize);
struct Font{
    int scale;
    std::unordered_map<int, Character> character_map;
    FT_Face face;
    Font(std::string font_name, int res);
    Font(int res);
};
text_image draw_text(const std::string& _text, const Font& font);
extern FT_Library* ft;
#endif
