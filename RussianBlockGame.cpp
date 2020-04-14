#include"RussianBlockGame.h"

SDL_Color backgroundColor = {177, 200, 202, 255};
RussianBlocks::blockData blockdata;
RussianBlocks::nextBlockHintRender nextBlockRenderer;


void RussianBlocks::loading(SDL_Renderer *renderer){

    heroList.loadData(heroListFilename);

    TTF_Font *fontTitle = TTF_OpenFont(fontFilename, 65);
    // TTF_SetFontStyle(fontTitle, TTF_STYLE_BOLD);
    TextWrap title;
    //set title
    title.createUTF8Blended(fontTitle, "俄 罗 斯 方 块", 
                            {0, 0, 0, 255},
                            renderer);
    int desPosition = (SCR_WIDTH - title.rect.w) / 2;
    title.rect.x = -title.rect.w;
    title.rect.y = (SCR_HEIGHT / 2 - title.rect.h) / 2;

    float velocity = (desPosition - title.rect.x) / 2500.0; //2000.0ms

    TextWrap subTitle;
    TTF_Font *fontSubTitle = TTF_OpenFont(fontFilename, 30);
    TTF_SetFontStyle(fontSubTitle, TTF_STYLE_ITALIC);
    subTitle.createUTF8Blended(fontSubTitle,
                        "——by Mao",
                        {0, 0, 0, 255},
                        renderer);
    subTitle.rect.x = (title.rect.x + title.rect.w - subTitle.rect.w);
    subTitle.rect.y = title.rect.y + title.rect.h + 10;

    TTF_CloseFont(fontSubTitle);
    TTF_CloseFont(fontTitle);

    SDL_Event eve;
    Uint32 time;
    Uint32 oldTime = SDL_GetTicks();
    Uint32 deltaTime = 0, tmpTime = 0;
    int deltaPosX;
    while(RussianBlocks::gameState == RussianBlocks::LOADING){
        time = SDL_GetTicks();
        deltaTime = time - oldTime;
        oldTime = time;
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                RussianBlocks::gameState = RussianBlocks::EXIT;
                return;
            }
            else if(eve.type == SDL_KEYDOWN){
                RussianBlocks::gameState = RussianBlocks::STARTMENU;
                return;
            }
        }
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                                backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);

        deltaPosX = static_cast<int>(deltaTime * velocity);
        title.rect.x += deltaPosX;

        if(title.rect.x >= desPosition){
            RussianBlocks::gameState = RussianBlocks::STARTMENU;
            return;
        }

        subTitle.rect.x += deltaPosX;
        title.render(title.rect.x, title.rect.y);
        subTitle.render(subTitle.rect.x, subTitle.rect.y);

        SDL_RenderPresent(renderer);
        tmpTime = SDL_GetTicks() - time;
        if(tmpTime < msPerFrame){
            SDL_Delay(msPerFrame - tmpTime);
        }
    }

}


void startButtonFunc(){
    RussianBlocks::gameState = RussianBlocks::PLAY;
}

void heroListButtonFunc(){
    RussianBlocks::gameState = RussianBlocks::HEROESLIST;
}

void exitButtonFunc(){
    RussianBlocks::gameState = RussianBlocks::EXIT;
}

