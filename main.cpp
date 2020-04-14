#include<iostream>
#include<SDL2/SDL.h>
#include<mySDL2Tools/MySDLStruct.h>
#include<SDL2/SDL_ttf.h>
#include"RussianBlockGame.h"
#include<lua.hpp>

//Global parameters definition
std::default_random_engine RussianBlocks::randomE;
const char* configfilename = "config.lua"; 
const char* heroListFilename = "GameData0";
RussianBlocks::HeroListType RussianBlocks::heroList;
const SDL_Color RussianBlocks::heroRecord::activeColor = {255, 50, 251, 255}; 

int SCR_WIDTH = 600;
int SCR_HEIGHT = 800;
int frame = 30;
float msPerFrame = 1000.0 / frame;
const char* fontFilename = "fangzhengxiangsu14.ttf";
namespace RussianBlocks{
    State gameState = RussianBlocks::LOADING;
    Uint32 time, deltaTime;
}   

lua_State *luaStack;

void readConfig();

int main(int argv, char* argc[]){
    RussianBlocks::randomE.seed(std::time(0));
    readConfig();

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Failed to initialize SDL! SDL Error:\n" << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Window *window = SDL_CreateWindow("RussianBlocks", 
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_ShowCursor(0);
    if(window == nullptr){
        std::cout << "Failed to create window! SDL Error:\n" << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, SCR_WIDTH, SCR_HEIGHT);

    TTF_Init();

    // SDL_Event eve;

    while(RussianBlocks::gameState != RussianBlocks::EXIT){
        switch(RussianBlocks::gameState){
            case RussianBlocks::LOADING:
                RussianBlocks::loading(renderer);
                break;
            case RussianBlocks::STARTMENU:
                RussianBlocks::startMenu(renderer);
                break;
            case RussianBlocks::PLAY:
                RussianBlocks::play(renderer);
                break;
            case RussianBlocks::HEROESLIST:
                RussianBlocks::heroesList(renderer);
                break;
            case RussianBlocks::GAMEOVER:
                // std::cout << "gameover!" << std::endl;
                RussianBlocks::gameover(renderer);
                break;
            case RussianBlocks::PAUSE:
                // std::cout << "pause!" << std::endl;
                RussianBlocks::pause(renderer);
                break;
        }
    }
    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

int getGlobInt(lua_State* L, const char* varName){
    int isnum, result;
    lua_getglobal(L, varName);
    result = lua_tointegerx(L, -1, &isnum);
    if(!isnum){
        std::cout << varName << " is not a number " << std::endl;
        exit(EXIT_FAILURE);
    }
    lua_pop(L, 1);
    return result;
}

int getColorComponent(lua_State *L, const char* varName, const char* colorName){
    int result, isnum;
    lua_pushstring(L, colorName);
    lua_gettable(L, -2);
    result = lua_tointegerx(L, -1, &isnum);
    if(!isnum){
        std::cout << varName << "." << colorName << " is not a integer!\n" << lua_tostring(L, -1) << std::endl;
        exit(EXIT_FAILURE);
    }
    if(result < 0 || result > 255){
        std::cout << varName << "." << colorName << " out of range! (should be in 0~255)" << std::endl;
        exit(EXIT_FAILURE); 
    }
    lua_pop(L, 1);
    return result;
}

SDL_Color getColor(lua_State *L, const char* varName){
    lua_getglobal(L, varName);
    SDL_Color resColor;
    resColor.r = getColorComponent(L, varName, "r");
    resColor.g = getColorComponent(L, varName, "g");
    resColor.b = getColorComponent(L, varName, "b");
    resColor.a = getColorComponent(L, varName, "a");
    lua_pop(L, 1);
    return resColor;
}

void readConfig(){
    luaStack = luaL_newstate();
    if(luaL_loadfile(luaStack, configfilename)||lua_pcall(luaStack, 0, 0, 0)){
        std::cout << "Failed to run config.lua file!" << std::endl
                    << lua_tostring(luaStack, -1) << std::endl;
        exit(EXIT_FAILURE);
    }
    SCR_WIDTH = getGlobInt(luaStack, "SCR_WIDTH");
    SCR_HEIGHT = getGlobInt(luaStack, "SCR_HEIGHT");
    frame = getGlobInt(luaStack, "frame");
    backgroundColor = getColor(luaStack, "background_color");

    lua_close(luaStack);
}