#include "font.h"

#include <stdexcept>
#include <iostream>

void font_create(font_t *font)
{
    if (FT_Init_FreeType(&font->library)) 
        throw std::runtime_error("FT_Init_FreeType failed");

    if (FT_New_Face(font->library, "liberation.ttf", 0, &font->face)) 
        throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

    FT_Set_Pixel_Sizes(font->face, (int)font->size, (int)font->size);

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
    glyph->pxWidth = glyph->bitmap.width + 2;
    glyph->pxHeight = glyph->bitmap.rows;
    glyph->left = ((float)glyph->bitmapGlyph->left);
    glyph->top = ((float)glyph->bitmapGlyph->top);
    glyph->xAdv = ((float)glyph->glyph->advance.x) / 65536.0f;
    glyph->yAdv = ((float)glyph->glyph->advance.y) / 65536.0f;
    glyph->buffer = (unsigned short*)malloc(sizeof(unsigned short) * glyph->pxWidth * glyph->pxHeight);
    for (int y = 0; y < glyph->pxHeight; ++y)
    {
        for (int x = 0; x < glyph->pxWidth; ++x)
        {
            if (x == 0 || x == glyph->pxWidth - 1)
                glyph->buffer[x + y * glyph->pxWidth] = 0;
            else
                glyph->buffer[x + y * glyph->pxWidth] = glyph->bitmap.buffer[(x - 1) + y * glyph->bitmap.width];
        }
    }
}

void font_buffer_texture(font_t *font)
{
    int width = 0;
    int height = 0;

    for (int i = 0; i < 128; ++i)
    {
        width += font->glyphs[i].pxWidth;
        short tmpHeight = font->glyphs[i].pxHeight;
        if (tmpHeight > height)
            height = tmpHeight;
    }

    font->height = height;

    unsigned short *buffer = (unsigned short*)malloc(sizeof(unsigned short) * width * height);
    for (int i = 0; i < width * height; ++i)
        buffer[i] = 0;

    int offset = 0;
    for (int i = 0; i < 128; ++i)
    {
        font->glyphs[i].width = font->glyphs[i].pxWidth - 2.0f;
        font->glyphs[i].height = font->glyphs[i].pxHeight;
        font->glyphs[i].left = font->glyphs[i].left + 1.0f;
        font->glyphs[i].top = font->glyphs[i].top;
        font->glyphs[i].xAdv = font->glyphs[i].xAdv + 1.0f;
        font->glyphs[i].yAdv = font->glyphs[i].yAdv;
        font->glyphs[i].uvWidth = ((float)font->glyphs[i].pxWidth - 2.0f)/((float)width);
        font->glyphs[i].uvHeight = ((float)font->glyphs[i].pxHeight)/((float)height);
        font->glyphs[i].uvOffset = ((float)offset + 1.0f)/((float)width);
        for (int y = 0; y < font->glyphs[i].pxHeight; ++y)
        {
            for (int x = 0; x < font->glyphs[i].pxWidth; ++x)
                buffer[y * width + x + offset] = font->glyphs[i].buffer[y * font->glyphs[i].pxWidth + x];
        }
        offset += font->glyphs[i].pxWidth;
    }

    font->textureData.width = width;
    font->textureData.height = height;
    font->textureData.size = width * height * 4;
    for (int i = 0; i < width * height; ++i)
    {
        for (int j = 0; j < 4; ++j)
            font->textureData.data.push_back(buffer[i]);
    }

    free(buffer);
};
