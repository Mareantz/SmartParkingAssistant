#include "button.h"
#include <cstring>
#include <unistd.h>
#include <iostream>

Button::Button()
{
}

Button::Button(float x, float y, float width, float height, sf::Font font, std::string text, sf::Color baseColor)
{
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2);
    shape.setFillColor(baseColor);
    this->font = font;
    this->text.setFont(this->font);
    this->text.setString(text);
    this->text.setFillColor(sf::Color::Black);
    this->text.setCharacterSize(40);
    this->text.setScale(sf::Vector2f(0.5, 0.5));
    sf::FloatRect textRect = this->text.getLocalBounds();
    this->text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    this->text.setPosition(sf::Vector2f(x + width / 2.0f, y + height / 2.0f));
    this->cooldown = 10.0f;
    isPressed = false;
}

void Button::update(const sf::Vector2f mousePos, int socket_desc)
{
    char message[256];
    message[0] = '\0';
    if (shape.getGlobalBounds().contains(mousePos))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)&&clock.getElapsedTime().asSeconds()>cooldown)
        {
            clock.restart();
            bzero(message, 256);
            isPressed = !isPressed;
            if (isPressed)
            {
                text.setString("Elibereaza loc");
                shape.setFillColor(sf::Color::Green);
                sf::FloatRect textRect = this->text.getLocalBounds();
                this->text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                this->text.setPosition(sf::Vector2f(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y / 2.0f));
                strcpy(message, "vreau");
            }
            else
            {
                text.setString("Vreau loc");
                shape.setFillColor(sf::Color::Red);
                sf::FloatRect textRect = this->text.getLocalBounds();
                this->text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                this->text.setPosition(sf::Vector2f(shape.getPosition().x + shape.getSize().x / 2.0f, shape.getPosition().y + shape.getSize().y / 2.0f));
                strcpy(message, "eliberez");
            }
            printf("%s\n", message);
            if (write(socket_desc, message, strlen(message)) < 0)
                perror("[client]Eroare la write() spre server.\n");
            else
                printf("[client]Mesajul a fost trasmis cu succes.\n");
        }
    }
}

void Button::render(sf::RenderTarget &target)
{
    target.draw(shape);
    target.draw(text);
}