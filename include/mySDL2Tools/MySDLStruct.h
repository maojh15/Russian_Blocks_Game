#ifndef MY_SDL_STRUCT
#define MY_SDL_STRUCT
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<string>

class TextureWrap{
public:
    TextureWrap(){
        texture = nullptr;
    }
    ~TextureWrap(){
        free();
    }
    void createFromIMG(SDL_Renderer* renderer, const char *imgFilename){
        if(texture != nullptr)
            SDL_DestroyTexture(texture);
        try{
            texture = IMG_LoadTexture(renderer, imgFilename);
            if(texture == nullptr){
                throw(std::runtime_error("cannot open file"));
            }
        }
        catch(std::runtime_error &e){
            throw(e);
        }
        SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
    }

    void free(){
        if(texture != nullptr){
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    SDL_Texture *texture;
    SDL_Rect rect;
};

//text 
class TextWrap{
public:
    TextWrap()noexcept{
        texture = nullptr;
    }

    TextWrap(TTF_Font *font_, const std::string& text_, SDL_Color color, SDL_Renderer *renderer){
        texture = nullptr;
        createUTF8Blended(font_, text_, color, renderer);
    }

    TextWrap(const TextWrap &rhs)noexcept{
        text = rhs.text;
        textColor = rhs.textColor;
        rect = rhs.rect;
        font = rhs.font;
        rendererOfText = rhs.rendererOfText;
        if(rhs.texture != nullptr){
            createUTF8Blended(font, text, textColor, rendererOfText);
        }
        else
            texture = nullptr;
    }

    TextWrap(TextWrap && rhs)noexcept{
        texture = rhs.texture;
        rhs.texture = nullptr;
        text = std::move(rhs.text);
        textColor = rhs.textColor;
        rect = rhs.rect;
        font = rhs.font;
        rendererOfText = rhs.rendererOfText;
    }

    TextWrap& operator = (TextWrap &&rhs){
        if(this == &rhs) return *this;
        texture = rhs.texture;
        rhs.texture = nullptr;
        text = std::move(rhs.text);
        textColor = rhs.textColor;
        rect = rhs.rect;
        font = rhs.font;
        rendererOfText = rhs.rendererOfText;
        return *this;
    }

    TextWrap& operator = (const TextWrap & rhs){
        if(this == &rhs) return *this;
        TextWrap temp{rhs};
        std::swap(*this, temp);
        return *this;
    }

    ~TextWrap(){
        free();
    }

    void createUTF8Blended(TTF_Font *font_, const std::string& text_, SDL_Color color, SDL_Renderer *renderer){
        free();
        text = text_;
        font = font_;
        textColor = color;
        rendererOfText = renderer;
        SDL_Surface *loadSurface = nullptr;
        if(text_.length() == 0){
            loadSurface = TTF_RenderUTF8_Blended(font, " ", color);
        }
        else
            loadSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        rect.w = loadSurface->w;
        rect.h = loadSurface->h;
        texture = SDL_CreateTextureFromSurface(renderer, loadSurface);
        SDL_FreeSurface(loadSurface);
    }

    void createUTF8Shaded(TTF_Font *font_, const std::string& text_, SDL_Color color, SDL_Color bgColor, SDL_Renderer *renderer){
        free();
        text = text_;
        font = font_;
        textColor = color;
        rendererOfText = renderer;
        SDL_Surface *loadSurface = nullptr;
        if(text_.length() == 0){
            loadSurface = TTF_RenderUTF8_Shaded(font, " ", color, bgColor);
        }
        else
            loadSurface = TTF_RenderUTF8_Shaded(font, text.c_str(), color, bgColor);
        rect.w = loadSurface->w;
        rect.h = loadSurface->h;
        texture = SDL_CreateTextureFromSurface(renderer, loadSurface);
        SDL_FreeSurface(loadSurface);
    }

    //when posX == -1 and posY == -1, use the value stored in rect.x and rect.y 
    void render(int posX = -1, int posY = -1){
        if(posX >= 0)
            rect.x = posX;
        if(posY >= 0)  
            rect.y = posY;
        SDL_RenderCopy(rendererOfText, texture, nullptr, &rect);
    }

    void resetTextAndColorUTF8Blended(const std::string &text_, SDL_Color color){
        if(texture == nullptr){
            std::cout << "resetTextAndColor Error! this object has not initialize!\n(neither font nor renderer has been setted)" << std::endl;
            return;
        } 
        createUTF8Blended(font, text_, color, rendererOfText);
    }

    void free(){
        if(texture != nullptr){
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    SDL_Texture *texture = nullptr;
    std::string text;
    SDL_Color textColor;
    SDL_Rect rect;
    TTF_Font *font = nullptr;
    SDL_Renderer *rendererOfText = nullptr;
};

//draw rect
class FillRectWrap{
public:
    //rect_ = {x, y, w, h}
    FillRectWrap(){}
    FillRectWrap(const SDL_Rect& rect_, const SDL_Color& color_):rect{rect_},color{color_}{}

    void draw(SDL_Renderer *renderer){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_Rect rect;
    SDL_Color color;
};

#endif
