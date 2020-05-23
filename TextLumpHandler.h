#include <fstream>

#include "Resource.h"
#pragma once

// Lump handling functions for Text data lumps.
static bool LoadTextLump(std::fstream *, pResourceLump);
static bool SaveTextLump(std::fstream *, pResourceLump);
static void UnloadTextLump(pResourceLump);

///////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS

//- LoadTextLump ------------------------------------------------------------
// Description: Loads in the data for a Text data lump.
// Parameters:  aStream - pointer to the file stream object.
//              aLump   - pointer to the lump node.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
static bool LoadTextLump(std::fstream *aStream, pResourceLump aLump)
{
	// First, allocate enough memory to the load the data into.
	if ((aLump->pData = (void *)(new char[aLump->Size])) == NULL)
		return false;

	// Next, load in the data.
	aStream->read((char*)(aLump->pData), aLump->Size);
	if (aStream->fail())
	{
		delete[](char *)(aLump->pData);
		aLump->pData = nullptr;
		return false;
	}

	// It worked, so return true.
	return true;
}

//- SaveTextLump ------------------------------------------------------------
// Description: Saves the data of a Text data lump to a file.
// Parameters:  aStream - pointer to the file stream object.
//              aLump   - pointer to the lump node.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
static bool SaveTextLump(std::fstream *aStream, pResourceLump aLump)
{
	// Write the data to the file.
	aStream->write((char *)(aLump->pData), aLump->Size);
	if (aStream->fail())
		return false;

	// It worked, so return true!
	return true;
}

//- UnloadTextLump ----------------------------------------------------------
// Description: Unloads the data of a Text data lump.
// Parameters:  aLump   - pointer to the lump node.
//-----------------------------------------------------------------------------
static void UnloadTextLump(pResourceLump aLump)
{
	// Delete the memory.
	delete[](char *)(aLump->pData);
	aLump->pData = nullptr;
}
