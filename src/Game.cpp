#include "Game.hpp"
#include "Math.hpp"
#include "imgui.h"

sf::Texture backgroundTxt;
sf::RectangleShape Game::m_Background{{SCREEN_WIDTH + TILE_SIZE * 3, SCREEN_HEIGHT + TILE_SIZE * 3}};

Game::Game()
{
    // Player::loadTextures();
    // Tile::loadTextures();
    // Item::LoadTextures();
    // Item::InitBackground();
    // Game::InitBackground();
}

void Game::Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime)
{
    if(m_GameState == MENU)
    {
        ImGui::Begin("Menu");
        if(ImGui::Button("Load Game"))
        {
            Load();
            m_GameState = MAP;
        }
        if( ImGui::Button("New Game") )
        {
            m_Player.position = {SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f};
            m_Player.canJump = true;
            m_GameState = PLAYER_SELECTOR;
        }
        ImGui::End();
    }
    else if(m_GameState == MAP)
    {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !m_Player.inventory.isMouseInInventory)
        {
            HandleMouseInput(mousePos, windowCenter, deltaTime);
        }
        m_Player.Update(deltaTime);
        HandleCollisions(deltaTime);
        m_Player.Move(m_Player.velocity * deltaTime);
        m_Player.inventory.Update(mousePos, windowCenter, event);
        if(m_Player.IsDead())
        {
            m_Player.velocity = Vector2(0.0f, 0.0f);
            SpawnPlayer();
        }
        ImGui::Begin("Player info");
            ImGui::Text("Can player jump: %s" , m_Player.canJump? "True" : "False");
            ImGui::Text("Player velocity %i, %i", (int)m_Player.velocity.x, (int)m_Player.velocity.y);
            ImGui::Text("Player position %i, %i", (int)m_Player.position.x, (int)m_Player.position.y);
            ImGui::Text("Player position %i, %i", (int)m_Player.GetCoords().x, (int)m_Player.GetCoords().y);
            if( ImGui::Button("Kill player") )
            {
                m_Player.health = 0.0f;
            }
            if( ImGui::Button("Go to menu") )
            {
                Save();
                m_GameState = MENU;
            }
            const float fps = 1.0f / deltaTime;
            ImGui::Text("FPS %u", (unsigned)fps);
        ImGui::End();
    }
    else if(m_GameState == SETTINGS)
    {

    }
    else if(m_GameState == PLAYER_SELECTOR)
    {
        ImGui::Begin("Character selector\n");
        ImGui::ColorEdit3("Skin color", m_Player.character.skinColor);
        ImGui::ColorEdit3("Eyes color", m_Player.character.eyeColor);
        ImGui::ColorEdit3("Hair color", m_Player.character.hairColor);
        ImGui::ColorEdit3("Pants color", m_Player.character.pantsColor);
        ImGui::ColorEdit3("Clothes color", m_Player.character.clothesColor);
        if( ImGui::Button("Creat character") )
        {
            m_Map.Generate();
            SpawnPlayer();
            m_GameState = MAP;
        }
        ImGui::End();
    }
}

void Game::Draw(Vector2 mousePos, sf::RenderWindow &window)
{
    m_Background.setPosition(m_Player.position);
    window.draw(m_Background);
    if(m_GameState == MENU)
    {
    }
    else if(m_GameState == MAP)
    {
        m_Map.Draw(m_Player.position, window);
        m_Player.Draw({1.0f, 1.0f}, window);
        m_Player.inventory.Draw(mousePos, window);
    }
    else if(m_GameState == PLAYER_SELECTOR)
    {
        m_Player.Draw({3.0f, 3.0f}, window);
    }
    else if(m_GameState == SETTINGS)
    {
        
    }
}

void Game::Save()
{
    m_Map.Save();
    m_Player.Save();
}

void Game::Load()
{
    m_Map.Load();
    m_Player.Load();
}

void Game::SpawnPlayer()
{
    m_Player.health = 100.0f;
    for(int y = 2; y < MAP_HEIGHT; y++)
    {
        const Tile &t1 = m_Map.UnsafeGetTile({MAP_WIDTH / 2, y});
        const Tile &t2 = m_Map.UnsafeGetTile({MAP_WIDTH / 2 + 1, y});
        const Tile &t3 = m_Map.UnsafeGetTile({MAP_WIDTH / 2 - 1, y});
        if(t3.IsCollidable())
        {
            m_Player.position = Vector2{MAP_WIDTH / 2, y - 2 } * TILE_SIZE;
            break;
        }
        if(t1.IsCollidable())
        {
            m_Player.position = Vector2{MAP_WIDTH / 2, y - 2} * TILE_SIZE;
            break;
        }
        if(t2.IsCollidable())
        {
            m_Player.position = Vector2{MAP_WIDTH / 2, y - 2} * TILE_SIZE;
            break;
        }
    }
}

