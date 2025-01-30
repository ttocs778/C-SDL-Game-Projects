#include "TiledBGComponent.h"
#include "Actor.h"
#include "Game.h"

TiledBGComponent::TiledBGComponent(Actor* owner, int drawOrder)
: SpriteComponent(owner)
{
}

void TiledBGComponent::Draw(SDL_Renderer* renderer)
{

	if (mTexture)
	{
		for (int i = 0; i < mTileMap.size(); i++)
		{
			for (int j = 0; j < mTileMap[i].size(); j++)
			{
				int tileID = mTileMap[i][j];
				if (tileID != -1)
				{
					SDL_Rect srcRect;
					srcRect.w = mWidth;
					srcRect.h = mHeight;
					int tilesPerRow = GetTexWidth() / mWidth;
					srcRect.x = (tileID % tilesPerRow) * mWidth;
					srcRect.y = (tileID / tilesPerRow) * mHeight;

					SDL_Rect dstRect;
					dstRect.w = mWidth;
					dstRect.h = mHeight;
					dstRect.x = static_cast<int>(j * mWidth - mOwner->GetGame()->GetCameraPos().x);
					dstRect.y = static_cast<int>(i * mHeight - mOwner->GetGame()->GetCameraPos().y);

					SDL_RenderCopyEx(renderer, mTexture, &srcRect, &dstRect, 0.0, nullptr,
									 SDL_FLIP_NONE);
				}
			}
		}
	}
}

void TiledBGComponent::LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight)
{
	mWidth = tileWidth;
	mHeight = tileHeight;
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load CSV: %s", fileName.c_str());
		return;
	}
	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		if (!line.empty())
		{
			std::vector<std::string> strings = CSVHelper::Split(line);
			mCol = 0;
			if (mRow >= mTileMap.size())
				mTileMap.resize(mRow + 1);
			for (int i = 0; i < strings.size(); i++)
			{
				int integer = std::stoi(strings[i]);
				if (mCol >= mTileMap[mRow].size())
					mTileMap[mRow].resize(strings.size());
				mTileMap[mRow][mCol] = integer;
				mCol++;
			}
			mRow++;
		}
	}
}