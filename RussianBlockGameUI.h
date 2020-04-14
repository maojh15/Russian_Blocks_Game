#ifndef _RUSSIANBLOCKGAMEUI_H_
#define _RUSSIANBLOCKGAMEUI_H_
#include<mySDL2Tools/MySDLStruct.h>
#include<SDL2/SDL_ttf.h>
#include<cmath>

namespace RussianBlocks{
    //class of Button
    class Button{
    public:
        Button(int verticalSideBlank_ = 16, int horizonSideBlank_ = 16, float circFrequencyMS_ = 0.003142):isSelected{false},\
            verticalSideBlank{verticalSideBlank_},horizonSideBlank{horizonSideBlank_},circFrequencyMS{circFrequencyMS_}{
            buttonRect.x = 0;
            buttonRect.y = 0;
        }

        //set the Text shown on Button.
        void setText(TTF_Font *font, const char* text_, 
                        SDL_Color color, SDL_Color selectedColor_1, 
                        SDL_Color selectedColor_2,   
                        SDL_Renderer *renderer){
            text.createUTF8Blended(font, text_, color, renderer);
            // selectedText.createUTF8Shaded(font, text_, color, selectedColor_, renderer);
            selectedColor1 = selectedColor_1;
            selectedColor2 = selectedColor_2;
            windowRenderer = renderer;
            buttonRect.h = text.rect.h + 2 * verticalSideBlank;
            buttonRect.w = text.rect.w + 2 * horizonSideBlank;
            
            flashColorK = {.r = (selectedColor2.r - selectedColor1.r) / 2,
                           .g = (selectedColor2.g - selectedColor1.g) / 2,
                           .b = (selectedColor2.b - selectedColor1.b) / 2,
                           .a = (selectedColor2.a - selectedColor1.a) / 2};
            flashColorB = {.r = (selectedColor2.r + selectedColor1.r) / 2,
                           .g = (selectedColor2.g + selectedColor1.g) / 2,
                           .b = (selectedColor2.b + selectedColor1.b) / 2,
                           .a = (selectedColor2.a + selectedColor1.a) / 2};
        }

        //set the position of Button. (parameters are screen coordinate of this Button.)
        void setTopLeftPosition(int posX, int posY){
            buttonRect.x = posX;
            buttonRect.y = posY;
            text.rect.x = posX + horizonSideBlank;
            text.rect.y = posY + verticalSideBlank;
            setDrawRects();
        }
        
        //render the button.
        void render(SDL_Color backgroundColor){
            if(isSelected){
                float colorT = std::sin(SDL_GetTicks() * circFrequencyMS);
                flashColor = {.r = Uint8(flashColorK.r * colorT + flashColorB.r),
                              .g = Uint8(flashColorK.g * colorT + flashColorB.g),
                              .b = Uint8(flashColorK.b * colorT + flashColorB.b),
                              .a = Uint8(flashColorK.a * colorT + flashColorB.a)};
                SDL_SetRenderDrawColor(windowRenderer, flashColor.r,
                                    flashColor.g, flashColor.b, flashColor.a);
            }
            else{
                SDL_SetRenderDrawColor(windowRenderer, backgroundColor.r,
                                        backgroundColor.g, backgroundColor.b,
                                        backgroundColor.a);
            }
            SDL_RenderFillRects(windowRenderer, drawRects, 9);
            SDL_SetRenderDrawColor(windowRenderer, 0, 0, 0, 255);
            SDL_RenderDrawLines(windowRenderer, drawLinePoints, 29);
            text.render(text.rect.x, text.rect.y);
        }

        TextWrap text;
        // TextWrap selectedText;
        bool isSelected; //whether the button is selected. true: selected.
        SDL_Rect buttonRect;
        SDL_Color selectedColor1, selectedColor2;// When the button is selected, the color of button will change between these two colors.
        int verticalSideBlank, horizonSideBlank;
        SDL_Renderer *windowRenderer;
        void (*bindFunction)(); //the function to be called when the button is pressed.
        float circFrequencyMS; //The circle frequency of flash when button is selected.
    
