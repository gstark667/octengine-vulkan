#include "font.h"

#include <stdexcept>
#include <iostream>

void font_create(font_t *font)
{
    if (FT_Init_FreeType(&font->library)) 
        throw std::runtime_error("FT_Init_FreeType failed");

    if (FT_New_Face(font->library, "test.ttf", 0, &font->face)) 
        throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

    int h = 16;
    FT_Set_Char_Size(font->face, h << 6, h << 6, 96, 96);

    std::cout << "generating font" << std::endl;
    for (int i = 0; i < 128; ++i)
        font_load_glyph(font, &font->glyphs[i], (char)i);

    FT_Done_Face(font->face);
    FT_Done_FreeType(font->library);

    font_buffer_texture(font);
}

void font_load_glyph(font_t *font, font_glyph_t *glyph, char ch)
{
    if (FT_Load_Glyph(font->face, FT_Get_Char_Index(font->face, ch), FT_LOAD_DEFAULT))
        throw std::runtime_error("FT_Load_Glyph failed");

    if (FT_Get_Glyph(font->face->glyph, &glyph->glyph))
        throw std::runtime_error("FT_Get_Glyph failed");

    FT_Glyph_To_Bitmap(&glyph->glyph, ft_render_mode_normal, 0, 1);
    glyph->bitmapGlyph = (FT_BitmapGlyph)glyph->glyph;
    glyph->bitmap = glyph->bitmapGlyph->bitmap;
}

void font_buffer_texture(font_t *font)
{
    int width = 0;
    int height = 0;

    for (int i = 0; i < 128; ++i)
    {
        width += font->glyphs[i].bitmap.width;
        short tmpHeight = font->glyphs[i].bitmap.rows;
        if (tmpHeight > height)
            height = tmpHeight;
    }

    unsigned short *buffer = (unsigned short*)malloc(sizeof(unsigned short) * width * height);
    for (int i = 0; i < width * height; ++i)
        buffer[i] = 0;

    int offset = 0;
    for (int i = 0; i < 128; ++i)
    {
        for (int y = 0; y < font->glyphs[i].bitmap.rows; ++y)
        {
            for (int x = 0; x < font->glyphs[i].bitmap.width; ++x)
                buffer[y * width + x + offset] = font->glyphs[i].bitmap.buffer[y * font->glyphs[i].bitmap.width + x];
        }
        offset += font->glyphs[i].bitmap.width;
    }

    font->textureData.width = width;
    font->textureData.height = height;
    font->textureData.size = width * height * 4;
    for (int i = 0; i < width * height; ++i)
    {
        for (int j = 0; j < 4; ++j)
            font->textureData.data.push_back(buffer[i]);
    }

    std::cout << "buffering: " << width << ":" << height << std::endl;
    free(buffer);
};
