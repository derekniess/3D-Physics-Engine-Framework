#include "FourierWaveSolver.h"
#include "Script.h"
#include "Primitive.h"
#include "GameObject.h"
#include "ImGuiWidget.h"

#define INDEX(x, y, width) (x * width) + y

void FourierWaveSolver::Initialize()
{
	CreateGrid();

	// Initialize data buffers
	FourierCoefficientAmn = new float *[100];
	WaveHeight = new float *[100];
	for (int m = 0; m < 100; ++m)
	{
		FourierCoefficientAmn[m] = new float[100];
		WaveHeight[m] = new float[100];
	}

	CalculateFourierCoefficients();
}

void FourierWaveSolver::Behavior()
{
	DrawWaveSolverSettingsWidget();
	CalculateHeightValues();
	UpdateGrid();
}

float FourierWaveSolver::WaveControlFunction(float x, float y)
{
	return (sin(sqrt(156 * y)) + cos(sqrt(21 * x)) + cos(sqrt(40 * x))) ;
}

void FourierWaveSolver::CalculateFourierCoefficients()
{
	// Default initialization 
	for (int m = 0; m < SectionsX; ++m)
	{
		for (int n = 0; n < SectionsY; ++n)
		{
			FourierCoefficientAmn[m][n] = 0.0f;
			WaveHeight[m][n] = 0.0f;
		}
	}

	// Calculate Fourier Coefficient A
	for (int m = 0; m < SectionsX; ++m)
	{
		for (int n = 0; n < SectionsY; ++n)
		{
			for (int p = 0; p < SectionsX; ++p)
			{
				for (int q = 0; q < SectionsY; ++q)
				{
					float xi = p * DeltaX * WaveScaleX;
					float yj = q * DeltaY * WaveScaleY;
					float waveControl = WaveControlFunction(xi, yj);
					float firstSine = sin((m * PI * xi) / ConstantA);
					float secondSine = sin((n * PI * yj) / ConstantB);
					FourierCoefficientAmn[m][n] += waveControl * firstSine * secondSine;
				}
			}
			// Perform multiplication after all summations are complete
 			FourierCoefficientAmn[m][n] *= (4 * DeltaX * DeltaY) / (ConstantA * ConstantB);
		}
	}
}

void FourierWaveSolver::CalculateHeightValues()
{
	TotalTime += TimeStep;
	for (int m = 0; m < SectionsX; ++m)
	{
		for (int n = 0; n < SectionsY; ++n)
		{
			// Boundary conditions
			if (m == 0)
			{
				WaveHeight[0][n] = WaveHeight[1][n];
			}
			else if (n == 0)
			{
				WaveHeight[m][0] = WaveHeight[m][1];
			}
			else if (m == SectionsX - 1)
			{
				WaveHeight[m][n] = WaveHeight[m - 1][n];
			}
			else if (n == SectionsY - 1)
			{
				WaveHeight[m][n] = WaveHeight[m][n - 1];
			}
			float xi = m * DeltaX * WaveScaleX;
			float yj = n * DeltaY * WaveScaleX;
			float gridX = WaveGrid.GridPoints[INDEX(m, n, SectionsY)].Position.x * WaveScaleX;
			float gridY = WaveGrid.GridPoints[INDEX(m, n, SectionsY)].Position.z * WaveScaleY;
			float Kmn = PI * sqrt((powf(m, 2) / powf(ConstantA, 2)) + (powf(n, 2) / powf(ConstantB, 2)));
			float firstCosine = cos(Kmn * ConstantC * TotalTime);
			float firstSine = sin((m * PI * gridX) / ConstantA);
			float secondSine = sin((n * PI * gridY) / ConstantB);
			WaveHeight[m][n] += FourierCoefficientAmn[m][n] * firstCosine * firstSine * secondSine;
		}
	}
}

void FourierWaveSolver::CreateGrid()
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

void FourierWaveSolver::UpdateGrid()
{
	WaveGrid = Grid(SectionsX, SectionsY, SizeX, SizeY);
	WaveGrid.Color = glm::vec3(0, 0, 1);
	WaveGrid.SpacingX = SpacingX;
	WaveGrid.SpacingY = SpacingY;
	for (int i = 0; i < SectionsX; ++i)
	{
		for (int j = 0; j < SectionsY; ++j)
		{
			WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Position.y = WaveHeight[i][j] * Displacement;
			WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Color = glm::vec4(0.1f, 0.1f, WaveGrid.GridPoints[INDEX(i, j, SectionsY)].Position.y, 1);
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

void FourierWaveSolver::DrawWaveSolverSettingsWidget()
{
	ImGui::Begin("Wave Solver Settings:");

	ImGui::SliderFloat("Spacing X:", &SpacingX, 0.1f, 5);
	ImGui::SliderFloat("Spacing Y:", &SpacingY, 0.1f, 5);

	int xSections = SectionsX, ySections = SectionsY;
	ImGui::SliderInt("Resolution X:", &xSections, 25, 100);
	ImGui::SliderInt("Resolution Y:", &ySections, 25, 100);
	// If either value is changed, recalculate the buffers and constants
	if (xSections != SectionsX || ySections != SectionsY)
	{
		SectionsX = xSections;
		SectionsY = ySections;
		CalculateFourierCoefficients();
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
				WaveHeight[m][n] = 0.0f;
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
				WaveHeight[m][n] = 0.0f;
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
				WaveHeight[m][n] = 0.0f;
			}
		}
	}

	DeltaX = 1.0f / SectionsX;
	DeltaY = 1.0f / SectionsY;
	ImGui::End();
}

void FourierWaveSolver::Destroy()
{
	for (int i = 0; i < SectionsY; ++i)
	{
		delete[] FourierCoefficientAmn[i];
		delete[] WaveHeight[i];
	}
	delete[] FourierCoefficientAmn;
	delete[] WaveHeight;
}
