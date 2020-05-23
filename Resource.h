#include <fstream>
#pragma once
///////////////////////////////////////////////////////////////////////////////
//	DEFINES

// Temporary file name used for saving a resource file.
#define RESFILE_TEMPNAME "rf$$$$$.res"

// Header defines.
#define RESFILE_SIGNATURE "RESF"

#define RESFILE_RAWLUMP   0

// Max absolute filename length
#define MAX_PATH 256

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ENUMS

// Resource file lump types.
enum LumpType
{
	RAW,
	TEXT,
	IMAGE,
	MESH,
	AUDIO,
	ResourceTypeEnd
};


///////////////////////////////////////////////////////////////////////////////
//	STRUCTURES

// Pack structures on byte alignment.
#pragma pack(push,1)

typedef struct ResourceHeader
{
	char Signature[4];				// 4-character identification value.
	unsigned char Version;          // Resource version.
	unsigned short Flags;         	// Special flags for the resource file.
	unsigned long NumLumps;			// Number of lumps in the resource file.
	unsigned long Checksum;			// Checksum (0FFFFFFFFh ^ file size).
	unsigned long LumpListOffset;	// Offset of the lump info list.
} ResourceHeader, *pResourceHeader;

// Structure of each element in the lump info list. Holds information which
// can be used to retrieve the contents of a lump.
typedef struct ResourceLumpInfo
{
	unsigned long Size;              // Size of the lump, in bytes.
	unsigned long Offset;            // Offset in the file of the lump.
	LumpType Type;                   // The type of lump.
	unsigned char NumChar;           // Length in characters of the lump's name.

	// ... The lump's name string goes here, but it can be of variable length;
	// therefore, we manually load it in to memory pointed to by 'lpName' in
	// the RESFILELUMP structure.

} ResourceLumpInfo, *pResourceLumpInfo;
// Restore structure packing alignment value.
#pragma pack(pop)

// Structure used to keep track of a lump in memory.
typedef struct ResourceLump
{
	unsigned long Size;             // Size of the lump, in bytes.
	unsigned long Offset;           // Offset in the file of the lump.
	LumpType Type;					// The type of lump.
	bool bNoFree;                   // Temporary flags.
	_Null_terminated_ char * pName; // Pointer to the name of the lump.
	void * pData;                   // Pointer to the data of the lump.
	void ** pDataAddress;           // Pointer to the data address variable.
	ResourceLump *pNextLump;       // Pointer to the next lump in the list.
} ResourceLump, *pResourceLump;



class Resource
{
protected:
	// Internal structure used to keep track of a lump handler.
	typedef struct ResourceLumpHandler
	{
		LumpType Type;
		bool (*Load)(std::fstream *, pResourceLump);
		bool (*Save)(std::fstream *, pResourceLump);
		void (*Unload)(pResourceLump);
		ResourceLumpHandler *pNextHandler;
	} ResourceLumpHandler, *pResourceLumpHandler;

	static bool bHandlerActive;
	// List of all lump handlers
	static pResourceLumpHandler pLumpHandlerList;

	ResourceHeader resFileHeader; // Header structure for the resource file.
	pResourceLump pLumpList;      // Pointer to the root of the lump linked list.

	char cFileName[MAX_PATH];      // Local storage of the resource file's name.
	char cFileMode[4];             // Current file access mode.
	std::fstream fStream;          // File stream object.
	std::fstream fSaveStream;      // Temporary file stream object for save operations.

									 // Private internal methods.
	long GetFileSize(void);
	pResourceLumpHandler GetLumpHandler(unsigned long);
	bool LoadLumpList(void);
	bool SaveLumpList(void);
	void UnloadLumpList(void);
	bool SaveLumps(void);

	static void Shutdown(void);

public:

	//- Constructor --------------------------------------------------------------
	// Description: Default constructor of this class - clears out data structures.
	//----------------------------------------------------------------------------
	Resource();

	//- Constructor --------------------------------------------------------------
	// Description: See the Resource::Open() method description.
	//----------------------------------------------------------------------------
	Resource(const char * aFileName, const char * aFileMode)
	{
		this->Open(aFileName, aFileMode);
	}

