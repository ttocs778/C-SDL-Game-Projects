#include "HeightMap.h"
#include "Game.h"
#include "CSVHelper.h"
#include <fstream>
#include <iostream>

HeightMap::HeightMap(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load CSV: %s", fileName.c_str());
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			std::vector<std::string> strings = CSVHelper::Split(line);
			mCol = 0;

			if (mRow >= mGrids.size())
			{
				mGrids.resize(mRow + 1);
			}

			for (int i = 0; i < strings.size(); i++)
			{
				int integer = std::stoi(strings[i]);
				if (mCol >= mGrids[mRow].size())
				{
					mGrids[mRow].resize(strings.size());
				}
				mGrids[mRow][mCol] = integer;
				mCol++;
			}
			mRow++;
		}
	}
	file.close();
}

bool HeightMap::IsCellOnTrack(int row, int col) const
{
	if (row < 0 || row >= mGrids.size())
	{
		return false;
	}

	if (col < 0 || col >= mGrids[row].size())
	{
		return false;
	}

	if (mGrids[row][col] == -1)
	{
		return false;
	}

	return true;
}

float HeightMap::GetHeightFromCell(int row, int col) const
{
	float height = static_cast<float>(mGrids[row][col]) * 5.0f - 40.0f;
	return height;
}

Vector3 HeightMap::CellToWorld(int row, int col) const
{
	if (!IsCellOnTrack(row, col))
	{
		return Vector3::Zero;
	}
	else
	{
		Vector3 pos{GRID_TOP - CELL_SIZE * row, GRID_LEFT + CELL_SIZE * col,
					GetHeightFromCell(row, col)};

		return pos;
	}
}

Vector2 HeightMap::WorldToCell(const Vector2& pos) const
{

	int row = static_cast<int>((Math::Abs(pos.x - GRID_TOP) + CELL_SIZE / 2.0f) / CELL_SIZE);
	int col = static_cast<int>((Math::Abs(pos.y - GRID_LEFT) + CELL_SIZE / 2.0f) / CELL_SIZE);

	return Vector2(static_cast<float>(row), static_cast<float>(col));
}

bool HeightMap::IsOnTrack(const Vector2& pos) const
{
	Vector2 cell = WorldToCell(pos);
	return IsCellOnTrack(static_cast<int>(cell.x), static_cast<int>(cell.y));
}

float HeightMap::GetHeight(const Vector2& pos) const
{
	if (!IsOnTrack(pos))
	{
		return -1000.0f;
	}
	Vector2 cell = WorldToCell(pos);
	return GetHeightFromCell(static_cast<int>(cell.x), static_cast<int>(cell.y));
}