void RussianBlocks::startMenu(SDL_Renderer *renderer){
    SDL_Event eve;
    Uint32 time, deltaTime;
    TTF_Font *fontTitle = TTF_OpenFont(fontFilename, 65);
    // TTF_SetFontStyle(fontTitle, TTF_STYLE_BOLD);
    TextWrap title;


    //set title
    title.createUTF8Blended(fontTitle, "俄 罗 斯 方 块", 
                            {0, 0, 0, 255},
                            renderer);
    title.rect.x = (SCR_WIDTH - title.rect.w) / 2;
    title.rect.y = (SCR_HEIGHT / 2 - title.rect.h) / 2;

    TextWrap subTitle;
    TTF_Font *fontSubTitle = TTF_OpenFont(fontFilename, 30);
    TTF_SetFontStyle(fontSubTitle, TTF_STYLE_ITALIC);
    subTitle.createUTF8Blended(fontSubTitle,
                        "——by Mao",
                        {0, 0, 0, 255},
                        renderer);
    subTitle.rect.x = (title.rect.x + title.rect.w - subTitle.rect.w);
    subTitle.rect.y = title.rect.y + title.rect.h + 10;

    //Button
    TTF_Font *fontButton = TTF_OpenFont(fontFilename, 40);    
    const int menuButtonNum = 3;
    RussianBlocks::Button menuButton[menuButtonNum];
    SDL_Color buttonSelectedColor1 = {142, 158, 191, 255};
    SDL_Color buttonSelectedColor2 = {180, 201, 243, 255};
    menuButton[0].setText(fontButton, "开始游戏", {0, 0, 0, 255},
                        buttonSelectedColor1, buttonSelectedColor2, renderer);
    menuButton[0].setTopLeftPosition((SCR_WIDTH - menuButton[0].buttonRect.w) / 2,
                                   static_cast<int>(SCR_HEIGHT * 3.0 / 5.0) - menuButton[0].buttonRect.h / 2);
    menuButton[0].bindFunction = startButtonFunc;
    menuButton[1].setText(fontButton, "英 雄 榜", {0, 0, 0, 255},
                        buttonSelectedColor1, buttonSelectedColor2, renderer);
    menuButton[1].setTopLeftPosition(menuButton[0].buttonRect.x, menuButton[0].buttonRect.y + menuButton[0].buttonRect.h + 15);
    menuButton[1].bindFunction = heroListButtonFunc;
    menuButton[2].setText(fontButton, "离开游戏", {0, 0, 0, 255},
                        buttonSelectedColor1, buttonSelectedColor2, renderer);
    menuButton[2].setTopLeftPosition(menuButton[1].buttonRect.x, menuButton[1].buttonRect.y + menuButton[1].buttonRect.h + 15);
    menuButton[2].bindFunction = exitButtonFunc;

    menuButton[0].isSelected = true;
    int selectedButton = 0;

    while(RussianBlocks::gameState == RussianBlocks::STARTMENU){
        time = SDL_GetTicks();
        while(SDL_PollEvent(&eve)){
            // if(eve.type == SDL_QUIT || eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_q){
            if(eve.type == SDL_QUIT){
                RussianBlocks::gameState = RussianBlocks::EXIT;
                return;
            }
            else if(eve.type == SDL_KEYDOWN){
                switch(eve.key.keysym.sym){
                    case SDLK_UP:
                        menuButton[selectedButton--].isSelected = false;
                        if(selectedButton == -1)
                            selectedButton = menuButtonNum - 1;
                        menuButton[selectedButton].isSelected = true;
                        break;
                    case SDLK_DOWN:
                        menuButton[selectedButton++].isSelected = false;
                        if(selectedButton == menuButtonNum)
                            selectedButton = 0;
                        menuButton[selectedButton].isSelected = true;
                        break;
                    case SDLK_RETURN:
                    case SDLK_SPACE:                        
                        menuButton[selectedButton].bindFunction();
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                        backgroundColor.g,
                                        backgroundColor.b,
                                        backgroundColor.a);
        SDL_RenderClear(renderer);
        //draw title
        title.render(title.rect.x, title.rect.y);
        subTitle.render(subTitle.rect.x, subTitle.rect.y);
        for(int i = 0; i < menuButtonNum; ++i){
            menuButton[i].render(backgroundColor);
        }
        SDL_RenderPresent(renderer);
        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame)
            SDL_Delay(msPerFrame - deltaTime);
    }
    TTF_CloseFont(fontButton);
}

void RussianBlocks::heroesList(SDL_Renderer *renderer){
    TTF_Font *titleFont = TTF_OpenFont(fontFilename, 65);
    TTF_Font *contentFont = TTF_OpenFont(fontFilename, 33);
    TTF_Font *hintFont = TTF_OpenFont(fontFilename, 30);

    TextWrap heroTitle;
    heroTitle.createUTF8Blended(titleFont, "英 雄 榜", {0, 0, 0, 255}, renderer);
    heroTitle.rect.x = (SCR_WIDTH - heroTitle.rect.w) / 2;
    heroTitle.rect.y = static_cast<int>(0.05 * SCR_HEIGHT);

    //Design UI of heroTitle
    int LeftPosX = 0.03 * SCR_WIDTH;
    int RightPosX = 0.97 * SCR_WIDTH;
    //内框的四个端点坐标
    SDL_Point Line1L = {.x = LeftPosX + 5, .y = heroTitle.rect.y - 5};
    SDL_Point Line1R = {.x = RightPosX - 6, .y = Line1L.y};
    SDL_Point Line2L = {.x = LeftPosX + 5, .y = heroTitle.rect.y + heroTitle.rect.h + 5};
    SDL_Point Line2R = {.x = RightPosX - 6, .y = Line2L.y};
    //外框粗线由四个矩形构成
    SDL_Rect line1 = {.x = LeftPosX, .y = heroTitle.rect.y - 10, .w = RightPosX - LeftPosX, .h = 3};
    SDL_Rect line2 = {.x = LeftPosX, .y = heroTitle.rect.y + heroTitle.rect.h + 7,
                        .w = line1.w, .h = 3};
    SDL_Rect line3 = {.x = LeftPosX, .y = line1.y, .w = 3, .h = line2.y - line1.y};
    SDL_Rect line4 = {.x= RightPosX - 3, .y = line1.y, .w = 3, .h = line3.h};

    //Design UI of Content of List
    int contentLeftPosX = 0.05 * SCR_WIDTH;
    int contentRightPosX = 0.95 * SCR_WIDTH;
    int contentY = Line2L.y + 20;

    //The hint: "Press any key to continue..."
    TextWrap heroHint;
    heroHint.createUTF8Blended(hintFont, "按下任意键继续...",
                                {0, 0, 0, 255}, renderer);
    heroHint.rect.x = static_cast<int>(0.05 * SCR_WIDTH);
    heroHint.rect.y = static_cast<int>(0.95 * SCR_HEIGHT - heroHint.rect.h);

    //create texture for contents of heroList
    TextWrap heroContent[3 * RussianBlocks::heroList.len];
    int count = 0;
    std::string num;
    const SDL_Color BlackColor = {0, 0, 0, 255};
    SDL_Color tempColor = BlackColor;
    for(int i = 0; i < RussianBlocks::heroList.len; ++i){
        num = std::to_string(i + 1);
        if(i < 9) num += ".  ";
        else num += ". ";

        if(heroList.list[i].isActive){
            tempColor = RussianBlocks::heroRecord::activeColor;
        }

        heroContent[count].createUTF8Blended(contentFont, num, 
                                                tempColor, renderer);
        heroContent[count].rect.x = contentLeftPosX;
        heroContent[count].rect.y = contentY;
        ++count;

        heroContent[count].createUTF8Blended(contentFont, heroList.list[i].name,
                                                tempColor, renderer);
        heroContent[count].rect.x = contentLeftPosX + heroContent[count - 1].rect.w;
        heroContent[count].rect.y = contentY;
        ++count;

        heroContent[count].createUTF8Blended(contentFont, std::to_string(heroList.list[i].score),
                                                tempColor, renderer);
        heroContent[count].rect.x = contentRightPosX - heroContent[count].rect.w;
        heroContent[count].rect.y = contentY;
        ++count;

        contentY = contentY + heroContent[0].rect.h + 10;

        if(heroList.list[i].isActive){
            heroList.list[i].isActive = false;
            tempColor = BlackColor;
        }
    }

    SDL_Event eve;
    Uint32 time, deltaTime;

    while(RussianBlocks::gameState == RussianBlocks::HEROESLIST){
        time = SDL_GetTicks();
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                RussianBlocks::gameState = RussianBlocks::EXIT;
            }
            else if(eve.type == SDL_KEYDOWN){
                RussianBlocks::gameState = RussianBlocks::STARTMENU;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, 
                                backgroundColor.g,
                                backgroundColor.b, 
                                backgroundColor.a);
        SDL_RenderClear(renderer);
        heroTitle.render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        //Draw UI of title
        SDL_RenderFillRect(renderer, &line1);
        SDL_RenderFillRect(renderer, &line2);
        SDL_RenderFillRect(renderer, &line3);
        SDL_RenderFillRect(renderer, &line4);
        SDL_RenderDrawLine(renderer, Line1L.x, Line1L.y, Line1R.x, Line1R.y);
        SDL_RenderDrawLine(renderer, Line2L.x, Line2L.y, Line2R.x, Line2R.y);
        SDL_RenderDrawLine(renderer, Line1L.x, Line1L.y, Line2L.x, Line2L.y);
        SDL_RenderDrawLine(renderer, Line1R.x, Line1R.y, Line2R.x, Line2R.y);

        for(int i = 0; i < 3 * RussianBlocks::heroList.len; ++i){
            heroContent[i].render();
        }

        heroHint.render();
        SDL_RenderPresent(renderer);

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame){
            SDL_Delay(msPerFrame - deltaTime);
        }
    }
    
    TTF_CloseFont(titleFont);
    TTF_CloseFont(contentFont);
    TTF_CloseFont(hintFont);
}