    private:
        //Data of shape of button.
        void setDrawRects(){
            int squareLenH = static_cast<int>(3.0/4.0 * verticalSideBlank);
            int squareLenW = static_cast<int>(3.0/4.0 * horizonSideBlank);
            drawRects[0] = {.x = buttonRect.x, .y = buttonRect.y, 
                            .w = squareLenW, .h = squareLenH};
            drawRects[1] = {.x = buttonRect.x, .y = buttonRect.y + verticalSideBlank, 
                            .w = squareLenW, .h = text.rect.h};
            drawRects[2] = {.x = buttonRect.x, .y = buttonRect.y + buttonRect.h - squareLenH,
                            .w = squareLenW, .h = squareLenH};
            drawRects[3] = {.x = buttonRect.x + squareLenW, .y = buttonRect.y + squareLenH, 
                            .w = horizonSideBlank - squareLenW, 
                            .h = buttonRect.h - 2 * squareLenH};
            drawRects[4] = {.x = text.rect.x, .y = buttonRect.y, 
                            .w = text.rect.w, .h = buttonRect.h};
            drawRects[5] = {.x = buttonRect.x + horizonSideBlank + text.rect.w, 
                            .y = buttonRect.y + squareLenH, 
                            .w = horizonSideBlank - squareLenW,
                            .h = buttonRect.h - 2 * squareLenH};
            drawRects[6] = {.x = buttonRect.x + buttonRect.w - squareLenW, 
                            .y = buttonRect.y, 
                            .w = squareLenW, .h = squareLenH};
            drawRects[7] = {.x = buttonRect.x + buttonRect.w - squareLenW,
                            .y = buttonRect.y + verticalSideBlank, 
                            .w = squareLenW, .h = text.rect.h};
            drawRects[8] = {.x = buttonRect.x + buttonRect.w - squareLenW, 
                            .y = buttonRect.y + buttonRect.h - squareLenH, 
                            .w = squareLenW, .h = squareLenH};

            //Line Points
            drawLinePoints[0] = {.x = buttonRect.x, .y = buttonRect.y};
            drawLinePoints[1] = {.x = buttonRect.x + squareLenW, .y = buttonRect.y};
            drawLinePoints[2] = {.x = buttonRect.x + squareLenW, 
                                 .y = buttonRect.y + verticalSideBlank};
            drawLinePoints[3] = {.x = buttonRect.x, .y = buttonRect.y + verticalSideBlank};
            drawLinePoints[4] = {.x = buttonRect.x, 
                                 .y = buttonRect.y + verticalSideBlank + text.rect.h};
            drawLinePoints[5] = {.x = buttonRect.x + squareLenW, .y = drawLinePoints[4].y};
            drawLinePoints[6] = {.x = buttonRect.x + squareLenW, .y = buttonRect.y + buttonRect.h};
            drawLinePoints[7] = {.x = buttonRect.x, .y = drawLinePoints[6].y};
            drawLinePoints[8] = {.x = buttonRect.x, .y = drawLinePoints[7].y - squareLenH};
            drawLinePoints[9] = {.x = buttonRect.x + horizonSideBlank, 
                                 .y = drawLinePoints[8].y};
            drawLinePoints[10] = {.x = drawLinePoints[9].x, .y = buttonRect.y + buttonRect.h};
            drawLinePoints[11] = {.x = buttonRect.x + horizonSideBlank + text.rect.w, 
                                  .y = drawLinePoints[10].y};
            drawLinePoints[12] = {.x = drawLinePoints[11].x, .y = drawLinePoints[9].y};
            drawLinePoints[13] = {.x = buttonRect.x + buttonRect.w, .y = drawLinePoints[12].y};
            drawLinePoints[14] = {.x = drawLinePoints[13].x, .y = drawLinePoints[11].y};
            drawLinePoints[15] = {.x = drawLinePoints[14].x - squareLenW, .y = drawLinePoints[14].y};
            drawLinePoints[16] = {.x = drawLinePoints[15].x, .y = drawLinePoints[5].y};
            drawLinePoints[17] = {.x = drawLinePoints[13].x, .y = drawLinePoints[16].y};
            drawLinePoints[18] = {.x = drawLinePoints[17].x, .y = drawLinePoints[2].y};
            drawLinePoints[19] = {.x = drawLinePoints[16].x, .y = drawLinePoints[2].y};                   
            drawLinePoints[20] = {.x = drawLinePoints[19].x, .y = drawLinePoints[0].y};
            drawLinePoints[21] = {.x = drawLinePoints[14].x, .y = drawLinePoints[0].y};
            drawLinePoints[22] = {.x = drawLinePoints[21].x, .y = drawLinePoints[21].y + squareLenH}; 
            drawLinePoints[23] = {.x = drawLinePoints[11].x, .y = drawLinePoints[22].y};
            drawLinePoints[24] = {.x = drawLinePoints[23].x, .y = drawLinePoints[0].y};
            drawLinePoints[25] = {.x = drawLinePoints[9].x, .y = drawLinePoints[0].y};
            drawLinePoints[26] = {.x = drawLinePoints[25].x, .y = drawLinePoints[22].y};                       
            drawLinePoints[27] = {.x = buttonRect.x, .y = drawLinePoints[26].y};
            drawLinePoints[28] = {.x = buttonRect.x, .y = buttonRect.y};
        }
        SDL_Rect drawRects[9];
        SDL_Point drawLinePoints[29];
        SDL_Color flashColor; //K sin(t) + B;
        SDL_Color flashColorK, flashColorB;
    };

