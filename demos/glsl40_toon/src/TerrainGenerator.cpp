#include "TerrainGenerator.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include "lodepng.h"


TerrainGenerator::TerrainGenerator(int power, vector<double> initialValues, int scale, float smoothness, float seed)
{
	this->scale = scale;
	this->smoothness = pow(2, -smoothness);

	size = pow(2, power) + 1;
	map = vector<vector<double>>(size, vector<double>(size));

	srand(seed);
	SetInitialValues(initialValues);
	minHeight = *min_element(initialValues.begin(), initialValues.end());
	maxHeight = *max_element(initialValues.begin(), initialValues.end());
}

void TerrainGenerator::SetInitialValues(vector<double> initialValues)
{
	map[0][0] = initialValues[0];
	map[0][size - 1] = initialValues[1];
	map[size - 1][0] = initialValues[2];
	map[size - 1][size - 1] = initialValues[3];
}

#pragma region DiamondSquareFunctions

void TerrainGenerator::GenerateDiamondSquare()
{
	int currentSize = size;
	while (currentSize > 2)
	{
		for (size_t i = 0; i < size - 1; i += currentSize - 1)
		{
			for (size_t j = 0; j < size - 1; j += currentSize - 1)
			{
				DiamondStep(i, j, currentSize - 1);
			}
		}

		int centerOffset = currentSize / 2;
		int start = centerOffset;

		for (size_t i = 0; i < size; i += centerOffset)
		{
			for (size_t j = start; j < size; j += currentSize - 1)
			{
				SquareStep(i, j, centerOffset);
			}
			start = start == 0 ? centerOffset : 0;
		}

		currentSize = currentSize / 2 + 1;
		scale *= smoothness;
		if (scale < 1)
		{
			scale = 1;
		}
	}

	cout << "min Max: " << minHeight << "  " << maxHeight << endl;
	NormalizeHeightMap();
	cout << "min Max: " << minHeight << "  " << maxHeight << endl;
}

void TerrainGenerator::DiamondStep(int i, int j, int currentSize)
{
	int centerOffset = currentSize / 2;
	double average = (map[i][j] + map[i][j + currentSize] + map[i + currentSize][j] + map[i + currentSize][j + currentSize]) / 4;
	average += static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / scale)) - (scale / 2);

	map[i + centerOffset][j + centerOffset] = average;
	UpdateMinMaxHeight(average);
}

void TerrainGenerator::SquareStep(int i, int j, int centerOffset)
{
	double average = 0;
	double count = 0;

	if (i - centerOffset >= 0)
	{
		average += map[i - centerOffset][j];
		count++;
	}
	if (i + centerOffset < size)
	{
		average += map[i + centerOffset][j];
		count++;
	}
	if (j - centerOffset >= 0)
	{
		average += map[i][j - centerOffset];
		count++;
	}
	if (j + centerOffset < size)
	{
		average += map[i][j + centerOffset];
		count++;
	}

	average /= count;
	average += static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / scale)) - (scale / 2);

	map[i][j] = average;
	UpdateMinMaxHeight(average);
}

#pragma endregion

void TerrainGenerator::GenerateTestFunction()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			map[i][j] = (unsigned int) (i + j)/2;
		}
	}
}

void TerrainGenerator::UpdateMinMaxHeight(double value)
{
	if (value < minHeight)
	{
		minHeight = value;
	}
	else if (value > maxHeight)
	{
		maxHeight = value;
	}
}

void TerrainGenerator::NormalizeHeightMap()
{
	float interval = maxHeight - minHeight;
	for (auto& line : map)
	{
		for (int j = 0; j < line.size(); j++)
		{
			line[j] = (line[j] - minHeight) / interval;
		}
	}
}

#pragma region NormalMapFunctions

void TerrainGenerator::GenerateNormalMap()
{
	normalMap = vector<vector<vec3>>(size, vector<vec3>(size));

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			normalMap[i][j] = CalculateNormal(i, j);
		}
	}
}

vec3 TerrainGenerator::CalculateNormal(int i, int j)
{
	float up = map[i][j];
	float down = map[i][j];
	float right = map[i][j];
	float left = map[i][j];

	if (i - 1 >= 0)
	{
		up = map[i - 1][j];
	}
	if (i + 1 < size)
	{
		down = map[i + 1][j];
	}
	if (j - 1 >= 0)
	{
		left = map[i][j - 1];
	}
	if (j + 1 < size)
	{
		right = map[i][j + 1];
	}

	float zVec = up - down;
	float xVec = right - left;

	return glm::normalize(vec3(2.0f * zVec, 4, 2.0f * xVec));
}

string TerrainGenerator::NormalToString(vec3 normal)
{
	return to_string(normal.x) + ";" + to_string(normal.y) + ";" + to_string(normal.z);
}

#pragma endregion


#pragma region OutputFunctions

void TerrainGenerator::PrintMap()
{
	for (auto& line : map)
	{
		for (auto vertex : line)
		{
			cout << vertex << " ";
		}
		cout << endl;
	}
}

void TerrainGenerator::WriteHeightMapToCSV(string path)
{
	ofstream outputFile(path);

	for (auto& line : map)
	{
		for (size_t i = 0; i < line.size() - 1; i++)
		{
			outputFile << line[i] << ",";
		}
		outputFile << line[line.size() - 1] << "\n";
	}
	outputFile.close();
}

void TerrainGenerator::WriteNormalMapToCSV(string path)
{
	ofstream outputFile(path);

	for (auto& line : normalMap)
	{
		for (size_t i = 0; i < line.size() - 1; i++)
		{
			outputFile << NormalToString(line[i]) << ",";
		}
		outputFile << NormalToString(line[line.size() - 1]) << "\n";
	}
	outputFile.close();
}

void TerrainGenerator::WriteHeightMapToPNG(string path)
{
	vector<unsigned char> image;

	for (size_t i = 0; i < map.size(); i++)
	{
		for (size_t j = 0; j < map[i].size(); j++)
		{
			image.push_back(map[i][j]); // R
			image.push_back(map[i][j]); // G
			image.push_back(map[i][j]); // B
			image.push_back((unsigned char)255); // A
		}
	}

	lodepng::encode(path, image, size, size);
}

void TerrainGenerator::WriteNormalMapToPNG(string path)
{
	vector<unsigned char> image;

	for (size_t i = 0; i < normalMap.size(); i++)
	{
		for (size_t j = 0; j < normalMap[i].size(); j++)
		{
			vec3 current = normalMap[i][j];
			current = (current + vec3(1)) / 2.0f * 255.0f;
			image.push_back((unsigned char)current.x); // R
			image.push_back((unsigned char)current.y); // G
			image.push_back((unsigned char)current.z); // B
			image.push_back((unsigned char)255); // A
		}
	}

	lodepng::encode(path, image, size, size);
}

#pragma endregion


void TerrainGenerator::FreeMap()
{
	map.clear();
}