void RussianBlocks::play(SDL_Renderer *renderer){
    // std::cout << "play" << std::endl;
    //main play UI
    int mainBlockLeftTopPosX = 0.03 * SCR_WIDTH;
    int mainBlockLeftTopPosY = 0.03 * SCR_WIDTH;
    int mainBlockRightDownPosX = 0.65 * SCR_WIDTH;
    int mainBlockRightDownPosY = 2 * (mainBlockRightDownPosX - mainBlockLeftTopPosX) + mainBlockLeftTopPosY - 10;
    RussianBlocks::doubleSquareUI mainBlock{renderer,
                                    mainBlockLeftTopPosX,
                                    mainBlockLeftTopPosY,
                                    mainBlockRightDownPosX,
                                    mainBlockRightDownPosY};

    //next Block UI
    TextWrap nextHint;
    TTF_Font *nextHintFont = TTF_OpenFont(fontFilename, 30);
    TTF_Font *HintFont = TTF_OpenFont(fontFilename, 28);
    nextHint.createUTF8Blended(nextHintFont, "下一个:", 
                                {0, 0, 0, 255}, renderer);
    int nextBlockLTX = mainBlockRightDownPosX + 10;
    int nextBlockLTY = mainBlockLeftTopPosY;
    nextHint.rect.x = nextBlockLTX + 10;
    nextHint.rect.y = nextBlockLTY + 10;
    int nextBlockRDX = 0.97 * SCR_WIDTH;
    int nextBlockRDY = nextBlockLTY + nextBlockRDX - nextBlockLTX + nextHint.rect.h;
    RussianBlocks::doubleSquareUI nextBlockUI{renderer, 
                                    nextBlockLTX,
                                    nextBlockLTY,
                                    nextBlockRDX,
                                    nextBlockRDY};

    //显示得分
    TextWrap getScoresText;
    TTF_Font *getScoresTextFont = TTF_OpenFont(fontFilename, 45);
    getScoresText.createUTF8Blended(nextHintFont, "得分:",
                                    {0, 0, 0, 255}, renderer);
    getScoresText.rect.x = nextBlockLTX;
    getScoresText.rect.y = 0.3 * SCR_HEIGHT;    

    TTF_Font *scoreTextFont = TTF_OpenFont(fontFilename, 55);
    TextWrap scoreNumText;
    scoreNumText.createUTF8Blended(scoreTextFont, "0", 
                                    {0, 0, 0, 255}, renderer);
    scoreNumText.rect.x = getScoresText.rect.x + 5;
    scoreNumText.rect.y = getScoresText.rect.y + getScoresText.rect.h + 2;

    //Hint text
    RussianBlocks::HintTextWrapInPlay hintText{renderer,
                                        HintFont, 
                                        mainBlockLeftTopPosX,
                                        mainBlockRightDownPosY};


    //should load saved data?
    if(blockdata.savedFlag){
        blockdata.savedFlag = false;
        nextBlockRenderer.setTheBlock(*blockdata.nextBlock, 
                            blockdata.colorList[blockdata.nextBlock->blockColorIndex]);
        scoreNumText.resetTextAndColorUTF8Blended(std::to_string(blockdata.scores), scoreNumText.textColor);
    }
    else{
        //load/reload block data
        RussianBlocks::blockData blockdata_{renderer, 
                                        "config.lua",
                                        mainBlock.points[0].x,
                                        mainBlock.points[0].y,
                                        mainBlock.points[1].x - mainBlock.points[0].x};
        blockdata = std::move(blockdata_);
    
        RussianBlocks::nextBlockHintRender nextBlockRenderer_{renderer, 
                                        nextBlockUI.points[0].x,
                                        nextBlockUI.points[0].y + nextHint.rect.h,
                                        nextBlockUI.points[1].x - nextBlockUI.points[0].x};
        nextBlockRenderer = std::move(nextBlockRenderer_);
    }


    //main loop
    SDL_Event eve;
    Uint32 time = 0, deltaTime, oldTime = 0;
    bool pauseFlag = false;
    // bool pauseFlag = false;
    while(RussianBlocks::gameState == RussianBlocks::PLAY){
        oldTime = time;
        time = SDL_GetTicks();
        deltaTime = time - oldTime;
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT)
                RussianBlocks::gameState = RussianBlocks::EXIT;
            else if(eve.type == SDL_KEYDOWN){
                switch(eve.key.keysym.sym){
                    case SDLK_ESCAPE:
                        if(RussianBlocks::messageHint(renderer, "离开将会丢失当前游戏进度，\n是否确定？"))
                            RussianBlocks::gameState = RussianBlocks::STARTMENU;
                        break;
                    case SDLK_UP:
                    case SDLK_r:
                        blockdata.optimizeRotation(blockdata.currentBlock);
                        break;
                    case SDLK_DOWN:
                        if(blockdata.currentBlock.data != nullptr){
                            while(!blockdata.isCollided(0, 1, &blockdata.currentBlock)){
                                blockdata.currentBlock.posInBlockDataY += 1;
                            }
                        }
                        break;
                    case SDLK_LEFT:
                        if(blockdata.currentBlock.data != nullptr && !blockdata.isCollided(-1, 0, &blockdata.currentBlock))
                            blockdata.currentBlock.posInBlockDataX -= 1;
                        break;
                    case SDLK_RIGHT:
                        if(blockdata.currentBlock.data != nullptr && !blockdata.isCollided(1, 0, &blockdata.currentBlock))
                            blockdata.currentBlock.posInBlockDataX += 1;
                        break;
                    case SDLK_SPACE:
                        pauseFlag = true;
                        // blockdata.savedFlag = true;
                        break;
                    //debug
                    // case SDLK_d:
                    //     RussianBlocks::gameState = RussianBlocks::GAMEOVER;
                    //     break;
                    // case SDLK_f:
                    //     blockdata.scores += blockdata.randomBlock(randomE);
                    //     scoreNumText.resetTextAndColorUTF8Blended(std::to_string(blockdata.scores), scoreNumText.textColor);
                    //     break;
                    // case SDLK_c:
                    //     blockdata.newActiveBlock();
                    //     nextBlockRenderer.setTheBlock(*blockdata.nextBlock, 
                    //                     blockdata.colorList[blockdata.nextBlock->blockColorIndex]);
                    //     break;
                }
            }
        }

        if(pauseFlag){
            pauseFlag = RussianBlocks::pause(renderer);
            time = SDL_GetTicks();
            SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                backgroundColor.g,
                                backgroundColor.b,
                                backgroundColor.a);
            SDL_RenderClear(renderer);
            
            //render        
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            mainBlock.render();
            nextBlockUI.render();
            nextHint.render();
            nextBlockRenderer.render();
            getScoresText.render();
            scoreNumText.render();
            hintText.render();
            blockdata.render();

            SDL_RenderPresent(renderer);
            continue;
        }

        //keyboard input.
        const Uint8 *keyState = SDL_GetKeyboardState(nullptr);        
        if(keyState[SDL_SCANCODE_F]){ //按f加速下落
            if(!blockdata.isCollided(0, 1, &blockdata.currentBlock))
                blockdata.currentBlock.posInBlockDataY += 1;
        }

        //game logic
        if(blockdata.currentBlock.data == nullptr){
            blockdata.newActiveBlock();
            nextBlockRenderer.setTheBlock(*blockdata.nextBlock, 
                                        blockdata.colorList[blockdata.nextBlock->blockColorIndex]);
        }
        if(!blockdata.moveDown(deltaTime, scoreNumText)){
            nextBlockRenderer.setTheBlock(*blockdata.nextBlock,
                                        blockdata.colorList[blockdata.nextBlock->blockColorIndex]);
        }


        // if(keyState[SDL_SCANCODE_LEFT]){
        //     if(!blockdata.isCollided(-1, 0, &blockdata.currentBlock))
        //         blockdata.currentBlock.posInBlockDataX -= 1;
        // }
        // if(keyState[SDL_SCANCODE_RIGHT]){
        //     if(!blockdata.isCollided(1, 0, &blockdata.currentBlock))
        //         blockdata.currentBlock.posInBlockDataX += 1;
        // }

        SDL_SetRenderDrawColor(renderer, backgroundColor.r,
                                backgroundColor.g,
                                backgroundColor.b,
                                backgroundColor.a);
        SDL_RenderClear(renderer);
        
        //render        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        mainBlock.render();
        nextBlockUI.render();
        nextHint.render();
        nextBlockRenderer.render();
        getScoresText.render();
        scoreNumText.render();
        hintText.render();
        blockdata.render();

        SDL_RenderPresent(renderer);

        if(blockdata.gameoverFlag){
            RussianBlocks::gameState = RussianBlocks::GAMEOVER;
            // RussianBlocks::totalScores = blockdata.scores;
        }

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame){
            SDL_Delay(msPerFrame - deltaTime);
        }
    }

    TTF_CloseFont(nextHintFont);
    TTF_CloseFont(HintFont);
    TTF_CloseFont(scoreTextFont);
    TTF_CloseFont(getScoresTextFont);
}

