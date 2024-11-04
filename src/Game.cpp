#include "Game.hpp"
#include "Math.hpp"
#include "imgui.h"

sf::Texture backgroundTxt;
std::shared_ptr<sf::RectangleShape> Game::background{std::make_shared<sf::RectangleShape>(sf::Vector2f{SCREEN_WIDTH + TILE_SIZE * 3, SCREEN_HEIGHT + TILE_SIZE * 3})};

void Game::Init()
{
    /*
        It has to be static because I load textures from global arrays where cpp standard don't guarante
        order of initialization beetwen Translation Units
    */
    Player::loadTextures();
    Tile::loadTextures();
    Item::LoadTextures();
    Item::InitBackground();
    Game::InitBackground();
}

void Game::Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime)
{
    if(gameState == MENU)
    {
        ImGui::Begin("Menu");
        if(ImGui::Button("Load Game"))
        {
            Load();
            gameState = MAP;
        }
        if( ImGui::Button("New Game") )
        {
            player.position = {SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f};
            player.canJump = true;
            gameState = PLAYER_SELECTOR;
        }
        ImGui::End();
    }
    else if(gameState == MAP)
    {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !player.inventory.isMouseInInventory)
        {
            HandleMouseInput(mousePos, windowCenter, deltaTime);
        }
        player.Update(deltaTime);
        map.HandleCollisions(player, deltaTime);
        player.inventory.Update(mousePos, windowCenter, event);
        player.Move(player.velocity * deltaTime);
        if(player.IsDead())
        {
            SpawnPlayer();
        }
        ImGui::Begin("Player info");
            ImGui::Text("Can player jump: %s" , player.canJump? "True" : "False");
            ImGui::Text("Player velocity %i, %i", (int)player.velocity.x, (int)player.velocity.y);
            ImGui::Text("Player position %i, %i", (int)player.position.x, (int)player.position.y);
            if( ImGui::Button("Kill player") )
            {
                player.health = 0.0f;
            }
            if( ImGui::Button("Go to menu") )
            {
                Save();
                map = Map();
                player = Player();
                gameState = MENU;
            }
            const float fps = 1.0f / deltaTime;
            ImGui::Text("FPS %u", (unsigned)fps);
        ImGui::End();
    }
    else if(gameState == SETTINGS)
    {

    }
    else if(gameState == PLAYER_SELECTOR)
    {
        ImGui::Begin("Character selector\n");
        ImGui::ColorEdit3("Skin color", player.character.skinColor);
        ImGui::ColorEdit3("Eyes color", player.character.eyeColor);
        ImGui::ColorEdit3("Hair color", player.character.hairColor);
        ImGui::ColorEdit3("Pants color", player.character.pantsColor);
        ImGui::ColorEdit3("Clothes color", player.character.clothesColor);
        if( ImGui::Button("Creat character") )
        {
            map.Generate();
            SpawnPlayer();
            gameState = MAP;
        }
        ImGui::End();
    }
}

void Game::Draw(Vector2 mousePos, sf::RenderWindow &window)
{
    if(gameState == MENU)
    {
        background->setPosition(player.position);
        window.draw(*background);
    }
    else if(gameState == MAP)
    {
        background->setPosition(player.position);
        window.draw(*background);
        map.Draw(player.position, window);
        player.Draw({1.0f, 1.0f}, window);
        player.inventory.Draw(mousePos, window);
    }
    else if(gameState == PLAYER_SELECTOR)
    {
        background->setPosition(player.position);
        window.draw(*background);
        player.Draw({3.0f, 3.0f}, window);
    }
    else if(gameState == SETTINGS)
    {
        
    }
}

void Game::Save()
{
    map.Save();
    player.Save();
}

void Game::Load()
{
    map.Load();
    player.Load();
}

