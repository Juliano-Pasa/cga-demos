#pragma once

#include <vector>
#include <string>

using namespace std;

class TerrainGenerator
{
private:
	int size;
	int scale;
	float smoothness;
	vector<vector<unsigned char>> map;

	void SetInitialValues(vector<unsigned char> initialValues);

	void DiamondStep(int i, int j, int currentSize);
	void SquareStep(int i, int j, int centerOffset);

	unsigned char UCharClamp(int value);

public:
	TerrainGenerator(int power, vector<unsigned char> initialValues, int scale, float smoothness, float seed);
	void GenerateDiamondSquare();
	void PrintMap();
	void WriteToCSV(string path);
};