void RussianBlocks::gameover(SDL_Renderer *renderer){
    
    TTF_Font *gameOverTextFont = TTF_OpenFont(fontFilename, 55);
    // TTF_SetFontStyle(gameOverTextFont, TTF_STYLE_BOLD);
    TextWrap gameOverText;
    gameOverText.createUTF8Blended(gameOverTextFont,
                            " 游 戏 结 束 ", 
                            {39, 112, 229, 255},
                            renderer);
    gameOverText.rect.x = (SCR_WIDTH - gameOverText.rect.w) / 2;
    gameOverText.rect.y = SCR_HEIGHT / 3;

    TTF_Font *scoreTextFont = TTF_OpenFont(fontFilename, 38);
    TextWrap scoreText;
    scoreText.createUTF8Blended(scoreTextFont, "总得分: " + std::to_string(blockdata.scores),
                {103, 31, 236, 255}, renderer);
    int lineSpacing = 25;
    scoreText.rect.y = gameOverText.rect.y + gameOverText.rect.h + lineSpacing;
    scoreText.rect.x = (SCR_WIDTH - scoreText.rect.w) / 2;

    TTF_Font *inputTextFont = TTF_OpenFont(fontFilename, 38);

    TextWrap hintInput;
    hintInput.createUTF8Blended(inputTextFont, "请教大侠姓名:", {0, 0, 0, 255}, renderer);
    hintInput.rect.x = (SCR_WIDTH - hintInput.rect.w) / 2;
    hintInput.rect.y = scoreText.rect.y + scoreText.rect.h + lineSpacing;

    RussianBlocks::InputText inputName{inputTextFont, {22, 154, 255, 255},
                                        renderer, 
                                        gameOverText.rect.x - 10,
                                        hintInput.rect.y + hintInput.rect.h + lineSpacing,
                                        gameOverText.rect.w + 20,
                                        500, 300, 
                                        {15, 60, 187, 255},
                                        {224, 231, 255, 255}, 8};

    pauseHintUI hint_ui{renderer, gameOverText.rect.x,
                        gameOverText.rect.y, gameOverText.rect.w,
                        inputName.rect.h + inputName.rect.y - gameOverText.rect.y + lineSpacing,
                        90, 30, 15};

    SDL_Event eve;
    Uint32 time = SDL_GetTicks(), oldTime, deltaTime, timeCount = 0;

    bool runFlag = true;
    while(runFlag){
        oldTime = time;
        time = SDL_GetTicks();
        deltaTime = time - oldTime;
        timeCount += deltaTime;
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                runFlag = false;
                RussianBlocks::gameState = RussianBlocks::EXIT;
            }
            else if(eve.type == SDL_KEYDOWN){
                runFlag = false;
            }
        }

        hint_ui.render();
        gameOverText.render();
        scoreText.render();
        SDL_RenderPresent(renderer);

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame){
            SDL_Delay(msPerFrame - deltaTime);
        }

        if(timeCount > 700)
            runFlag = false;
    }
    
    if(blockdata.scores >= heroList.list[heroList.len - 1].score){
        //input user name
        SDL_StartTextInput();

        while(RussianBlocks::gameState == RussianBlocks::GAMEOVER){
            oldTime = time;
            time = SDL_GetTicks();
            deltaTime = time - oldTime;
            while(SDL_PollEvent(&eve)){
                if(eve.type == SDL_QUIT){
                    RussianBlocks::gameState = RussianBlocks::EXIT;
                }
                else if(eve.type == SDL_KEYDOWN){
                    if(eve.key.keysym.sym == SDLK_c && SDL_GetModState()&KMOD_CTRL)
                        SDL_SetClipboardText(inputName.text.c_str());
                    else if(eve.key.keysym.sym == SDLK_v && SDL_GetModState()&KMOD_CTRL){
                        inputName.text = SDL_GetClipboardText();
                        inputName.reRenderFlag = true;  
                    }
                    else{ 
                        switch(eve.key.keysym.sym){
                            case SDLK_BACKSPACE:
                                if(inputName.text.length() > 0){
                                    inputName.text.pop_back();
                                    inputName.reRenderFlag = true;
                                }
                                break;
                            case SDLK_RETURN:
                                RussianBlocks::heroList.addRecord(inputName.text, blockdata.scores);
                                RussianBlocks::gameState = RussianBlocks::HEROESLIST;
                                break;
                        }
                    }
                }
                else if(eve.type == SDL_TEXTINPUT){
                    inputName.text += eve.text.text;
                    inputName.reRenderFlag = true;
                }
                
            }

            hint_ui.render();
            gameOverText.render();
            scoreText.render();
            hintInput.render();
            inputName.render(deltaTime);

            SDL_RenderPresent(renderer);

            deltaTime = SDL_GetTicks() - time;
            if(deltaTime < msPerFrame){
                SDL_Delay(msPerFrame - deltaTime);
            }

        }
        SDL_StopTextInput();
    }

    else{
        TextWrap continueHint;
        continueHint.createUTF8Blended(inputTextFont, "按下任意键继续...", {0, 0, 0, 255}, renderer);
        continueHint.rect.y = hintInput.rect.y;
        continueHint.rect.x = gameOverText.rect.x;

        while(RussianBlocks::gameState == RussianBlocks::GAMEOVER){
            time = SDL_GetTicks();
            while(SDL_PollEvent(&eve)){
                if(eve.type == SDL_QUIT){
                    RussianBlocks::gameState = RussianBlocks::EXIT;
                }
                else if(eve.type == SDL_KEYDOWN){
                    RussianBlocks::gameState = RussianBlocks::HEROESLIST;
                }
            }

            hint_ui.render();
            gameOverText.render();
            scoreText.render();
            continueHint.render();
            SDL_RenderPresent(renderer);

            deltaTime = SDL_GetTicks() - time;
            if(deltaTime < msPerFrame){
                SDL_Delay(msPerFrame - deltaTime);
            }

        }
    }

    

    TTF_CloseFont(gameOverTextFont);
    TTF_CloseFont(scoreTextFont);
    TTF_CloseFont(inputTextFont);
}

