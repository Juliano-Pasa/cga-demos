#include "TerrainGenerator.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <ctime>


TerrainGenerator::TerrainGenerator(int power, vector<unsigned char> initialValues, int scale, float smoothness, float seed)
{
	this->scale = scale;
	this->smoothness = pow(2, -smoothness);
	cout << "smoothness: " << this->smoothness << endl;

	size = pow(2, power) + 1;
	map = vector<vector<unsigned char>>(size, vector<unsigned char>(size));

	srand(seed);
	SetInitialValues(initialValues);
}

void TerrainGenerator::SetInitialValues(vector<unsigned char> initialValues)
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
				//cout << "teste";
				DiamondStep(i, j, currentSize - 1);
			}
		}

		//cout << "aaa";
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
}

void TerrainGenerator::DiamondStep(int i, int j, int currentSize)
{
	int centerOffset = currentSize / 2;
	int average = (map[i][j] + map[i][j + currentSize] + map[i + currentSize][j] + map[i + currentSize][j + currentSize]) / 4;
	average += (rand() % scale) - (scale / 2);

	map[i + centerOffset][j + centerOffset] = UCharClamp(average);
}

void TerrainGenerator::SquareStep(int i, int j, int centerOffset)
{
	int average = 0;
	int count = 0;

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
	average += (rand() % scale) - (scale / 2);

	map[i][j] = UCharClamp(average);
}

#pragma endregion


#pragma region OutputFunctions

void TerrainGenerator::PrintMap()
{
	for (auto& line : map)
	{
		for (auto vertex : line)
		{
			cout << (int)vertex << " ";
		}
		cout << endl;
	}
}

void TerrainGenerator::WriteToCSV(string path)
{
	ofstream outputFile(path);

	for (auto& line : map)
	{
		for (size_t i = 0; i < line.size() - 1; i++)
		{
			outputFile << (int)line[i] << ",";
		}
		outputFile << (int)line[line.size() - 1] << "\n";
	}
	outputFile.close();
}

#pragma endregion



unsigned char TerrainGenerator::UCharClamp(int value)
{
	if (value < 0)
	{
		return 0;
	}
	if (value > UCHAR_MAX)
	{
		return UCHAR_MAX;
	}
	return value;
}