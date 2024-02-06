#pragma once
#include <SFML/Graphics.hpp>
#include "button.h"
#include <atomic>

class Window
{
    sf::Font font;
    sf::Vector2f centerScreen;
    int windowHeight, windowWidth;
    sf::RenderWindow window;
    sf::Mouse myMouse;
    public:
    Window();
    void loadFont();
    void init();
    void run(bool** parcare, int socket_desc,int* slot_i,int* slot_j,std::atomic<bool>* isRunning);
};