//return true: still pause;
//return false: continue playing game;
bool RussianBlocks::pause(SDL_Renderer *renderer){
    TTF_Font *pauseHintFont = TTF_OpenFont(fontFilename, 45);
    // TTF_SetFontStyle(pauseHintFont, TTF_STYLE_BOLD);
    TTF_Font *pauseHintFont1 = TTF_OpenFont(fontFilename, 33);
    
    int lineInterval = 18;
    TextWrap pauseHintText;
    pauseHintText.createUTF8Blended(pauseHintFont, " 暂  停 ", 
                                    {0, 0, 0, 255}, renderer);
    pauseHintText.rect.y = (SCR_HEIGHT - lineInterval) / 2 - pauseHintText.rect.h;
    pauseHintText.rect.x = SCR_WIDTH / 2 - pauseHintText.rect.w / 2;
    TextWrap pauseHintText1;
    pauseHintText1.createUTF8Blended(pauseHintFont1, "按下 空格键 继续",
                                        {0, 0, 0, 255},
                                        renderer);
    pauseHintText1.rect.y = (SCR_HEIGHT + lineInterval) / 2;
    pauseHintText1.rect.x = SCR_WIDTH / 2 - pauseHintText1.rect.w / 2;

    RussianBlocks::pauseHintUI pause_ui{renderer, pauseHintText1.rect.x,
                                        pauseHintText.rect.y, 
                                        pauseHintText1.rect.w,
                                        pauseHintText.rect.h + pauseHintText1.rect.h + lineInterval,
                                        45, 25, 10};

    TTF_CloseFont(pauseHintFont);    
    TTF_CloseFont(pauseHintFont1);

    SDL_Event eve;
    // while(RussianBlocks::gameState == RussianBlocks::PAUSE){
    while(true){
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                RussianBlocks::gameState = RussianBlocks::EXIT;
            }
            else if(eve.type == SDL_KEYDOWN){
                switch(eve.key.keysym.sym){
                    case SDLK_SPACE:
                        // RussianBlocks::gameState = RussianBlocks::PLAY;
                        return false;
                        break;
                    case SDLK_ESCAPE:
                        if(RussianBlocks::messageHint(renderer, "离开将会丢失当前游戏进度，\n是否确定？")){
                            RussianBlocks::gameState = RussianBlocks::STARTMENU;
                            blockdata.savedFlag = false;
                        }
                        return true;
                        break;
                }
            }
        }
        
        pause_ui.render();
        pauseHintText.render();
        pauseHintText1.render();
        SDL_RenderPresent(renderer);

    }

}

