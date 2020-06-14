#include "FiniteDifferenceWaveSolver.h"
#include "Script.h"
#include "Primitive.h"
#include "GameObject.h"
#include "ImGuiWidget.h"

#define INDEX(x, y, width) (x * width) + y

void FiniteDifferenceWaveSolver::Initialize()
{
	CreateGrid();

	// Initialize data buffers
	WaveHeightCurrent = new float *[100];
	WaveHeightPrevious = new float *[100];
	for (int m = 0; m < 100; ++m)
	{
		WaveHeightCurrent[m] = new float[100];
		WaveHeightPrevious[m] = new float[100];
	}

	CalculateInitialValues();
}

void FiniteDifferenceWaveSolver::Behavior()
{
	DrawWaveSolverSettingsWidget();
	CalculateHeightValues();
	UpdateGrid();
}

float FiniteDifferenceWaveSolver::WaveControlFunctionF(float x, float y)
{
	return ((1 - abs(sin(x))) * (1 - abs(sin(y))) + 2 * (1 - abs(sin(x))) * (1 - abs(sin(3 * y))));
}

float FiniteDifferenceWaveSolver::WaveControlFunctionG(float x, float y)
{
	return (0.5f * (1.0f - abs(sin(x))) * (1.0f - abs(sin(2.0f * y))) + 2.0f * (1.0f - abs(sin(5.0f * x))) * (1.0f - abs(sin(7.0f * y))));
}

float FiniteDifferenceWaveSolver::WaveControlFunctionS(float x, float y, float t)
{
	return -((abs(sin(5 * x))) * (1 - sin(5 * y)) + 2 * (1 - abs(cos(3 * t))) * (1 - abs(sin(8 * y))));
}

void FiniteDifferenceWaveSolver::CalculateInitialValues()
{
	for (int i = 0; i < SectionsX; ++i)
	{
		for (int j = 0; j < SectionsY; ++j)
		{
			float xi = i * DeltaX;
			float yj = j * DeltaY;
			WaveHeightCurrent[i][j] = (WaveControlFunctionF(xi, yj) + (WaveControlFunctionG(xi, yj) * TimeStep));
			WaveHeightPrevious[i][j] = WaveHeightCurrent[i][j];
		}
	}
}

void FiniteDifferenceWaveSolver::CalculateHeightValues()
{
	TimeStep += 0.0000016f; 
	for (int i = 0; i < SectionsX; ++i)
	{
		for (int j = 0; j < SectionsY; ++j)
		{
			// Boundary conditions - skip calculations
			if (i == 0)
			{
				WaveHeightCurrent[0][j] = 0;
				continue;
			}
			else if (j == 0)
			{
				WaveHeightCurrent[i][0] = 0;
				continue;
			}
			else if (i == SectionsX - 1)
			{
				WaveHeightCurrent[i][j] = 0;
				continue;
			}
			else if (j == SectionsY - 1)
			{
				WaveHeightCurrent[i][j] = 0;
				continue;
			}
			ConstantR = (ConstantC * TimeStep) / DeltaX;
			float right = WaveHeightCurrent[i + 1][j];
			float left = WaveHeightCurrent[i - 1][j];
			float up = WaveHeightCurrent[i][j + 1];
			float down = WaveHeightCurrent[i][j - 1];
			float centerCurrent = WaveHeightCurrent[i][j];
			float centerPrevious = WaveHeightPrevious[i][j];
			float xi = i * DeltaX;
			float yj = j * DeltaY;
			float S = WaveControlFunctionS(xi, yj, TimeStep) * powf(TimeStep, 2);
			float newHeightValue;
			newHeightValue = (powf(ConstantR, 2) * (right + left + up + down - (4 * centerCurrent)) + (2 * centerCurrent) - centerPrevious + S);
			// Update previous height values
			WaveHeightPrevious[i][j] = WaveHeightCurrent[i][j];
			WaveHeightCurrent[i][j] = newHeightValue;
		
		}
	}
}

