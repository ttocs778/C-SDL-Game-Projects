#pragma once
#include "SpriteComponent.h"
#include <vector>
#include "CSVHelper.h"
#include <fstream>

class TiledBGComponent : public SpriteComponent
{
public:
	TiledBGComponent(class Actor* owner, int drawOrder = 50);
	void Draw(SDL_Renderer* renderer) override;
	void LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight);

private:
	int mWidth = 0;
	int mHeight = 0;
	int mRow = 0;
	int mCol = 0;
	std::vector<std::vector<int>> mTileMap;
};