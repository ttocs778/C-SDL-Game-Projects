#pragma once
#include "Game.h"
#include "iostream"

class HeightMap
{
public:
	HeightMap(const std::string& fileName);
	Vector3 CellToWorld(int row, int col) const;
	Vector2 WorldToCell(const Vector2& pos) const;
	bool IsOnTrack(const Vector2& pos) const;
	float GetHeight(const Vector2& pos) const;

private:
	std::vector<std::vector<int>> mGrids;
	int mRow = 0;
	int mCol = 0;
	bool IsCellOnTrack(int row, int col) const;
	float GetHeightFromCell(int row, int col) const;
	const float CELL_SIZE = 40.55f;
	const float GRID_TOP = 1280.0f;
	const float GRID_LEFT = -1641.0f;
};