    class doubleSquareUI{
    public:
        //Notice: the renderer color must be setted before render this UI.
        doubleSquareUI(SDL_Renderer *renderer_, int LeftTopX_, int LeftTopY_,
                        int RightDownX_, int RightDownY_, 
                        int widthOfOutterLine_ = 3,
                        int spacingBetweenOutterLineAndInnerLine = 2):renderer{renderer_}{
                rects[0] = {.x = LeftTopX_, .y = LeftTopY_,
                            .w = RightDownX_ - LeftTopX_,
                            .h = widthOfOutterLine_};
                rects[1] = {.x = LeftTopX_, .y = RightDownY_ - widthOfOutterLine_,
                            .w = rects[0].w,
                            .h = widthOfOutterLine_};
                rects[2] = {.x = LeftTopX_, .y = LeftTopY_,
                            .w = widthOfOutterLine_,
                            .h = rects[1].y - LeftTopY_};
                rects[3] = {.x = RightDownX_ - widthOfOutterLine_, 
                            .y = LeftTopY_,
                            .w = widthOfOutterLine_,
                            .h = rects[2].h};
                points[0] = {.x = LeftTopX_ + widthOfOutterLine_ + spacingBetweenOutterLineAndInnerLine,
                            .y = LeftTopY_ + widthOfOutterLine_ + spacingBetweenOutterLineAndInnerLine};
                points[1] = {.x = RightDownX_ - widthOfOutterLine_ - spacingBetweenOutterLineAndInnerLine,
                            .y = points[0].y};
                points[3] = {.x = points[0].x,
                            .y = RightDownY_ - widthOfOutterLine_ - spacingBetweenOutterLineAndInnerLine};
                points[2] = {.x = points[1].x,
                            .y = points[3].y};
                points[4] = {.x = points[0].x, .y = points[0].y};
        }

        void reDraw(int LeftTopX_, int LeftTopY_,
                        int RightDownX_, int RightDownY_, 
                        int widthOfOutterLine_ = 3,
                        int spacingBetweenOutterLineAndInnerLine = 2){
            rects[0] = {.x = LeftTopX_, .y = LeftTopY_,
                            .w = RightDownX_ - LeftTopX_,
                            .h = widthOfOutterLine_};
            rects[1] = {.x = LeftTopX_, .y = RightDownY_ - widthOfOutterLine_,
                        .w = rects[0].w,
                        .h = widthOfOutterLine_};
            rects[2] = {.x = LeftTopX_, .y = LeftTopY_,
                        .w = widthOfOutterLine_,
                        .h = rects[1].y - LeftTopY_};
            rects[3] = {.x = RightDownX_ - widthOfOutterLine_, 
                        .y = LeftTopY_,
                        .w = widthOfOutterLine_,
                        .h = rects[2].h};
            points[0] = {.x = LeftTopX_ + widthOfOutterLine_ + spacingBetweenOutterLineAndInnerLine,
                        .y = LeftTopY_ + widthOfOutterLine_ + spacingBetweenOutterLineAndInnerLine};
            points[1] = {.x = RightDownX_ - widthOfOutterLine_ - spacingBetweenOutterLineAndInnerLine,
                        .y = points[0].y};
            points[3] = {.x = points[0].x,
                        .y = RightDownY_ - widthOfOutterLine_ - spacingBetweenOutterLineAndInnerLine};
            points[2] = {.x = points[1].x,
                        .y = points[3].y};
            points[4] = {.x = points[0].x, .y = points[0].y};
        }

