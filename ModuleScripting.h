#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"
#include "Globals.h"
#include "ResourceScriptsLibrary.h"
#include <vector>
#include <string>

class ModuleScripting : public Module
{
public:
	ModuleScripting(const char* name, bool start_enabled = true);
	~ModuleScripting();

	bool Init(Data& config);
	bool Start();

	bool CleanUp();
	void SaveBeforeClosing(Data& data)const;

	DWORD GetError();
	void LoadScriptsLibrary();
	vector<const char*> GetScriptNamesList()const;
	void GetPublics(const char* script_name, map<const char*, string>* public_chars, map<const char*, int>* public_ints, map<const char*, float>* public_floats, map<const char*, bool>* public_bools, map<const char*, GameObject>* public_gos);

	bool resource_created;
	bool scripts_loaded;
	bool setting_go_var;

private:
	DWORD last_error = 0;

public:
	ResourceScriptsLibrary* scripts_lib;
};

#endif // !__MOUDLESCRIPTING_H__