namespace RussianBlocks{
    void messageHint_Button0(void *a){

    }

    void messageHint_Button1(void *a){

    }
}

bool RussianBlocks::messageHint(SDL_Renderer *renderer, const std::string &hintString){
    bool result;
    TTF_Font *hintTextFont = TTF_OpenFont(fontFilename, 40); 
    std::vector<TextWrap> hintText;

    size_t oldPos = 0;
    size_t pos = hintString.find_first_of('\n');
    while(pos != std::string::npos){
        hintText.push_back(TextWrap(hintTextFont, 
                                    hintString.substr(oldPos, pos - oldPos), 
                                    {0, 0, 0, 255}, renderer));
        oldPos = pos + 1;
        pos = hintString.find_first_of('\n', oldPos);
    }
    hintText.push_back(TextWrap(hintTextFont,
                                hintString.substr(oldPos),
                                {0, 0, 0, 255},
                                renderer));
    int numOfLines = hintText.size();
    int linespacing = 10;

    //find the max text width;
    int maxWidth = 0;
    for(int i = 0; i < numOfLines; ++i){
        if(hintText[i].rect.w > maxWidth)
            maxWidth = hintText[i].rect.w;
    }
    hintText[0].rect.x = (SCR_WIDTH - maxWidth) / 2;
    hintText[0].rect.y = SCR_HEIGHT / 3 - numOfLines * (hintText[0].rect.h + linespacing) / 2;
    for(int i = 1; i < numOfLines; ++i){
        hintText[i].rect.x = hintText[0].rect.x;
        hintText[i].rect.y = hintText[i - 1].rect.y + hintText[i-1].rect.h + linespacing;
    }

    const SDL_Color ButtonColor = {195, 178, 219, 255};
    // const SDL_Color ButtonColor = {0, 0, 0, 255};
    TTF_Font *buttonTextFont = TTF_OpenFont(fontFilename, 38);
    RussianBlocks::ButtonClassicalUI buttons[2];
    buttons[0].reset(renderer, hintText[0].rect.x + 10,
                     hintText[numOfLines - 1].rect.y + hintText[0].rect.h + 3 * linespacing,
                     buttonTextFont, " 确 定 ", {0, 0, 0, 255},
                     ButtonColor);
    buttons[0].callbackFunc = messageHint_Button0;
    buttons[1].reset(renderer, hintText[1].rect.x + maxWidth / 2 + 50,
                     hintText[numOfLines - 1].rect.y + hintText[0].rect.h + 3 * linespacing,
                     buttonTextFont, " 取 消 ", {0, 0, 0, 255},
                     ButtonColor);
    buttons[1].callbackFunc = messageHint_Button1;
    unsigned int selected = 0;
    buttons[selected].isSelected = true;

    RussianBlocks::pauseHintUI hintui{renderer, hintText[0].rect.x,
                                        hintText[0].rect.y,
                                        maxWidth, 
                                        buttons[0].rect.y + buttons[0].rect.h - hintText[0].rect.y};



    TTF_CloseFont(hintTextFont);
    TTF_CloseFont(buttonTextFont);

    SDL_Event eve;
    Uint32 time = SDL_GetTicks(), oldTime, deltaTime;

    bool runFlag = true;
    while(runFlag){
        time = SDL_GetTicks();
        while(SDL_PollEvent(&eve)){
            if(eve.type == SDL_QUIT){
                runFlag = false;
                RussianBlocks::gameState = RussianBlocks::EXIT;
                return false;
            }
            else if(eve.type == SDL_KEYDOWN){
                switch(eve.key.keysym.sym){
                    case SDLK_ESCAPE:
                        return false;
                        break;
                    case SDLK_RETURN:
                    case SDLK_SPACE:
                        if(selected == 0)
                            return true;
                        else return false;
                        break;
                    case SDLK_LEFT:
                        buttons[selected].isSelected = false;
                        if(selected == 0){
                            selected = 1;
                        }
                        else --selected;
                        buttons[selected].isSelected = true;
                        break;
                    case SDLK_RIGHT:
                        buttons[selected].isSelected = false;
                        if(selected == 1){
                            selected = 0;
                        }
                        else ++selected;
                        buttons[selected].isSelected = true;
                        break;
                }
            }
        }

        hintui.render();
        for(int i = 0; i < numOfLines; ++i){
            hintText[i].render();
        }
        buttons[0].render();
        buttons[1].render();
        SDL_RenderPresent(renderer);

        deltaTime = SDL_GetTicks() - time;
        if(deltaTime < msPerFrame){
            SDL_Delay(msPerFrame - deltaTime);
        }
    }
}