        void render(){
            for(int i = 0; i < 4; ++i)
                SDL_RenderFillRect(renderer, &rects[i]);
            SDL_RenderDrawLines(renderer, points, 5);
        }
        //Four points of inner square lines
        SDL_Point points[5];
        //Four rects rep the outter square lines
        SDL_Rect rects[4];
        SDL_Renderer *renderer;
    };

    class ButtonClassicalUI{
    public:
        ButtonClassicalUI():squareUI{nullptr, 0, 0, 0, 0}{}
        ButtonClassicalUI(SDL_Renderer *renderer_, int posX_,
                            int posY_,
                            TTF_Font *font, 
                            const std::string &text_, SDL_Color textColor,
                            SDL_Color buttonColor_, 
                            int spacingX_ = 20, int spacingY_ = 10, 
                            bool isSelected_ = false):
                            renderer{renderer_},
                            spacingX{spacingX_}, spacingY{spacingY_}, 
                            isSelected{isSelected_}, buttonColor{buttonColor_},
                            squareUI{renderer_, 0, 0, 0, 0}{
            texture.createUTF8Blended(font, text_, textColor, renderer);
            rect = {.x = posX_, .y = posY_, .w = texture.rect.w + 2 * spacingX,
                                .h = texture.rect.h + 2 * spacingY};
            texture.rect.x = posX_ + spacingX;
            texture.rect.y = posY_ + spacingY;
            squareUI.reDraw(posX_, posY_, posX_ + rect.w, posY_ + rect.h);
            // selectedRect = {.x = posX_ + spacingX / 4, .y = posY_ + spacingY / 4,
            //                 .w = rect.w - spacingX / 2, .h = rect.h - spacingY / 2};
            selectedRect = {.x = posX_ + spacingX_ / 2, 
                            .y = texture.rect.y + texture.rect.h + spacingY,
                            .w = texture.rect.w,
                            .h = 3};
        }

        void reset(SDL_Renderer *renderer_, int posX_,
                            int posY_,
                            TTF_Font *font, 
                            const std::string &text_, SDL_Color textColor,
                            SDL_Color buttonColor_, 
                            int spacingX_ = 20, int spacingY_ = 10, 
                            bool isSelected_ = false){
            renderer = renderer_;
            spacingX = spacingX_; 
            spacingY = spacingY_; 
            isSelected = isSelected_;
            buttonColor = buttonColor_;
            squareUI.renderer = renderer_;
            texture.createUTF8Blended(font, text_, textColor, renderer);
            rect = {.x = posX_, .y = posY_, .w = texture.rect.w + 2 * spacingX,
                                .h = texture.rect.h + 2 * spacingY};
            texture.rect.x = posX_ + spacingX;
            texture.rect.y = posY_ + spacingY;
            squareUI.reDraw(posX_, posY_, posX_ + rect.w, posY_ + rect.h);
            // selectedRect = {.x = posX_ + spacingX / 2, .y = posY_ + spacingY,
            //                 .w = rect.w - spacingX, .h = rect.h - spacingY * 2};
            selectedRect = {.x = texture.rect.x - 3, 
                            .y = texture.rect.y + texture.rect.h + 2,
                            .w = texture.rect.w + 6,
                            .h = 3};
        }

        void render(){
            SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g,
                                            buttonColor.b, buttonColor.a);
            squareUI.render();
            SDL_SetRenderDrawColor(renderer, 100, 50, 50, 255);
            if(isSelected)
                SDL_RenderFillRect(renderer, &selectedRect);
            texture.render();
        }

        bool isSelected;
        int spacingX, spacingY;
        SDL_Rect rect, selectedRect;
        TextWrap texture;
        SDL_Renderer *renderer;
        SDL_Color buttonColor;
        doubleSquareUI squareUI;
        void (*callbackFunc)(void *);
    };
}