void FiniteDifferenceWaveSolver::CreateGrid()
{
	// Create grid for owner 
	WaveGrid = Grid(SectionsX, SectionsY, SizeX, SizeY);
	WaveGrid.SpacingX = SpacingX;
	WaveGrid.SpacingY = SpacingY;

	Primitive * gridMesh = nullptr;
	gridMesh = pOwningScript->GetOwner()->GetComponent<Primitive>();
	if (gridMesh)
	{
		gridMesh->BindVertexData(WaveGrid.GridVertices);
	}
}

void FiniteDifferenceWaveSolver::UpdateGrid()
{
	WaveGrid = Grid(SectionsX, SectionsY, SizeX, SizeY);
	WaveGrid.Color = vector3(0, 0, 1);
	WaveGrid.SpacingX = SpacingX;
	WaveGrid.SpacingY = SpacingY;
	for (int i = 0; i < SectionsX; ++i)
	{
		for (int j = 0; j < SectionsY; ++j)
		{
			WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Position.y = WaveHeightCurrent[i][j] * Displacement;
			WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Color = vector4(0.1f, 0.1f, WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Position.y, 1);
		}
	}
	WaveGrid.CalculateGrid();
	Primitive * gridMesh = nullptr;
	gridMesh = pOwningScript->GetOwner()->GetComponent<Primitive>();
	if (gridMesh)
	{
		gridMesh->Debuffer();
		gridMesh->BindVertexData(WaveGrid.GridVertices);
	}
}

void FiniteDifferenceWaveSolver::DrawWaveSolverSettingsWidget()
{
	ImGui::Begin("Wave Solver Settings:");

	ImGui::SliderFloat("Spacing X:", &SpacingX, 0.1f, 5);
	ImGui::SliderFloat("Spacing Y:", &SpacingY, 0.1f, 5);

	int xSections = SectionsX, ySections = SectionsY;
	ImGui::SliderInt("Resolution X:", &xSections, 20, 100);
	ImGui::SliderInt("Resolution Y:", &ySections, 20, 100);
	// If either value is changed, recalculate the buffers and constants
	if (xSections != SectionsX || ySections != SectionsY)
	{
		SectionsX = xSections;
		SectionsY = ySections;
	}

	float xScale = WaveScaleX, yScale = WaveScaleY;
	ImGui::InputFloat("Wave Scale X:", &xScale, 0.0001f, 0.001f);
	ImGui::InputFloat("Wave Scale Y:", &yScale, 0.0001f, 0.001f);
	if (xScale != WaveScaleX || yScale != WaveScaleY)
	{
		WaveScaleX = xScale;
		WaveScaleY = yScale;
		// If either value is changed, reset the height values of wave
		for (int m = 0; m < SectionsX; ++m)
		{
			for (int n = 0; n < SectionsY; ++n)
			{
				WaveHeightCurrent[m][n] = 0.0f;
			}
		}
	}

	float c = ConstantC;
	ImGui::InputFloat("Wave Equation Constant:", &c, 0.01f, 0.1f);
	if (c != ConstantC)
	{
		ConstantC = c;
		// If value is changed, reset the height values of wave
		for (int m = 0; m < SectionsX; ++m)
		{
			for (int n = 0; n < SectionsY; ++n)
			{
				WaveHeightCurrent[m][n] = 0.0f;
			}
		}
	}

	ImGui::SliderFloat("Wave Height Scalar:", &Displacement, 1.f, 20.f);

	float t = TimeStep;
	ImGui::InputFloat("Timestep:", &t, 0.0001f, 0.001f);
	if (t != TimeStep)
	{
		TimeStep = t;
		// If value is changed, reset the height values of wave
		for (int m = 0; m < SectionsX; ++m)
		{
			for (int n = 0; n < SectionsY; ++n)
			{
				WaveHeightCurrent[m][n] = 0.0f;
			}
		}
	}

	DeltaX = 1.0f / SectionsX;
	DeltaY = 1.0f / SectionsY;
	ImGui::End();
}

void FiniteDifferenceWaveSolver::Destroy()
{
	for (int i = 0; i < SectionsY; ++i)
	{
		delete[] WaveHeightCurrent[i];
	}
	delete[] WaveHeightCurrent;
}
