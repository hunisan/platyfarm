#pragma once
#include "gamestate.h"
#include "images.h"
#include "objects.h"

class Menu : public GameState
{
private:
    int currentFrame = 0;
    Cursor *cursor;
    vector<Button> buttons;

public:

    Menu()
    {

    }

    void Init()
    {
        cursor = new Cursor();

        fr(0,8)
            gif[i] = loadImage("gif/Layer " + to_string(i+1) + ".png");

        Button buttonNew = Button(images[ids["gui"]],SCREEN_WIDTH/2-75,SCREEN_HEIGHT/3,150,50);
        Button buttonLoad = Button(images[ids["gui"]],SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2,150,50);
        Button buttonExit = Button(images[ids["gui"]],SCREEN_WIDTH/2-50,SCREEN_HEIGHT*2/3,100,50);
        Button buttonMute = Button(images[ids["gui"]],0,0,120,50);
        Button buttonEditor = Button(images[ids["gui"]],0,SCREEN_HEIGHT-50,120,50);

        buttonNew.name = "New Game";
        buttonLoad.name = "Load Game";
        buttonExit.name = "Exit";
        buttonMute.name = "Mute";
        buttonEditor.name = "Editor";

        buttons.push_back(buttonNew);
        if(SaveFileExists())
            buttons.push_back(buttonLoad);
        buttons.push_back(buttonExit);
        buttons.push_back(buttonMute);
        buttons.push_back(buttonEditor);
    }
    int Update()
    {
        if(KeyData.Click)
        {
            for(auto& e : buttons)
                if(Contains(&e,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    if(e.name == "New Game")
                        return CREATE;
                    else if(e.name == "Load Game")
                        return LOAD;
                    else if(e.name == "Exit")
                        return EXIT;
                    else if(e.name == "Editor")
                    {
                        return EDITOR;
                    }
                    else if(e.name == "Mute")
                    {
                        e.name =  "Unmute";
                        audioCommands.push("mute");
                    }
                    else if(e.name == "Unmute")
                    {
                        e.name =  "Mute";
                        audioCommands.push("mute");
                    }
                }
        }
        //else if(KeyData.MouseMove)
        {
            for(auto& e : buttons)
                if(e.ContainsPoint(KeyData.MouseX,KeyData.MouseY))
                    e.Hover(KeyData.MouseX,KeyData.MouseY);
        }
        cursor->Update();
        return NOTHING;
    }
    void Draw()
    {
        DrawImage(gif[currentFrame/10],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        currentFrame++;
        if(currentFrame > 79)
            currentFrame = 0;

        for(auto& e : buttons)
        {
            e.Draw();
            DrawStringCentered(e.name,e.x+e.w/2,e.y+e.h/2-TILESIZE/2);
        }
        cursor->Draw();
    }
};
