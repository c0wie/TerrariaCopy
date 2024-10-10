#include "Map.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include "Math.hpp"
#include <cstdlib> 
#include <ctime>   

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    std::cout << "I've started\n";
    Map map(true);
    std::srand((unsigned int)std::time(NULL));

    sf::Texture texture;
    texture.loadFromFile("resources/Tiles_2.png");

    sf::IntRect rect;
    rect.width = texture.getSize().x / 16;
    rect.height = texture.getSize().y / 22;
    rect.top = 1 * rect.height;
    rect.left = 2 * rect.width;

    sf::RectangleShape box;
    box.setSize({100.0f, 100.0f});
    box.setPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
    box.setTexture(&texture);
    box.setTextureRect(rect);

    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();
    bool isRelased = false;
    bool inMenu = true;
    while(window.isOpen())
    {
        isRelased = false;
        deltaTime = DT_Clock.restart().asSeconds();
        if(deltaTime > 1.0f / 20.0f)
        {
            deltaTime = 1.0f / 20.0f;
        }
        sf::Time clock = sf::seconds(deltaTime);

        sf::Event evnt{};
        while (window.pollEvent(evnt))
        {
            ImGui::SFML::ProcessEvent(evnt);
            if(evnt.type == sf::Event::Closed)
            {
                window.close();
            }
            if(evnt.type ==  sf::Event::MouseButtonReleased && evnt.mouseButton.button == sf::Mouse::Left)
            {
                isRelased = true;
            }
        }
        if(inMenu)
        {
            ImGui::SFML::Update(window, clock);
            ImGui::Begin("Menu");
            if(ImGui::Button("Load Game"))
            {
                inMenu = false;
                map = Map(false);
            }
            if(ImGui::Button("New Game"))
            {
                inMenu = false;
                map = Map(true);
            }
            ImGui::End();
            window.clear();
            window.draw(box);
            ImGui::SFML::Render(window);
            window.display();
        }
        else
        {
            view.setCenter(map.player.position);
            window.setView(view);
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);
            map.Update({worldMousePosition.x, worldMousePosition.y}, isRelased, deltaTime);
            ImGui::SFML::Update(window, clock);
            ImGui::Begin("Player info");
            ImGui::Text("Can player jump: %s" , map.player.canJump? "True" : "False");
            ImGui::Text("Player velocity %i, %i", (int)map.player.velocity.x, (int)map.player.velocity.y);
            ImGui::Text("Player position %i, %i", (int)map.player.position.x, (int)map.player.position.y);
            if(ImGui::Button("Go to menu"))
            {
                inMenu = true;
                map.~Map();
            }
            float fps = 1.0f / deltaTime;
            ImGui::Text("FPS %u", (unsigned)fps);
            ImGui::End();
            window.clear();
            map.Draw(window);
            ImGui::SFML::Render(window);
            window.display();
        }
    }
    ImGui::SFML::Shutdown();
}