//special UI, used only once
namespace RussianBlocks{
    class HintTextWrapInPlay{
    public:
        HintTextWrapInPlay(SDL_Renderer *renderer_, TTF_Font *HintFont_, 
                            int startPosX_, int startPosY_,
                            SDL_Color keyInHintTextColor_ = {244, 49, 32, 255}):renderer{renderer_},
                            HintFont{HintFont_},
                            keyInHintTextColor{keyInHintTextColor_},
                            totalHintText{sizeof(text)/sizeof(TextWrap)}{
            setHintText(startPosX_, startPosY_);
        }

        void setHintText(int startPosX, int startPosY){
            text[0].createUTF8Blended(HintFont, 
                                "提示: 按下",
                                {0, 0, 0, 255}, renderer);
            text[0].rect.x = startPosX;
            text[0].rect.y = startPosY + 6;

            text[1].createUTF8Blended(HintFont, "esc", keyInHintTextColor,
                                        renderer);
            text[1].rect.x = text[0].rect.x + text[0].rect.w;
            text[1].rect.y = text[0].rect.y;

            text[2].createUTF8Blended(HintFont, "返回主菜单，按下", 
                                        {0, 0, 0, 255},
                                        renderer);
            text[2].rect.x = text[1].rect.x + text[1].rect.w;
            text[2].rect.y = text[0].rect.y;

            text[3].createUTF8Blended(HintFont, "空格键", 
                                        keyInHintTextColor,
                                        renderer);
            text[3].rect.x = text[2].rect.x + text[2].rect.w;
            text[3].rect.y = text[0].rect.y;

            text[4].createUTF8Blended(HintFont, "暂停", 
                                        {0, 0, 0, 255},
                                        renderer);
            text[4].rect.x = text[3].rect.x + text[3].rect.w;
            text[4].rect.y = text[0].rect.y;

            text[5].createUTF8Blended(HintFont,
                                        "左右下键",
                                        keyInHintTextColor, renderer);
            text[5].rect.x = startPosX;
            text[5].rect.y = text[0].rect.y + text[0].rect.h + 2;

            text[6].createUTF8Blended(HintFont,
                                        "移动，",
                                        {0, 0, 0, 255}, renderer);
            text[6].rect.x = text[5].rect.x + text[5].rect.w;
            text[6].rect.y = text[5].rect.y;

            text[7].createUTF8Blended(HintFont,
                                        "上",
                                        keyInHintTextColor, renderer);
            text[7].rect.x = text[6].rect.x + text[6].rect.w;
            text[7].rect.y = text[5].rect.y;

            text[8].createUTF8Blended(HintFont,
                                        "或",
                                        {0, 0, 0, 255}, renderer);
            text[8].rect.x = text[7].rect.x + text[7].rect.w;
            text[8].rect.y = text[0].rect.y + text[0].rect.h + 2;

            text[9].createUTF8Blended(HintFont,
                                        "R",
                                        keyInHintTextColor, renderer);
            text[9].rect.x = text[8].rect.x + text[8].rect.w;
            text[9].rect.y = text[0].rect.y + text[0].rect.h + 2;

            text[10].createUTF8Blended(HintFont,
                                        "键旋转方块, ",
                                        {0, 0, 0, 255}, renderer);
            text[10].rect.x = text[9].rect.x + text[9].rect.w;
            text[10].rect.y = text[0].rect.y + text[0].rect.h + 2;

            text[11].createUTF8Blended(HintFont,
                                        "F",
                                        keyInHintTextColor, renderer);
            text[11].rect.x = text[10].rect.x + text[10].rect.w;
            text[11].rect.y = text[0].rect.y + text[0].rect.h + 2;

            text[12].createUTF8Blended(HintFont,
                                        "键加速下落",
                                        {0, 0, 0, 255}, renderer);
            text[12].rect.x = text[11].rect.x + text[11].rect.w;
            text[12].rect.y = text[0].rect.y + text[0].rect.h + 2;
        }
        void render(){
            for(int i = 0; i < totalHintText; ++i){
                text[i].render();
            }
        }

        const int totalHintText;
        SDL_Color keyInHintTextColor;
        SDL_Renderer *renderer;
        TTF_Font *HintFont;
        TextWrap text[13];
        
    };