void Game::HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime)
{
    const Item playerItem = m_Player.inventory.GetCurrentItem();
    const Vector2 mouseCoords = Round(mousePos / TILE_SIZE);
    const std::vector<Vector2> breakableTiles = m_Map.GetBreakableTilesCoords(m_Player.position, m_Player.size);

    if(playerItem.IsWeapon())
    {
        // std::cout << "Attack\n";
    }
    else if(playerItem.IsTool())
    {
        Tile *const tile = m_Map.Raycast(m_Player.position, mousePos);
        if(tile == nullptr)
        {
            return;
        }

        if(!tile->IsNone())
        {
            tile->AddDurability( (m_Player.strength + playerItem.damage) * deltaTime * -1);
            if(tile->GetDurability() <= 0.0f)
            {
                // log has the same light consumption as Tile::NONE so we don't have to update lighting of map
                if(tile->GetType() == Tile::LOG)
                {
                    std::vector<Vector2> treeTilesCoords = m_Map.GetTreeTilesCoords(tile->GetCoords());
                    for(int i = 1; i < treeTilesCoords.size(); i++)
                    {
                        Tile &t = m_Map.UnsafeGetTile({treeTilesCoords[i].x, treeTilesCoords[i].y});
                        m_Player.inventory.FindSlotForItem(Tile::Type::LOG);
                        t.SetProperties(Tile::NONE);
                        m_Map.UpdateSureoundingTilesSubetypes(t.GetCoords());
                    }
                }
                else
                {
                    const int tileLightConsumption = tile->GetLightConsumption();
                    m_Player.inventory.FindSlotForItem( tile->GetType() );
                    // removes lightSource
                    if( tile->IsLightSource() )
                    {
                        m_Map.lightSources.erase(std::remove(m_Map.lightSources.begin(), m_Map.lightSources.end(), tile->GetCoords()));
                    }
                    tile->SetProperties(Tile::NONE);
                    tile->SetLighting(tile->GetLighLevel() );
                    m_Map.UpdateSureoundingTilesSubetypes(tile->GetCoords());
                    m_Map.UpdateLighting();
                }
            }
        }
    }
    else if(playerItem.IsBlock() && m_Player.canPlaceBlock)
    {
        // Checks if user is clicking in player's range 
        if(!PointRectCollision(mouseCoords, breakableTiles[0], breakableTiles[breakableTiles.size() - 1]))
        {
            return;
        }

        Tile &tile = m_Map.UnsafeGetTile({mouseCoords.x, mouseCoords.y}); 
        // this part of code is suspicious
        const std::vector<Vector2> playerbb = m_Map.GetPlayerBBTilesCoords(m_Player.position);
        if( !PointRectCollision(mouseCoords, playerbb[0], playerbb[playerbb.size() - 1]) )
        {
            if( (!m_Map.UnsafeGetTile({mouseCoords.x + 1, mouseCoords.y}).IsNone()
                || !m_Map.UnsafeGetTile({mouseCoords.x - 1, mouseCoords.y}).IsNone() 
                || !m_Map.UnsafeGetTile({mouseCoords.x, mouseCoords.y - 1}).IsNone() 
                || !m_Map.UnsafeGetTile({mouseCoords.x, mouseCoords.y + 1}).IsNone()) && tile.IsNone())
            {
                m_Player.inventory.PlaceBlock();
                tile.SetProperties(playerItem.type);
                tile.SetLighting( tile.GetLighLevel() );
                if(tile.IsLightSource())
                {
                    m_Map.lightSources.push_back(tile.GetCoords());
                }
                tile.SetSubtype(m_Map.CheckTileIntersection(mouseCoords));
                m_Map.UpdateSureoundingTilesSubetypes(mouseCoords);
                // Tile::NONE has lightConsumption = 0 so tile has the same lightConsumption lighting stays the same
                if(tile.GetLightConsumption() != 0 || tile.IsLightSource());
                {
                    m_Map.UpdateLighting();
                }
            }
            // else if(tile.IsWallVisible() && playerItem.CanBeHang())
            // {

            // }
        }
    }
}

void Game::HandleCollisions(float deltaTime)
{
    Vector2 collisionPoint, collisionNormal;
    float collisonTime = 0.0f;
    std::vector<int> realCollisions;

    // retrive collison tiles
    const std::vector<Vector2> possibleCollisions = m_Map.GetCollidableTilesCoords(m_Player.position, m_Player.size);
    realCollisions.reserve(possibleCollisions.size());
    for(int i = 0; i < possibleCollisions.size(); i++)
    { 
        const Tile &tile = m_Map.UnsafeGetTile(possibleCollisions[i]);
        if(!tile.IsCollidable())
        {
            continue;
        }
        if(RectDynamicRectCollision(m_Player, tile, collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            realCollisions.emplace_back(i);
        }
    }
    // resolve player collision
    for(int collisionIndex : realCollisions)
    {
        if(RectDynamicRectCollision(m_Player, m_Map.UnsafeGetTile(possibleCollisions[collisionIndex]), collisionPoint, collisionNormal, collisonTime, deltaTime))
        {
            m_Player.velocity += collisionNormal * Abs(m_Player.velocity) * (1.0f - collisonTime);
            if(collisionNormal == Vector2{0.0f, -1.0f})
            {
                m_Player.canJump = true;
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
    m_Background.setOrigin(m_Background.getSize() / 2.0f);
    m_Background.setTexture(&backgroundTxt);
}