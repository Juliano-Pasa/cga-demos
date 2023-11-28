#pragma once

#include <vector>
#include <string>
#include "glslprogram.h"

using namespace std;

class TerrainGenerator
{
private:
	int size;
	int scale;
	float smoothness;
	double minHeight, maxHeight;

	vector<vector<double>> map;
	vector<vector<vec3>> normalMap;

	void SetInitialValues(vector<double> initialValues);

	void DiamondStep(int i, int j, int currentSize);
	void SquareStep(int i, int j, int centerOffset);

	void UpdateMinMaxHeight(double value);
	void NormalizeHeightMap();

	vec3 CalculateNormal(int i, int j);
	string NormalToString(vec3 normal);

public:
	TerrainGenerator(int power, vector<double> initialValues, int scale, float smoothness, float seed);
	void GenerateDiamondSquare();
	void GenerateNormalMap();
	void GenerateTestFunction();
	void PrintMap();
	void WriteHeightMapToCSV(string path);
	void WriteNormalMapToCSV(string path);
	void WriteHeightMapToPNG(string path);
	void WriteNormalMapToPNG(string path);
	void FreeMap();
};