void Game::SpawnPlayer()
{
    player.health = 100.0f;
    for(int y = 2; y < MAP_HEIGHT; y++)
    {
        const Tile &t1 = map.UnsafeGetTile({MAP_WIDTH / 2, y});
        const Tile &t2 = map.UnsafeGetTile({MAP_WIDTH / 2 + 1, y});
        const Tile &t3 = map.UnsafeGetTile({MAP_WIDTH / 2 - 1, y});
        if(t3.isCollidable())
        {
            player.position = Vector2{MAP_WIDTH / 2, y - 2 } * TILE_SIZE;
            break;
        }
        if(t1.isCollidable())
        {
            player.position = Vector2{MAP_WIDTH / 2, y - 2} * TILE_SIZE;
            break;
        }
        if(t2.isCollidable())
        {
            player.position = Vector2{MAP_WIDTH / 2, y - 2} * TILE_SIZE;
            break;
        }
    }
}

void Game::HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime)
{
    const Item playerItem = player.inventory.GetCurrentItem();
    const Vector2 mouseCoords = Round(mousePos / TILE_SIZE);
    const std::vector<Vector2> breakableTiles = map.GetBreakableTilesCoords(player.position, player.size);

    if(playerItem.IsWeapon())
    {
        // std::cout << "Attack\n";
    }
    else if(playerItem.IsTool())
    {
        Tile *const tile = map.Raycast(player.position, mousePos);
        if(tile == nullptr)
        {
            return;
        }

        if(!tile->isNone())
        {
            if(tile->type == Tile::LOG)
            {
                std::vector<Vector2> treeTilesCoords = map.GetTreeTilesCoords(tile->GetCoords());
                tile->durability -= (player.strength + playerItem.damage) * deltaTime;
                if(tile->durability <= 0.0f)
                {
                    for(int i = 1; i < treeTilesCoords.size(); i++)
                    {
                        Tile &t = map.UnsafeGetTile({treeTilesCoords[i].x, treeTilesCoords[i].y});
                        player.inventory.FindSlotForItem(Tile::Type::LOG);
                        t.SetProperties(Tile::NONE);
                        map.UpdateSurroundingTiles(t.GetCoords());
                    }
                }
            }
            else
            {
                tile->durability -= (player.strength + playerItem.damage) * deltaTime;
                if(tile->durability <= 0.0f)
                {
                    player.inventory.FindSlotForItem(tile->type);
                    tile->SetProperties(Tile::NONE);
                    map.UpdateSurroundingTiles(tile->GetCoords());
                }
            }
        }
    }
    else if(playerItem.IsBlock() && player.canPlaceBlock)
    {
        // Checks if user is clicking in player's range 
        if(!PointRectCollision(mouseCoords, breakableTiles[0], breakableTiles[breakableTiles.size() - 1]))
        {
            return;
        }

        Tile &tile = map.UnsafeGetTile({mouseCoords.x, mouseCoords.y}); 
        // this part of code is suspicious
        const std::vector<Vector2> playerbb = map.GetPlayerBBTilesCoords(player.position);
        if(!PointRectCollision(mouseCoords, playerbb[0],
                                playerbb[playerbb.size() - 1]) && tile.isNone())
        {
            // checks if I can place a block
            if(!map.UnsafeGetTile({mouseCoords.x + 1, mouseCoords.y}).isNone()
                || !map.UnsafeGetTile({mouseCoords.x - 1, mouseCoords.y}).isNone() 
                || !map.UnsafeGetTile({mouseCoords.x, mouseCoords.y - 1}).isNone() 
                || !map.UnsafeGetTile({mouseCoords.x, mouseCoords.y + 1}).isNone())
            {
                player.inventory.PlaceBlock();
                tile.SetProperties(playerItem.type);
                tile.UpdateTextureRect(map.CheckTileIntersection({mouseCoords.x, mouseCoords.y}));
                map.UpdateSurroundingTiles({mouseCoords.x, mouseCoords.y});
            }
        }
    }
}

void Game::InitBackground()
{
    if( !backgroundTxt.loadFromFile("resources/background.png") )
    {
        std::cout << "Unable to load file background.png\n";
    }
    background->setOrigin(background->getSize() / 2.0f);
    background->setTexture(&backgroundTxt);
}