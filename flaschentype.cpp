#include <cstdlib>
#include <array>
#include "flaschentype.hpp"
#include <iostream>
#include <memory>
#include <cstring>
#include "ebg.hpp"

void draw_glyph_at(text_image& img, int x, int baseline_descent, const Character& c){
    //std::cout << c.Bearing[1] << " vs " << c.Size[1] << "\n";
    for(int i = 0;i < c.Size[1];i++){
        for(int j = 0;j < c.Size[0];j++){
            img(i - c.Bearing[1] + baseline_descent, j + c.Bearing[0] + x) += c.buffer[i * c.Size[0] + j];
        }
    }
}
int GetCodepointNext(const char *text, int *codepointSize){
    const char *ptr = text;
    int codepoint = 0x3f;       // Codepoint (defaults to '?')
    *codepointSize = 1;

    // Get current codepoint and bytes processed
    if (0xf0 == (0xf8 & ptr[0]))
    {
        // 4 byte UTF-8 codepoint
        if(((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80) || ((ptr[3] & 0xC0) ^ 0x80)) { return codepoint; } //10xxxxxx checks
        codepoint = ((0x07 & ptr[0]) << 18) | ((0x3f & ptr[1]) << 12) | ((0x3f & ptr[2]) << 6) | (0x3f & ptr[3]);
        *codepointSize = 4;
    }
    else if (0xe0 == (0xf0 & ptr[0]))
    {
        // 3 byte UTF-8 codepoint */
        if(((ptr[1] & 0xC0) ^ 0x80) || ((ptr[2] & 0xC0) ^ 0x80)) { return codepoint; } //10xxxxxx checks
        codepoint = ((0x0f & ptr[0]) << 12) | ((0x3f & ptr[1]) << 6) | (0x3f & ptr[2]);
        *codepointSize = 3;
    }
    else if (0xc0 == (0xe0 & ptr[0]))
    {
        // 2 byte UTF-8 codepoint
        if((ptr[1] & 0xC0) ^ 0x80) { return codepoint; } //10xxxxxx checks
        codepoint = ((0x1f & ptr[0]) << 6) | (0x3f & ptr[1]);
        *codepointSize = 2;
    }
    else if (0x00 == (0x80 & ptr[0]))
    {
        // 1 byte UTF-8 codepoint
        codepoint = ptr[0];
        *codepointSize = 1;
    }

    return codepoint;
}
FT_Library* ft = nullptr;
Font::Font(int res){
    if(ft == nullptr){
        ft = new FT_Library;
            if (FT_Init_FreeType(ft)){
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            exit(1);
        }
    }
    
    scale = res;
    

    // load font as face
    if (FT_New_Memory_Face(*ft, EBGaramond_Regular_Kern_ttf, EBGaramond_Regular_Kern_ttf_len, 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load embedded font\n";
        exit(1);
    }
    FT_Set_Pixel_Sizes(face, 0, res);
    for(int i = 0;i < 256;i++){
        //std::cout << i <<  "\n";
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << i << std::endl;
            //exit(1);
        }
        else{
            //std::cerr << size_t(face->glyph->bitmap.buffer) << "\n";
            Character character = {
                std::array<int, 2>{(int)face->glyph->bitmap.width,(int)face->glyph->bitmap.rows},
                std::array<int, 2>{(int)face->glyph->bitmap_left, (int)face->glyph->bitmap_top },
                static_cast<unsigned int>(face->glyph->advance.x),
                [this]{
                    std::unique_ptr<char[]> ret = std::make_unique<char[]>(face->glyph->bitmap.width * face->glyph->bitmap.rows);
                    if(face->glyph->bitmap.buffer != nullptr)
                        std::memcpy(ret.get(), face->glyph->bitmap.buffer, face->glyph->bitmap.width * face->glyph->bitmap.rows);
                    else
                        std::fill(ret.get(), ret.get() + face->glyph->bitmap.width * face->glyph->bitmap.rows, 0);
                    return ret;
                }()
            };
            character_map[i] = std::move(character);
        }
    }

}
Font::Font(std::string font_name, int res){
    if(ft == nullptr){
        ft = new FT_Library;
            if (FT_Init_FreeType(ft)){
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            exit(1);
        }
    }
    
    scale = res;
    if (font_name.empty()){
        std::cout << "ERROR::FREETYPE: Failed to load font: " << font_name << std::endl;
        exit(1);
    }

    // load font as face
    if (FT_New_Face(*ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << font_name<< std::endl;
        exit(1);
    }
    FT_Set_Pixel_Sizes(face, 0, res);
    for(int i = 0;i < 256;i++){
        //std::cout << i <<  "\n";
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << i << std::endl;
            //exit(1);
        }
        else{
            //std::cerr << size_t(face->glyph->bitmap.buffer) << "\n";
            Character character = {
                std::array<int, 2>{(int)face->glyph->bitmap.width,(int)face->glyph->bitmap.rows},
                std::array<int, 2>{(int)face->glyph->bitmap_left, (int)face->glyph->bitmap_top },
                static_cast<unsigned int>(face->glyph->advance.x),
                [this]{
                    std::unique_ptr<char[]> ret = std::make_unique<char[]>(face->glyph->bitmap.width * face->glyph->bitmap.rows);
                    if(face->glyph->bitmap.buffer != nullptr)
                        std::memcpy(ret.get(), face->glyph->bitmap.buffer, face->glyph->bitmap.width * face->glyph->bitmap.rows);
                    else
                        std::fill(ret.get(), ret.get() + face->glyph->bitmap.width * face->glyph->bitmap.rows, 0);
                    return ret;
                }()
            };
            character_map[i] = std::move(character);
        }
    }
}
text_image draw_text(const std::string& _text, const Font& font) {
    float xOffset = 0;
    //float scale = font.scale;
    
    int max_keller = 0;
    int width = 0;
    int baseline_descent = 0;
    const char* text = _text.c_str();
    for (;;) {
        
        int bytes = 0;
        if(*text == 0)break;
        int c = GetCodepointNext(text, &bytes);
        text += bytes;
        int kernoffset = 0;
        if(*text){
            int cn = GetCodepointNext(text, &bytes);
            FT_Vector kerning;
            FT_UInt glyph1 = FT_Get_Char_Index(font.face, c);
            FT_UInt glyph2 = FT_Get_Char_Index(font.face, cn);
            FT_Get_Kerning(font.face, glyph1, glyph2, FT_KERNING_DEFAULT, &kerning);
            kernoffset = kerning.x;
        }
        max_keller = std::max(font.character_map.at(c).Size[1] - font.character_map.at(c).Bearing[1], max_keller);
        baseline_descent = std::max(font.character_map.at(c).Bearing[1], baseline_descent);
        auto it = font.character_map.find(c);
        if (it != font.character_map.end()) {
            if(*text){
                width += (it->second.Advance + kernoffset) * (1.0f / 64.0f);
            }
            else{
                width += it->second.Bearing[0] + it->second.Size[0];
            }
        }
    }
    //width += 10;
    int height = max_keller + baseline_descent;
    text = _text.c_str();
    //std::cout << width << "\n";
    text_image ret(width, height);
    for (;;) {
        int bytes = 0;
        if(*text == 0)break;
        int c = GetCodepointNext(text, &bytes);
        text += bytes;
        auto it = font.character_map.find(c);

        if (it != font.character_map.end()) {
            int kernoffset = 0;
            if(*text){
                int cn = GetCodepointNext(text, &bytes);
                FT_Vector kerning;
                FT_UInt glyph1 = FT_Get_Char_Index(font.face, c);
                FT_UInt glyph2 = FT_Get_Char_Index(font.face, cn);
                FT_Get_Kerning(font.face, glyph1, glyph2, FT_KERNING_DEFAULT, &kerning);
                kernoffset = kerning.x;
            }
            //std::cout << kerning.x << "\n";
            const Character& character = it->second;
            //draw_rectangle(character.tex, xPos, yPos, scale, col);
            draw_glyph_at(ret, xOffset, baseline_descent, it->second);

            xOffset += (character.Advance + kernoffset) * (1.0f / 64.0f);
            
        }
    }
    return ret;
}