	//- Open ---------------------------------------------------------------------
	// Description: Opens a resource file either for reading from, writing to, or
	//              modification. Read mode opens up a resource file for read only
	//              operations, and in this mode, the resource file can not be
	//              modified. Write mode creates a new resource file (or overwrites
	//              any existing file with the same name) and allows the programmer
	//              to add lumps to the file. Modification mode opens up an existing
	//              file (or creates one if one doesn't exist) and allows the
	//              programmer to add more lumps to the file or to load in the lumps.
	// Parameters:  aFileName - name of the resource file (may include path) to open.
	//              aFileMode - string containing the access mode, can be of the
	//                           following contents:
	//                          "r"  - Read Mode
	//							"w"  - Write Mode
	//							"r+" - Modification Mode
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool Open(const char * aFileName, const char * aFileMode);

	//- Save ---------------------------------------------------------------------
	// Description: Saves the contents of a resource file in memory to disk. If
	//              for some reason you want to save the file with an alternate
	//              file name (ie. SaveAs operations), then pass the new file name
	//              string pointer in the "lpAltFileName" parameter. Otherwise,
	//              you can just pass NULL for this parameter to keep the original
	//              file name.
	// Parameters:  aAltFileName - alternate file name.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool Save(const char * aAltFileName);

	//- Close --------------------------------------------------------------------
	// Description: Closes the resource file if one is currently open.
	//----------------------------------------------------------------------------
	void Close(void);

	//- RegisterLumpHandler ------------------------------------------------------
	// Description: Registers a lump handler with the Resource class. A lump handler
	//              consists of three functions, one each for loading, saving, and
	//              unloading a lump of a particular type. You must also pass the
	//              value which will used to identify whether lumps should be
	//              handled by the lump handler or not - each value must be unique.
	// Parameters:  aType   - the type of lumps this handler will handle.
	//              aLoad   - pointer to the loading function.
	//              aSave   - pointer to the saving function.
	//              aUnload - pointer to the unloading function.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	static bool RegisterLumpHandler(LumpType aType, bool (*aLoad)(std::fstream *, pResourceLump), bool (*aSave)(std::fstream *, pResourceLump), void (*aUnload)(pResourceLump));

	//- RemoveLumpHandler --------------------------------------------------------
	// Description: Removes a lump handler previously added by a message to the
	//              Resource::RegisterLumpHandler() method.
	// Parameters:  aType - the lump type of the handler to remove.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	static bool RemoveLumpHandler(unsigned long aType);

	//- LumpExists ---------------------------------------------------------------
	// Description: Checks to see if a lump, with a particular name, exists.
	// Parameters:  aName - string of the lump to check for existence.
	// Returns:     TRUE if it exists, FALSE if it doesn't.
	//----------------------------------------------------------------------------
	bool LumpExists(const char * aName);

	//- CreateLump ---------------------------------------------------------------
	// Description: Creates a new lump and adds it to the active resource file.
	//              Note that it will not be saved with the file unless the
	//              Resource::Save() method is messaged.
	// Parameters:  aName - Name of the lump.
	//              aType - Particular type of the lump (used for loading/saving)
	//              aData - Pointer to the data or data structure that will be
	//                       stored in the lump.
	//              aSize - Size of the data in bytes (used only for RAW data lumps).
	//              aFree  - Set this to TRUE if you want the data (pointed to by
	//                       aData) to be deallocated when the file is closed.
	// Returns:     TRUE is successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool CreateLump(const char * aName, LumpType aType, void * aData, unsigned long aSize, bool aFree = false);

	//- DeleteLump ---------------------------------------------------------------
	// Description: Removes or deletes a particular lump, designated by 'aName',
	//              from the active resource file.
	// Parameters:  aName - name of the lump to remove from the file.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool DeleteLump(const char * aName);

	//- LoadLump -----------------------------------------------------------------
	// Description: Loads in a lump from a resource file. Depending upon it's
	//              type, it will used the specially designed routine for loading
	//              it in. However, if such a routine doesn't exist, it will
	//              default the lump as raw data.
	// Parameters:  aName   - name of the lump to load in.
	//              aDataAddress - pointer to the location where the address of the data
	//                         or data structure will be stored.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool LoadLump(const char * aName, void **aDataAddress);

	//- UnloadLump ---------------------------------------------------------------
	// Description: Unloads a lump from memory that was previously loaded in from
	//              a resource file using the Resource::LoadLump() method.
	// Parameters:  aName - name of the lump to unload from memory.
	// Returns:     TRUE if successful, FALSE otherwise.
	//----------------------------------------------------------------------------
	bool UnloadLump(const char * aName);

	//- Destructor ---------------------------------------------------------------
	// Description: Deallocates any memory and closes the resource file if it
	//              is still open.
	//----------------------------------------------------------------------------
	~Resource();
};