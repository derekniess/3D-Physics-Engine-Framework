#pragma once
#include "ScriptBehavior.h"
#include "Grid.h"

#define E_PI 3.141592f

struct FiniteDifferenceWaveSolver : public ScriptBehavior
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	int SectionsX = 20;
	int SectionsY = 20;
	float SizeX = 1;
	float SizeY = 1;
	float SpacingX = 1.0f;
	float SpacingY = 1.0f;
	float WaveScaleX = 0.01f;
	float WaveScaleY = 0.001f;
	float DeltaX = 1.0f / SectionsX;
	float DeltaY = 1.0f / SectionsY;
	float ConstantR = 1.0f;
	float ConstantC = 0.5f;
	float ** WaveHeightCurrent = nullptr;
	float ** WaveHeightPrevious = nullptr;
	float TimeStep = 0.00016f;
	float Displacement = 0.1f;
	const float PI = E_PI;

	Grid WaveGrid = Grid(10, 10, 10, 10);
private:
	float lastInterval;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Initialize() override;
	virtual void Behavior() override;
	virtual void Destroy() override;

	float WaveControlFunctionF(float x, float y);
	float WaveControlFunctionG(float x, float y);
	float WaveControlFunctionS(float x, float y, float t);

	void CalculateInitialValues();
	void CalculateHeightValues();
	void CreateGrid();
	void UpdateGrid();
	void DrawWaveSolverSettingsWidget();
};