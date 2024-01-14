#include "window.h"
#include "button.h"
#include <iostream>

#define SCREEN_DIVIDER 3

std::string loc_parcare(int i, int j)
{
	std::string loc;
	if (i == 0)
		loc = "A";
	else if (i == 1)
		loc = "B";
	else if (i == 2)
		loc = "C";
	else if (i == 3)
		loc = "D";
	else if (i == 4)
		loc = "E";
	else if (i == 5)
		loc = "F";
	loc += std::to_string(j + 1);
	return loc;
}

Window::Window()
{
    windowHeight = sf::VideoMode::getDesktopMode().height;
    windowWidth = sf::VideoMode::getDesktopMode().width;
    window.create(sf::VideoMode(windowWidth / SCREEN_DIVIDER * 2, windowHeight / SCREEN_DIVIDER * 2), "Smart Parking Assistant");
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    centerScreen.x = window.getPosition().x;
    centerScreen.y = window.getPosition().y;
    init();
}

void Window::loadFont()
{
    if (!font.loadFromFile("Arialn.ttf"))
    {
        std::cout << "Couldn't load font. You must've install the above font in the same folder as the project to continue. Returning -1.";
        exit(0);
    }
}

void Window::init()
{
    loadFont();
}

void Window::run(bool **parcare, int socket_desc,int* slot_i,int* slot_j)
{
    int i, j;
    
    Button myButton = Button(window.getSize().x*0.20, window.getSize().y*0.76, 120, 60, font, "Vreau loc", sf::Color::Red);
    while (window.isOpen())
    {
        window.clear(sf::Color(80, 80, 80));
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                exit(0);
            }
            auto mouseCoords = window.mapPixelToCoords(myMouse.getPosition(window));
            myButton.update(mouseCoords,socket_desc);
            // std::cout << mouseCoords.x << " " << mouseCoords.y << std::endl;
        }
        sf::RectangleShape rectangle(sf::Vector2f(window.getSize().x, window.getSize().y * 3 / 4 - 3));
        rectangle.setOutlineThickness(-10);
        rectangle.setOutlineColor(sf::Color::Black);
        rectangle.setFillColor(sf::Color::Transparent);
        window.draw(rectangle);

        sf::RectangleShape entrance(sf::Vector2f(window.getSize().x * 0.1, 10));
        entrance.setPosition(sf::Vector2f(window.getSize().x * 0.45, window.getSize().y * 3 / 4 - 13));
        entrance.setFillColor(sf::Color(80, 80, 80));
        window.draw(entrance);

        int parkingWidth = 35;
        int parkingHeight = 47;
        int freeSlots = 0;
        char rowLetters[]={'A','B','C','D','E','F'};

        for (int k = 0; k < 6; k++)
        {
            int startX = (k < 3) ? 91 : window.getSize().x * 0.55;
            int startY = (k % 3) * parkingHeight * 4 + 10;
            for (int l = 0; l < 24; l++)
            {
                
                
                int i = l / 12;
                int j = l % 12;

                sf::RectangleShape parkingDelimiter(sf::Vector2f(parkingWidth / 7, parkingHeight));
                parkingDelimiter.setPosition(startX + j * (parkingWidth + parkingWidth / 7), startY + i * parkingHeight * 2);
                window.draw(parkingDelimiter);

                sf::RectangleShape parkingSlot(sf::Vector2f(parkingWidth, parkingHeight));
                parkingSlot.setPosition(startX + parkingWidth / 7 + j * (parkingWidth + parkingWidth / 7), startY + i * parkingHeight * 2);

                if (!parcare[k][l])
                    freeSlots++;
                parkingSlot.setFillColor(parcare[k][l] ? sf::Color::Red : sf::Color::Green);
                if(parcare[k][l]&&*slot_i==k&&*slot_j==l)
                {
                    parkingSlot.setFillColor(sf::Color::Blue);
                }
                window.draw(parkingSlot);

                sf::Text slotNumber;
                slotNumber.setFont(font);
                slotNumber.setCharacterSize(50);
                slotNumber.setScale(sf::Vector2f(.5, .5));
                slotNumber.setString(std::to_string(l + 1));
                slotNumber.setFillColor(sf::Color::Black);
                slotNumber.setPosition(parkingSlot.getPosition().x + parkingWidth / 2 - slotNumber.getGlobalBounds().width / 2, parkingSlot.getPosition().y + parkingHeight / 2 - slotNumber.getGlobalBounds().height / 2);
                window.draw(slotNumber);

                if (j == 11)
                {
                    sf::RectangleShape parkingDelimiter(sf::Vector2f(parkingWidth / 7, parkingHeight));
                    parkingDelimiter.setPosition(startX + (j + 1) * (parkingWidth + parkingWidth / 7), startY + i * parkingHeight * 2);
                    window.draw(parkingDelimiter);
                }
            }
            if (k % 3 != 2)
            {
                sf::RectangleShape greenSpace(sf::Vector2f(parkingWidth / 7 * 97, parkingHeight));
                greenSpace.setPosition(startX, startY + parkingHeight * 3);
                greenSpace.setFillColor(sf::Color(1, 50, 32));
                window.draw(greenSpace);
            }
            sf::Text rowLetter;
            rowLetter.setFont(font);
            rowLetter.setCharacterSize(50);
            rowLetter.setScale(sf::Vector2f(.5, .5));
            rowLetter.setString(rowLetters[k]);
            rowLetter.setFillColor(sf::Color::White);
            rowLetter.setPosition(startX+window.getSize().x*0.18, startY + parkingHeight*1.2);
            window.draw(rowLetter);
        }

        sf::Text freeSlotsText;
        freeSlotsText.setFont(font);
        freeSlotsText.setCharacterSize(50);
        freeSlotsText.setScale(sf::Vector2f(.5, .5));
        freeSlotsText.setString("Locuri disponibile: " + std::to_string(freeSlots));
        freeSlotsText.setFillColor(sf::Color::White);
        freeSlotsText.setPosition(window.getSize().x*0.75, window.getSize().y*0.75);
        window.draw(freeSlotsText);

        sf::Text parkingStatus;
        parkingStatus.setFont(font);
        parkingStatus.setCharacterSize(50);
        parkingStatus.setScale(sf::Vector2f(.5, .5));
        if(*slot_i!=-1&&*slot_j!=-1)
        {
            parkingStatus.setString("Locul tau este: "+loc_parcare(*slot_i,*slot_j));
        }
        else
        {
            parkingStatus.setString("Nu ai loc");
        }
        parkingStatus.setFillColor(sf::Color::White);
        parkingStatus.setPosition(window.getSize().x*0.2, window.getSize().y*0.75+100);
        window.draw(parkingStatus);

        myButton.render(window);
        window.display();
    }
}