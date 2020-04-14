#ifndef _RUSSIANBLOCKGAME_H_
#define _RUSSIANBLOCKGAME_H_
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<mySDL2Tools/MySDLStruct.h>
#include<lua.hpp>
#include"RussianBlockGameUI.h"
#include<iostream>
#include<fstream>
#include<string>
#include<random>
#include<ctime>
#include<vector>
#include<algorithm>

extern lua_State* luaStack;
extern int frame;
extern float msPerFrame;
extern int SCR_WIDTH;
extern int SCR_HEIGHT;
extern const char* fontFilename;
extern SDL_Color backgroundColor;
extern const char* heroListFilename;

namespace RussianBlocks{
    enum State{
        LOADING,
        STARTMENU,
        PLAY,
        HEROESLIST,
        PAUSE,
        GAMEOVER,
        EXIT,
        TOTNUMER
    };

    extern RussianBlocks::State gameState;
    extern Uint32 clock;
    extern std::default_random_engine randomE;

    extern void loading(SDL_Renderer *renderer);
    extern void startMenu(SDL_Renderer *renderer);
    extern void heroesList(SDL_Renderer *renderer);
    extern void play(SDL_Renderer *renderer);
    extern void gameover(SDL_Renderer *renderer);
    extern bool pause(SDL_Renderer *renderer);
    extern bool messageHint(SDL_Renderer *renderer, const std::string& hintString);

    struct heroRecord{
        static const int nameLen = 30;
        static const SDL_Color activeColor;
        char name[nameLen];
        int score;
        bool isActive;
        heroRecord():isActive{false}{}
        heroRecord(const heroRecord& rhs):isActive{rhs.isActive}{
            for(int i = 0; i < nameLen; ++i){
                name[i] = rhs.name[i];
            }
            score = rhs.score;
        }
        heroRecord(heroRecord&& rhs):isActive{rhs.isActive}{
            std::swap(name, rhs.name);
            score = rhs.score;
        }
        heroRecord & operator = (heroRecord &&rhs){
            if(this == &rhs) return *this;
            std::swap(name, rhs.name);
            score = rhs.score;
            isActive = rhs.isActive;
            return *this;
        }
        heroRecord & operator = (const heroRecord & rhs){
            if(this == &rhs) return *this;
            heroRecord temp{rhs};
            std::swap(*this, temp);
            isActive = rhs.isActive;
            return *this;
        }
        void set(std::string name_ = "---", int score_ = 0, bool isActive_ = false){
            name_.copy(name, nameLen, 0);
            if(name_.length() < nameLen){
                name[name_.length()] = '\0';
            }
            else{
                name[nameLen - 1] = '\0';
            }
            score = score_;
            isActive = isActive_;
        }
    };

    class unitBlockBase{
    public:
        unitBlockBase(SDL_Renderer *renderer_):renderer{renderer_}{}
        virtual ~unitBlockBase(){}

        //set renderer color before drawBlock.
        void drawBlock(int posX, int posY){
            int spacing = 1;
            SDL_Rect drawRect = {.x = posX + spacing, 
                                .y = posY + spacing,
                                .w = unitBlockSize - 2 * spacing,
                                .h = unitBlockSize - 2 * spacing};
            
            SDL_RenderDrawRect(renderer, &drawRect);
            spacing = 0.15 * unitBlockSize;
            drawRect = {.x = posX + spacing, 
                                .y = posY + spacing,
                                .w = unitBlockSize - 2 * spacing,
                                .h = unitBlockSize - 2 * spacing};
            
            SDL_RenderFillRect(renderer, &drawRect);
        }

        int unitBlockSize;
        SDL_Renderer *renderer;
    };

