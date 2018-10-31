#ifndef H_FONT
#define H_FONT

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include "texture.h"


struct font_glyph_t
{
    FT_Glyph glyph;
    FT_BitmapGlyph bitmapGlyph;
    FT_Bitmap bitmap;
};

struct font_t
{
    FT_Library library;
    FT_Face face;
    font_glyph_t glyphs[128];
    texture_t texture;
    texture_data_t textureData;
};

void font_create(font_t *font);
void font_load_glyph(font_t *font, font_glyph_t *glyph, char ch);
void font_buffer_texture(font_t *font);

#endif