    class pauseHintUI{
    public:
        pauseHintUI(SDL_Renderer *renderer_, int innerPosX_,
                    int innerPosY_, int textW_, int textH_,
                    int spacingX_ = 40, int spacingY_ = 25, int distance_ = 10):renderer{renderer_},
                            innerPosX{innerPosX_}, innerPosY{innerPosY_},
                            textW{textW_}, textH{textH_}, spacingX{spacingX_},
                            spacingY{spacingY_}, distance{distance_}{
            rect1.x = innerPosX - spacingX_;
            rect1.y = innerPosY - spacingY_;
            rect1.w = 2 * spacingX + textW;
            rect1.h = 2 * spacingY + textH;

            rect2.x = rect1.x + distance;
            rect2.y = rect1.y + distance;
            rect2.w = rect1.w;
            rect2.h = rect1.h;
        }
        

        void render(){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect2);
            
            SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
            SDL_RenderFillRect(renderer, &rect1);
            
        }

        int innerPosX, innerPosY;
        int textW, textH;
        int spacingX, spacingY;
        int distance;
        SDL_Renderer *renderer;

    private:
        SDL_Rect rect1, rect2;
    };

    class InputText: public TextWrap{
    public:
        InputText(TTF_Font *font_, SDL_Color color, SDL_Renderer *renderer,
                    int posX, int posY, int width,
                    Uint32 timeShown_, Uint32 timeHidden_,
                    SDL_Color colorShown_, SDL_Color colorHidden_,
                    int spacing_ = 5)noexcept:
                    cursor{timeShown_, timeHidden_, colorShown_, colorHidden_},
                    spacing{spacing_},
                    square_ui{renderer, 0, 0,
                            0, 0}{
            createUTF8Blended(font_, " ", color, renderer);
            text = "";
            rect.x = posX;
            rect.y = posY;
            cursor.rect = {.x = posX, .y = posY, .w = rect.w, .h = rect.h};
            square_ui.reDraw(posX - spacing, posY - spacing,
                            posX + spacing + width, posY + spacing + rect.h);
            maxTextLength = width / rect.w - 1;
        }

        void render(Uint32 deltaTime){
            SDL_SetRenderDrawColor(rendererOfText, 0, 0, 0, 255);
            square_ui.render();
            if(reRenderFlag){
                reRenderFlag = false;
                if(text.length() <= maxTextLength){
                    if(text.length() > 0){
                        resetTextAndColorUTF8Blended(text, textColor);
                        cursor.rect.x = rect.x + rect.w;
                    }
                    else{
                        resetTextAndColorUTF8Blended(" ", textColor);
                        cursor.rect.x = rect.x;
                        text = "";
                    }
                }
                else{
                    text = text.substr(0, maxTextLength);
                }
            }
            TextWrap::render();
            cursor.render(rendererOfText, deltaTime);
        }


        bool reRenderFlag = false;
        doubleSquareUI square_ui;
        int spacing;
    private:
        struct InputCursor{
            InputCursor(Uint32 timeShown_, Uint32 timeHidden_,
                        SDL_Color colorShown_, SDL_Color colorHidden_)noexcept:
                            timeShown{timeShown_}, timeHidden{timeHidden_},
                            colorShown{colorShown_}, colorHidden{colorHidden_}{
                totTime = timeShown + timeHidden;
                timeCount = 0;
            }

            void render(SDL_Renderer *renderer, Uint32 deltaTime){
                timeCount = (timeCount + deltaTime) % totTime;
                if(timeCount < timeShown){
                    SDL_SetRenderDrawColor(renderer, colorShown.r,
                                            colorShown.g, colorShown.b,
                                            colorShown.a);
                }
                else
                    SDL_SetRenderDrawColor(renderer, colorHidden.r,
                                            colorHidden.g, colorHidden.b,
                                            colorHidden.a);
                SDL_RenderFillRect(renderer, &rect);

            }
            SDL_Rect rect;
            SDL_Color colorShown, colorHidden;
            Uint32 timeShown, timeHidden;
        private:
            Uint32 timeCount, totTime;
        };

        InputCursor cursor;
        int maxTextLength;

    };
}

#endif