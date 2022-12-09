// ZombieArena.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "player.h"
#include "zombieArena.h"
#include "textureHolder.h"
#include "bullet.h"

using namespace sf;

int main()
{
    TextureHolder holder;

    enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };

    State state = State::GAME_OVER;

    Vector2f resolution;
    resolution.x = VideoMode::getDesktopMode().width;
    resolution.y = VideoMode::getDesktopMode().height;
    RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);
    
    View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));
    
    Clock clock;

    Time gameTimeTotal;

    Vector2f mouseWorldPosition;

    Vector2i mouseScreenPosition;

    Player player;

    IntRect arena;

    VertexArray background;

    Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

    int numZombies = 0;
    int numZombiesAlive = 0;
    Zombie* zombies = nullptr;

    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 1;

    Time lastPressed;

    window.setMouseCursorVisible(false);
    Sprite spriteCrosshair;
    Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
    spriteCrosshair.setTexture(textureCrosshair);
    spriteCrosshair.setOrigin(25, 25);

    while (window.isOpen()) {

        /*
        ****************************************
        Handle Input
        ****************************************
        */

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Return && state == State::PLAYING) {
                    state = State::PAUSED;
                }
                else if (event.key.code == Keyboard::Return && state == State::PAUSED)
                {
                    state = State::PLAYING;
                    clock.restart();
                }
                else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
                {
                    state = State::LEVELING_UP;
                }

                if (state == State::PLAYING) {
                    if (event.key.code == Keyboard::R) {
                        if (bulletsSpare >= clipSize) {
                            bulletsInClip = clipSize;
                            bulletsSpare -= clipSize;
                        }
                        else if (bulletsSpare > 0) 
                        {
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                        }
                        else
                        {
                            //Later
                        }
                    }
                }
            }// End if (event.type == Event::KeyPressed)
        }// End event polling

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        if (state == State::PLAYING) {
            if (Keyboard::isKeyPressed(Keyboard::W))
            {
                player.moveUp();
            }
            else
            {
                player.stopUp();
            }
        }

        if (state == State::PLAYING) {
            if (Keyboard::isKeyPressed(Keyboard::S))
            {
                player.moveDown();
            }
            else
            {
                player.stopDown();
            }
        }

        if (state == State::PLAYING) {
            if (Keyboard::isKeyPressed(Keyboard::A))
            {
                player.moveLeft();
            }
            else
            {
                player.stopLeft();
            }
        }

        if (state == State::PLAYING) {
            if (Keyboard::isKeyPressed(Keyboard::D))
            {
                player.moveRight();
            }
            else
            {
                player.stopRight();
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0) {
                bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y,
                    mouseWorldPosition.x, mouseWorldPosition.y);
                currentBullet++;
                if (currentBullet > 99) {
                    currentBullet = 0;
                }
                lastPressed = gameTimeTotal;
                bulletsInClip--;
            }
        }

        if (state == State::LEVELING_UP) {

            if (event.key.code == Keyboard::Num1) {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num2) {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num3) {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num4) {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num5) {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num6) {
                state = State::PLAYING;
            }

            if (state == State::PLAYING)
            {
                arena.width = 500;
                arena.height = 500;
                arena.left = 0;
                arena.top = 0;

                int tileSize = createBackground(background, arena);

                player.spawn(arena, resolution, tileSize);

                numZombies = 10;

                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                numZombiesAlive = numZombies;

                clock.restart();
            }
        }

        /*
        ****************************************
        UPDATE THE FRAME
        ****************************************
        */

        if (state == State::PLAYING) {
            Time dt = clock.restart();

            gameTimeTotal += dt;

            float dtAsSeconds = dt.asSeconds();

            mouseScreenPosition = Mouse::getPosition(window);

            mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

            spriteCrosshair.setPosition(mouseWorldPosition.x, mouseWorldPosition.y);

            player.update(dtAsSeconds, mouseScreenPosition);

            Vector2f playerPosition(player.getCenter());

            mainView.setCenter(player.getCenter());

            for (int i = 0; i < numZombies; i++) {
                if (zombies[i].isAlive()) {
                    zombies[i].update(dt.asSeconds(), playerPosition);
                }
            }

            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {
                    bullets[i].update(dtAsSeconds);
                }
            }
        }

        /*
        ****************************************
        DRAW THE SCENE
        ****************************************
        */

        if (state == State::PLAYING) {

            window.clear();

            window.setView(mainView);

            window.draw(background, &textureBackground);

            for (int i = 0; i < numZombies; i++) {
                window.draw(zombies[i].getSprite());
            }

            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {
                    window.draw(bullets[i].getShape());
                }
            }

            window.draw(player.getSprite());

            window.draw(spriteCrosshair);
        }

        if (state == State::LEVELING_UP) {

        }

        if (state == State::PAUSED) {

        }

        if (state == State::GAME_OVER) {

        }

        window.display();
    }

    delete[] zombies;

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
