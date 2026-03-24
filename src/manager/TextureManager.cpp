#include "TextureManager.h"
#include "../Game.h"

#include <iostream>
#include <ostream>
#include <SDL3/SDL_rect.h>

// Doesn't create any extra memory, just grabs the reference of this object.
extern Game* game;
std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;

// Load our texture with a path.
SDL_Texture* TextureManager::load(const char* path) {
    // Caching: storing the result of some work so you don't have to repeat the work next time.
    // Check if the texture already exists in the map.
    auto it = textures.find(path);
    if (it != textures.end()) { // If the iterator is not at the end, it is already cached.
        return it->second;
    }

    // What is a surface?
    // A surface represents an image in RAM (CPU memory).
    // Do we want to do anything with the image before we store it in GPU?.
    SDL_Surface* tempSurface = IMG_Load(path);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << path << std::endl;
    }

    // Texture represents an image in VRAM (GPU memory).
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);

    // Cleanup the surface.
    SDL_DestroySurface(tempSurface);

    // Check if the texture creation was successful.
    if (!texture) {
        std::cout << "Failed to create texture from: " << path << std::endl;
        return nullptr;
    }

    // Store the new texture in the cache.
    textures[path] = texture;

    return texture;
}

void TextureManager::loadLabel(Label& label) {
    auto it = textures.find(label.textureCacheKey);

    if (it != textures.end()) {
        label.texture = it->second;
        return;
    }

    updateLabel(label);
}


void TextureManager::updateLabel(Label& label) {
    if (!label.dirty) {
        return;
    }

    if (label.texture) {
        SDL_DestroyTexture(label.texture);
        label.texture = nullptr;
    }

    // Create the surface for the fill of the text.
    SDL_Surface* tempFillSurface = TTF_RenderText_Blended(
        label.font,
        label.text.c_str(),
        label.text.size(),
        label.color
    );

    // Create the surface for the black outline of the text.
    int outlineSize = 1;
    TTF_Font* fontOutline = TTF_CopyFont(label.font);
    TTF_SetFontOutline(fontOutline, outlineSize);

    SDL_Surface* tempOutlineSurface = TTF_RenderText_Blended(
        fontOutline,
        label.text.c_str(),
        label.text.size(),
        {0, 0, 0, 255}
    );

    if (!tempFillSurface || !tempOutlineSurface) {
        std::cout << "Failed to load surface: " << label.textureCacheKey << std::endl;
    }

    // Blit fill surface onto the outline surface to create the outlined text surface.
    SDL_Rect dst = {outlineSize, outlineSize, tempFillSurface->w, tempFillSurface->h};
    SDL_SetSurfaceBlendMode(tempFillSurface, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(tempFillSurface, nullptr, tempOutlineSurface, &dst);

    // Create the texture.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempOutlineSurface);

    // Clean up temp surfaces and outline font.
    SDL_DestroySurface(tempFillSurface);
    SDL_DestroySurface(tempOutlineSurface);
    TTF_CloseFont(fontOutline);

    if (!texture) {
        std::cout << "Failed to create texture: " << label.textureCacheKey << std::endl;
    }

    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    label.dst.w = w;
    label.dst.h = h;

    // Cache the texture.
    label.texture = texture;
    textures[label.textureCacheKey] = texture;

    // Clean the texture.
    label.dirty = false;
}

void TextureManager::draw(SDL_Texture* texture, const SDL_FRect* src, const SDL_FRect* dst) {
    // Draw the texture to the screen from VRAM.
    SDL_RenderTexture(game->renderer, texture, src, dst);
}

void TextureManager::clean() {
    for (auto& tex : textures) {
        SDL_DestroyTexture(tex.second);
        tex.second = nullptr;
    }

    textures.clear();
}
