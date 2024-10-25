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

    Map map;
    std::srand((unsigned int)std::time(NULL));

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
                map.Save();
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
                map.Load();
            }
            if(ImGui::Button("New Game"))
            {
                inMenu = false;
                map.Generate();
                map.SpawnPlayer();
            }
            ImGui::End();
            window.clear();
            ImGui::SFML::Render(window);
            window.display();
        }
        else
        {
            view.setCenter(map.player.position);
            window.setView(view);
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            sf::Vector2f worldMousePosition = window.mapPixelToCoords(mousePosition);
            map.Update({worldMousePosition.x, worldMousePosition.y}, evnt, deltaTime);
            ImGui::SFML::Update(window, clock);
            ImGui::Begin("Player info");
            ImGui::Text("Can player jump: %s" , map.player.canJump? "True" : "False");
            ImGui::Text("Player velocity %i, %i", (int)map.player.velocity.x, (int)map.player.velocity.y);
            ImGui::Text("Player position %i, %i", (int)map.player.position.x, (int)map.player.position.y);
            if(ImGui::Button("Kill player"))
            {
                map.player.health = 0.0f;
            }
            if(ImGui::Button("Go to menu"))
            {
                inMenu = true;
                map.Save();
                map = Map();
            }
            float fps = 1.0f / deltaTime;
            ImGui::Text("FPS %u", (unsigned)fps);
            ImGui::End();
            window.clear();
            map.Draw(window, {worldMousePosition.x, worldMousePosition.y});
            ImGui::SFML::Render(window);
            window.display();
        }
    }
    ImGui::SFML::Shutdown();
}