    //active block
    class activeBlock{
    public:
        activeBlock()noexcept:isActive{false},data{nullptr},
                    posInBlockDataX{0},posInBlockDataY{0}{}
        activeBlock(const activeBlock& rhs)noexcept{
            isActive = rhs.isActive;
            blockColorIndex = rhs.blockColorIndex;
            w = rhs.w;
            h = rhs.h;
            realXMin = rhs.realXMin;
            realXMax = rhs.realXMax;
            realYMin = rhs.realYMin;
            realYMax = rhs.realYMax;
            posInBlockDataX = rhs.posInBlockDataX;
            posInBlockDataY = rhs.posInBlockDataY;
            if(rhs.data != nullptr){
                data = new bool[w * h];
                for(int i = 0; i < w * h; ++i)
                    data[i] = rhs.data[i];
            }
            else data = nullptr;
        }
        activeBlock(activeBlock &&rhs)noexcept{
            isActive = rhs.isActive;
            blockColorIndex = rhs.blockColorIndex;
            w = rhs.w;
            h = rhs.h;
            realXMin = rhs.realXMin;
            realXMax = rhs.realXMax;
            realYMin = rhs.realYMin;
            realYMax = rhs.realYMax;
            posInBlockDataX = rhs.posInBlockDataX;
            posInBlockDataY = rhs.posInBlockDataY;
            data = rhs.data;
            rhs.data = nullptr;
        }
        activeBlock& operator = (activeBlock&& rhs)noexcept{
            if(this == &rhs)
                return *this;
            isActive = rhs.isActive;
            blockColorIndex = rhs.blockColorIndex;
            w = rhs.w;
            h = rhs.h;
            realXMin = rhs.realXMin;
            realXMax = rhs.realXMax;
            realYMin = rhs.realYMin;
            realYMax = rhs.realYMax;
            posInBlockDataX = rhs.posInBlockDataX;
            posInBlockDataY = rhs.posInBlockDataY;
            std::swap(data, rhs.data);
            return *this;
        }

        activeBlock& operator = (const activeBlock& rhs)noexcept{
            if(this == &rhs) return *this;
            activeBlock temp{rhs};
            std::swap(*this, temp);
            return *this;
        }

        ~activeBlock(){
            free();            
        }
        void free(){
            if(data != nullptr){
                delete[] data;
                data = nullptr;
                w = 0;
                h = 0;
                isActive = false;
            }
        }
        // void setActiveBlock(int width, int height,
        //                     int leftTopX, int leftTopY){
        //     data = new bool[width * height];
        //     posInBlockDataX = leftTopX;
        //     posInBlockDataY = leftTopY;
        // }

        //get component at i-th row, j-th col.
        bool &at(int i, int j){
            return data[i * w + j];
        }

        void setRealXYMaxMin(){
            int i, j;
            bool flag = false;
            for(i = 0; i < h; ++i){
                for(j = 0; j < w; ++j){
                    if(at(i, j)){
                        realYMin = i;
                        flag = true;
                        break;
                    }
                }
                if(flag)
                    break;
            }
            ++i;
            for(; i < h; ++i){
                flag = true;
                for(j = 0; j < w; ++j){
                    if(at(i, j)){
                        flag = false;  
                        break;   
                    }               
                }
                if(flag){
                    break;
                }
            }
            realYMax = i;

            flag = false;
            for(j = 0; j < w; ++j){
                for(i = 0; i < h; ++i){
                    if(at(i, j)){
                        realXMin = j;
                        flag = true;
                        break;
                    }
                }
                if(flag)
                    break;
            }
            ++j;
            for(; j < w; ++j){
                flag = true;
                for(i = 0; i < h; ++i){
                    if(at(i, j)){
                        flag = false;
                        break;
                    }
                }
                if(flag)
                    break;
            }
            realXMax = j;
        }

