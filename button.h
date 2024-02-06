#pragma once
#include <SFML/Graphics.hpp>

class Button
{
    sf::Font font;
    sf::Text text;
    sf::RectangleShape shape;
    bool isPressed;
    float cooldown;
    sf::Clock clock;
    bool initialPress;
    public:
    Button();
    Button(float x, float y,float width,float height, sf::Font font, std::string text, sf::Color baseColor);
    void update(const sf::Vector2f mousePos,int socket_desc);
    void render(sf::RenderTarget& target);
};