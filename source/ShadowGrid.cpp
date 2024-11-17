#include "precomp.h"
#include "ShadowGrid.h"

//side size should be number on the power of two
ShadowGrid::ShadowGrid(float sideSize)
{
	cellSideSize = sideSize;
	for (int y = 0; y < SCRHEIGHT / sideSize; y++)
	{
		for (int x = 0; x < SCRWIDTH / sideSize; x++)
		{
			float2 coords = { x * cellSideSize, y * cellSideSize };
			cells.push_back(coords);
		}
	}

}