        //used for debug
        void output(){
            for(int i = 0; i < h; ++i){
                for(int j = 0; j < w; ++j){
                    std::cout << at(i, j) << " ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }

        bool isActive;
        int blockColorIndex;
        int w, h;
        int realXMin, realXMax, realYMin, realYMax;
        bool *data;
        int posInBlockDataX, posInBlockDataY;
    };

    class nextBlockHintRender: public unitBlockBase{
    public:
        nextBlockHintRender()noexcept:unitBlockBase{nullptr}{}

        nextBlockHintRender(SDL_Renderer *renderer_,
                            int leftTopPosX, int leftTopPosY,
                            int width_)noexcept:
                            unitBlockBase{renderer_},
                            posX{leftTopPosX},
                            posY{leftTopPosY},
                            width{width_}{}
        
        nextBlockHintRender(const nextBlockHintRender &rhs):unitBlockBase{rhs.renderer},
                                theBlock{rhs.theBlock}{
            unitBlockSize = rhs.unitBlockSize;
            color = rhs.color;
            posX = rhs.posX;
            posY = rhs.posY;
            width = rhs.width;
        }

        nextBlockHintRender(nextBlockHintRender &&rhs):unitBlockBase{rhs.renderer},
                                theBlock{std::move(rhs.theBlock)}{
            unitBlockSize = rhs.unitBlockSize;
            color = rhs.color;
            posX = rhs.posX;
            posY = rhs.posY;
            width = rhs.width;
        }

        nextBlockHintRender& operator = (nextBlockHintRender&& rhs){
            if(this == &rhs) return *this;
            renderer = rhs.renderer;
            unitBlockSize = rhs.unitBlockSize;
            color = rhs.color;
            posX = rhs.posX;
            posY = rhs.posY;
            width = rhs.width;
            std::swap(theBlock, rhs.theBlock);
            return *this;
        }

        nextBlockHintRender& operator = (const nextBlockHintRender &rhs){
            if(this == &rhs) return *this;
            nextBlockHintRender temp{rhs};
            std::swap(*this, temp);
            return *this;
        }

        void setTheBlock(const activeBlock& nextBlock, SDL_Color color_){
            theBlock = nextBlock;
            color = color_;
            unitBlockSize = width / theBlock.w;
        } 

        void render(){
            int x, y;
            y = posY;
            SDL_SetRenderDrawColor(renderer, color.r,
                                    color.g, color.b, color.a);
            for(int i = 0; i < theBlock.h; ++i){
                x = posX;
                for(int j = 0; j < theBlock.w; ++j){
                    if(theBlock.at(i, j)){
                        drawBlock(x, y);
                    }
                    x += unitBlockSize;
                }
                y += unitBlockSize;
            }
        }

        SDL_Color color;
        activeBlock theBlock;
        int posX, posY;
        int width;
    };

    //record blocks data when playing game.
    class blockData:public unitBlockBase{
    public:
        blockData()noexcept:unitBlockBase{nullptr},
                    posX{0}, posY{0},
                    rows{sizeof(currentState)/sizeof(currentState[0])},
                    cols{sizeof(currentState[0])/sizeof(int)},nextBlock{nullptr},
                    colorList{nullptr},definedBlockData{nullptr},
                    countTime{0},gameoverFlag{false},nextBlockIndex{0},savedFlag{false}{
        }
        blockData(SDL_Renderer* renderer_, const char* configFileName, int leftTopPosX_,
                    int leftTopPosY_, int widthOfBlock)noexcept:
                    posX{leftTopPosX_}, posY{leftTopPosY_},
                    rows{sizeof(currentState)/sizeof(currentState[0])},
                    cols{sizeof(currentState[0])/sizeof(int)},
                    unitBlockBase{renderer_},nextBlock{nullptr},
                    colorList{nullptr},definedBlockData{nullptr},
                    countTime{0},gameoverFlag{false},nextBlockIndex{0},savedFlag{false}{
            unitBlockSize = widthOfBlock / cols;
            if(!loadConfig(configFileName)){
                std::cout << "Error: Failed to load config file when initialize class blockData!" << std::endl;
                free();
            }

            //initial currentState
            for(int i = 0; i < rows; ++i){
                for(int j = 0; j < cols; ++j)
                    currentState[i][j] = -1;
            }

            randomColor.param(std::uniform_int_distribution<int>(0, numOfColorInList - 1).param());
            randomBlock.param(std::uniform_int_distribution<unsigned>(0, numOfDefinedBlockData - 1).param());
            newActiveBlock();

            //set scores;
            scoreEachLine = 2000 / msPerMoveDown;
            scores = 0;
        }

        //copy constructor
        blockData(const blockData & rhs)noexcept:unitBlockBase{rhs.renderer}{
            unitBlockSize = rhs.unitBlockSize;
            numOfColorInList = rhs.numOfColorInList;
            if(rhs.colorList != nullptr){
                colorList = new SDL_Color[numOfColorInList];
                for(int i = 0; i < numOfColorInList; ++i){
                    colorList[i] = rhs.colorList[i];
                }
            }
            else   
                colorList = nullptr;

            cols = rhs.cols;
            rows = rhs.rows;
            countTime = rhs.countTime;
            currentBlock = rhs.currentBlock;

            for(int i = 0; i < rows; ++i)
                for(int j = 0; j < cols; ++j){
                    currentState[i][j] = rhs.currentState[i][j];
                }

            numOfDefinedBlockData = rhs.numOfDefinedBlockData;
            if(rhs.definedBlockData != nullptr){
                definedBlockData = new activeBlock[numOfDefinedBlockData];
                for(int i = 0; i < numOfDefinedBlockData; ++i)
                    definedBlockData[i] = rhs.definedBlockData[i];
            }
            else definedBlockData = nullptr;

            gameoverFlag = rhs.gameoverFlag;
            msPerMoveDown = rhs.msPerMoveDown;
            nextBlockIndex = rhs.nextBlockIndex;
            nextBlock = &definedBlockData[nextBlockIndex];
            posX = rhs.posX;
            posY = rhs.posY;
            randomBlock = rhs.randomBlock;
            randomColor = rhs.randomColor;
            scoreEachLine = rhs.scoreEachLine;
            scores = rhs.scores;

            savedFlag = rhs.savedFlag;
        }

        // move constructor
        blockData(blockData &&rhs)noexcept:unitBlockBase{rhs.renderer},currentBlock{std::move(rhs.currentBlock)}{
            unitBlockSize = rhs.unitBlockSize;
            numOfColorInList = rhs.numOfColorInList;
            colorList = rhs.colorList;
            rhs.colorList = nullptr;

            cols = rhs.cols;
            rows = rhs.rows;
            countTime = rhs.countTime;
            // currentBlock = std::move(rhs.currentBlock);

            std::swap(currentState, rhs.currentState);

            numOfDefinedBlockData = rhs.numOfDefinedBlockData;
            definedBlockData = rhs.definedBlockData;
            rhs.definedBlockData = nullptr;

            gameoverFlag = rhs.gameoverFlag;
            msPerMoveDown = rhs.msPerMoveDown;
            nextBlockIndex = rhs.nextBlockIndex;
            nextBlock = &definedBlockData[nextBlockIndex];
            posX = rhs.posX;
            posY = rhs.posY;
            randomBlock = std::move(rhs.randomBlock);
            randomColor = std::move(rhs.randomColor);
            scoreEachLine = rhs.scoreEachLine;
            scores = rhs.scores;

            savedFlag = rhs.savedFlag;
        }

        //move assignment
        blockData &operator = (blockData && rhs){
            if(this == &rhs) return *this;
            renderer = rhs.renderer;
            unitBlockSize = rhs.unitBlockSize;
            numOfColorInList = rhs.numOfColorInList;
            std::swap(colorList, rhs.colorList);

            cols = rhs.cols;
            rows = rhs.rows;
            countTime = rhs.countTime;
            std::swap(currentBlock, rhs.currentBlock);
            std::swap(currentState, rhs.currentState);

            numOfDefinedBlockData = rhs.numOfDefinedBlockData;
            std::swap(definedBlockData, rhs.definedBlockData);

            gameoverFlag = rhs.gameoverFlag;
            msPerMoveDown = rhs.msPerMoveDown;
            nextBlockIndex = rhs.nextBlockIndex;
            nextBlock = &definedBlockData[nextBlockIndex];
            posX = rhs.posX;
            posY = rhs.posY;
            std::swap(randomBlock, rhs.randomBlock);
            std::swap(randomColor, rhs.randomColor);
            scoreEachLine = rhs.scoreEachLine;
            scores = rhs.scores;
            savedFlag = rhs.savedFlag;
            return *this;
        }
        
        //copy assignment
        blockData& operator = (const blockData &rhs){
            if(this == &rhs) return *this;
            blockData temp{rhs};
            std::swap(*this, temp);
            return *this;
        }

        ~blockData(){
            free();
        }

        bool loadConfig(const char* configFileName){
            lua_State *L = luaL_newstate();
            if(luaL_loadfile(L, configFileName) || lua_pcall(L, 0, 0, 0)){
                std::cout << "Error: Failed to run lua code in " << configFileName << std::endl;
                return false;
            }

            int tmpResult;
            if(!readLuaInteger(L, "msPerMoveDown", tmpResult)) return false;
            msPerMoveDown = tmpResult;

            readLuaInteger(L, "numOfColorInList", numOfColorInList);
            colorList = new SDL_Color[numOfColorInList];

            //read table colorList
            lua_getglobal(L, "colorList");
            for(int i = 0; i < numOfColorInList; ++i){
                for(int nth = 0; nth < 4; ++nth){
                    lua_pushinteger(L, 4 * i + nth + 1);
                    lua_gettable(L, -2);
                    int isnum;
                    int result = lua_tointegerx(L, -1, &isnum);
                    if(!isnum){
                        std::cout << "Error: All component in colorList should be integer!" << std::endl;
                        return false;
                    }
                    switch(nth){
                        case 0:
                            colorList[i].r = result;
                            break;
                        case 1:
                            colorList[i].g = result;
                            break;
                        case 2:
                            colorList[i].b = result;
                            break;
                        case 3:
                            colorList[i].a = result;
                            break;
                    }
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);

            //read defined block data;
            readLuaInteger(L, "numOfDefinedBlockData", numOfDefinedBlockData);
            definedBlockData = new activeBlock[numOfDefinedBlockData];
            lua_getglobal(L, "blocksShape");
            lua_getglobal(L, "blocks");
            for(int i = 0; i < numOfDefinedBlockData; ++i){
                //read width and height
                lua_pushinteger(L, i + 1);
                lua_gettable(L, -3);
                lua_pushinteger(L, 1);
                lua_gettable(L, -2);
                definedBlockData[i].h = lua_tointeger(L, -1);
                lua_pop(L, 1);
                lua_pushinteger(L, 2);
                lua_gettable(L, -2);
                definedBlockData[i].w = lua_tointeger(L, -1);
                lua_pop(L, 2);

                //read block defined data
                lua_pushinteger(L, i + 1);
                lua_gettable(L, -2);
                int count = 1;
                int result;
                definedBlockData[i].data = new bool[definedBlockData[i].w * definedBlockData[i].h];
                for(int m = 0; m < definedBlockData[i].h; ++m){
                    for(int n = 0; n < definedBlockData[i].w; ++n){
                        lua_pushinteger(L, count);
                        lua_gettable(L, -2);
                        result = lua_tointeger(L, -1);
                        lua_pop(L, 1);
                        if(result == 0){
                            definedBlockData[i].at(m, n) = false;
                        }
                        else
                            definedBlockData[i].at(m, n) = true;
                        ++count;
                    }
                }

                lua_pop(L, 1);
            }

            lua_pop(L, 2);
            lua_close(L);
            return true;
        }

        void free(){
            if(colorList != nullptr){
                delete[] colorList;
                colorList = nullptr;
                numOfColorInList = 0;
            }
            if(definedBlockData != nullptr){
                delete[] definedBlockData;
                definedBlockData = nullptr;
                numOfDefinedBlockData = 0;
            }
        }

        //渲染当前控制的方块。
        void renderActiveBlock(){
            if(currentBlock.data!=nullptr){
                SDL_Color color = colorList[currentBlock.blockColorIndex];
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                int x, y;
                y = posY + currentBlock.posInBlockDataY * unitBlockSize;
                for(int i = 0; i < currentBlock.h; ++i){
                    x = posX + currentBlock.posInBlockDataX * unitBlockSize;
                    for(int j = 0; j < currentBlock.w; ++j){
                        if(currentBlock.at(i, j)){
                            if(y >= posY){
                                drawBlock(x, y);
                            }
                        }
                        x += unitBlockSize;
                    }
                    y += unitBlockSize;
                }
            }
        }

        //渲染静止的方块
        void render(){
            int x, y;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            y = posY + rows * unitBlockSize;
            SDL_RenderDrawLine(renderer, posX, y,
                                        posX + cols * unitBlockSize, y);
            SDL_Color *drawColor;
            y = posY;
            for(int i = 0; i < rows; ++i){
                x = posX;
                for(int j = 0; j < cols; ++j){
                    if(currentState[i][j] >= 0){
                        drawColor = &colorList[currentState[i][j]];
                        SDL_SetRenderDrawColor(renderer, drawColor->r, 
                                            drawColor->g, drawColor->b,
                                            drawColor->a);
                        drawBlock(x, y);
                    }
                    x += unitBlockSize;
                }
                y += unitBlockSize;
            }
            renderActiveBlock();
        }

        //碰撞检测
        //return true: 有碰撞
        bool isCollided(int deltaX, int deltaY, activeBlock* currentBlock_){
            if(currentBlock_ == nullptr) return false;

            int x = currentBlock_->posInBlockDataX + deltaX;
            int y = currentBlock_->posInBlockDataY + deltaY;
            int testX, testY;
            for(int i = 0; i < currentBlock_->h; ++i){
                for(int j = 0; j < currentBlock_->w; ++j){
                    if(currentBlock_->at(i, j)){
                        testX = x + j;
                        testY = y + i;
                        if(testX < 0 || testX >= cols ||
                            testY >= rows)
                            return true; //out of boundary.
                        if(testY >= 0 && currentState[testY][testX] != -1)
                            return true; //collided
                    }
                }
            }

            return false;
        }

        //方块下落一格
        //return true: if no collision,
        //return false: if collision happened.
        //parameter scoreText: used for update score num text;
        bool moveDown(Uint32 deltaTime, TextWrap &scoreText){
            countTime += deltaTime;
            if(countTime > msPerMoveDown){
                bool res;
                res = updateCurrentState(scoreText);
                countTime = 0;
                return res;
            }
            return true;
        }

        void rotateBlock(activeBlock& theBlock){
            bool tmpData[20][20];
            if(theBlock.data == nullptr) return;
            for(int i = 0; i < theBlock.h; ++i){
                for(int j = 0; j < theBlock.w; ++j){
                    tmpData[theBlock.w - j - 1][i] = theBlock.at(i, j);
                }
            }
            std::swap(theBlock.w, theBlock.h);
            for(int i = 0; i < theBlock.h; ++i){
                for(int j = 0; j < theBlock.w; ++j){
                    theBlock.at(i, j) = tmpData[i][j];
                }
            }
        }

        void rotateBlockBack(activeBlock& theBlock){
            bool tmpData[20][20];
            if(theBlock.data == nullptr) return;
            for(int i = 0; i < theBlock.h; ++i){
                for(int j = 0; j < theBlock.w; ++j){
                    tmpData[j][theBlock.h - i - 1] = theBlock.at(i, j);
                }
            }
            std::swap(theBlock.w, theBlock.h);
            for(int i = 0; i < theBlock.h; ++i){
                for(int j = 0; j < theBlock.w; ++j){
                    theBlock.at(i, j) = tmpData[i][j];
                }
            }
        }

        void optimizeRotation(activeBlock& theBlock){
            if(theBlock.data == nullptr) return;
            activeBlock tmpBlock = theBlock;

            rotateBlock(theBlock);
            bool collideFlag = true;
            bool continueFlag = false;
            // bool breakFlag = false;
            int x, y;
            int testX, testY;
            while(collideFlag){
                x = theBlock.posInBlockDataX;
                y = theBlock.posInBlockDataY;
                collideFlag = false;
                for(int i = 0; i < theBlock.h; ++i){
                    for(int j = 0; j < theBlock.w; ++j){
                        if(theBlock.at(i, j)){
                            testX = x + j;
                            testY = y + i;
                            if(testX < 0){
                                theBlock.posInBlockDataX += 1;
                                continueFlag = true;
                                break;
                            }
                            else if(testX >= cols){
                                theBlock.posInBlockDataX -= 1;
                                continueFlag = true;
                                break;
                            }
                            else if(testY >= rows){
                                theBlock.posInBlockDataY -= 1;
                                continueFlag = true;
                                break;
                            }
                        }
                    }
                    if(continueFlag){
                        continueFlag = false;
                        collideFlag = true;
                        break;
                    }
                }
            }
            if(isCollided(0, 0, &theBlock)){
                theBlock = tmpBlock;
            }
        }

        void newActiveBlock(){
            if(nextBlock != nullptr)
                currentBlock = *nextBlock;
            nextBlockIndex = randomBlock(randomE);
            nextBlock = &definedBlockData[nextBlockIndex];
            // nextBlock->output();

            std::uniform_int_distribution<unsigned> randomRot{0, 3};
            int rotTimes = randomRot(randomE);
            for(int i = 0; i < rotTimes; ++i){
                rotateBlock(*nextBlock);
                // nextBlock->output();
            }

            //set inintial position
            nextBlock->setRealXYMaxMin();

            std::uniform_int_distribution<int> randomPosX{-nextBlock->realXMin, 
                                                                cols - nextBlock->realXMax};
            nextBlock->posInBlockDataX = randomPosX(randomE);
            nextBlock->posInBlockDataY = -nextBlock->realYMin;

            //set color of nextBlock
            nextBlock->blockColorIndex = randomColor(randomE);

            if(currentBlock.data != nullptr && isCollided(0, 0, &currentBlock)){
                gameoverFlag = true;
            }

        }

        void test(){
            for(int i = 0; i < rows; ++i)
                for(int j = 0; j < cols; ++j){
                    currentState[i][j] = randomColor(randomE);
                }
        }

        //currentState[i][j] is an integer rep the state of block at i-th row j-th col,
        //If the integer == -1, no block at this position, otherwise, the integer is the 
        //index of color in the color list.  
        int currentState[20][10];
        int rows, cols;
        SDL_Color *colorList;
        int numOfColorInList;
        int posX, posY;
        Uint32 msPerMoveDown;
        Uint32 countTime;
        activeBlock *nextBlock;
        activeBlock currentBlock;
        unsigned int scores, scoreEachLine;
        bool gameoverFlag;

        activeBlock *definedBlockData;
        int numOfDefinedBlockData;

        std::uniform_int_distribution<int> randomColor;
        std::uniform_int_distribution<unsigned> randomBlock;

        bool savedFlag;

    private:

        bool annihilateLines(){
            std::vector<int> indexOfRowsDisspr;
            bool flag;
            for(int i = 0; i < rows; ++i){
                flag = true;
                for(int j = 0; j < cols; ++j){
                    if(currentState[i][j] == -1){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    indexOfRowsDisspr.push_back(i);
                }
            }
            int len = indexOfRowsDisspr.size();
            if(len == 0) return false;

            scores += (scoreEachLine * len);

            //play animation of annihilation
            Uint32 oldtime = SDL_GetTicks();
            Uint32 currentTime;
            Uint32 animationTimeLength = 450;
            Uint32 flashTime = animationTimeLength / 4;
            Uint32 deltaTime = 0;
            bool alphaFlag = true;
            SDL_Color drawColor;
            int x, y;
            SDL_Rect backRect = {.x = posX, .y = posY,
                                 .w = cols * unitBlockSize,
                                 .h = rows * unitBlockSize};
            SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                    backgroundColor.g, backgroundColor.b,
                                    backgroundColor.a);
            SDL_RenderFillRect(renderer, &backRect);
            render();
            while(deltaTime < animationTimeLength){
                currentTime = SDL_GetTicks();
                deltaTime = currentTime - oldtime;
                if((deltaTime / flashTime) % 2 == 0){
                    alphaFlag = true;
                }
                else{
                    alphaFlag = false;
                }
                SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                    backgroundColor.g, backgroundColor.b,
                                    backgroundColor.a);
                for(auto itr = indexOfRowsDisspr.begin();
                            itr != indexOfRowsDisspr.end();
                            ++itr){
                    for(int j = 0; j < cols; ++j){
                        if(alphaFlag){
                            drawColor = colorList[currentState[(*itr)][j]];
                            SDL_SetRenderDrawColor(renderer,
                                                drawColor.r, drawColor.g,
                                                drawColor.b, drawColor.a);
                        }
                        x = posX + j * unitBlockSize;
                        y = posY + (*itr) * unitBlockSize;
                        drawBlock(x, y);
                    }
                }
                SDL_RenderPresent(renderer);

                currentTime = SDL_GetTicks() - currentTime;
                if(currentTime < msPerFrame){
                    SDL_Delay(msPerFrame - currentTime);
                }
            }

            //Blocks fall down
            for(int i = 0; i < len; ++i){
                for(int linenum = indexOfRowsDisspr[i]; linenum > i; --linenum){
                    for(int j = 0; j < cols; ++j){
                        currentState[linenum][j] = currentState[linenum - 1][j];
                    }
                }
            }
            for(int i = 0; i < len; ++i){
                for(int j = 0; j < cols; ++j)
                    currentState[i][j] = -1;
            }
            return true;
        }

        //return true: if no collision happened
        //return false: if collision heppened.
        bool updateCurrentState(TextWrap &scoreText){
            if(currentBlock.data == nullptr) return true;
            if(isCollided(0, 1, &currentBlock)){
                //add to currentState array.
                int x, y;
                x = currentBlock.posInBlockDataX;
                y = currentBlock.posInBlockDataY;
                for(int i = 0; i < currentBlock.h; ++i){
                    for(int j = 0; j < currentBlock.w; ++j){
                        if(currentBlock.at(i, j)){
                            currentState[y + i][x + j] = currentBlock.blockColorIndex;
                        }
                    }
                }
                if(annihilateLines()){
                    scoreText.resetTextAndColorUTF8Blended(std::to_string(scores), scoreText.textColor);
                }

                currentBlock.free();
                newActiveBlock();
                return false;
            }
            else{
                currentBlock.posInBlockDataY += 1;
                return true;
            }
        }

        bool readLuaInteger(lua_State *L, const char* varName, int &var){
            lua_getglobal(L, varName);
            int isnum;
            var = lua_tointegerx(L, -1, &isnum);
            if(!isnum){
                std::cout << varName << " should be an integer" << std::endl;
                return false;
            }
            lua_pop(L, 1);
            return true;
        }


        int nextBlockIndex;

    };

    class HeroListType{
    public:
        HeroListType()noexcept:len{sizeof(list)/sizeof(list[0])}{}
        void loadData(const char* filename){
            filename_ = filename;
            file.open(filename, std::ios::in|std::ios::out|std::ios::binary);
            if(!file){
                // file not exist!
                file.close();
                file.open(filename, std::ios::out|std::ios::binary);         
                for(int i = 0; i < len; ++i){
                    list[i].set();
                }
                file.write((char*)(&list[0]), sizeof(list));
            }
            else{
                file.read((char*)(&list[0]), sizeof(list));
            }
            file.close();
            for(int i = 0; i < len; ++i){
                list[i].isActive = false;
            }
        }

        void addRecord(const std::string& name, unsigned int score){
            list[14].set(name, score, true);
            std::sort(&list[0], &list[15], 
                        [](const RussianBlocks::heroRecord &x, const RussianBlocks::heroRecord &y){
                                        long xs = (std::string(x.name) == "---")?-1:x.score;
                                        long ys = (std::string(y.name) == "---")?-1:y.score;
                                        return xs > ys;
                                    });
            file.open(filename_, std::ios::out|std::ios::binary);
            file.write((char*)(&list[0]), sizeof(list));
            file.close();
        }

        RussianBlocks::heroRecord list[15];
        const int len;
        std::string filename_;
        std::fstream file;
    };

    extern HeroListType heroList;
}

extern RussianBlocks::blockData blockdata;

#endif