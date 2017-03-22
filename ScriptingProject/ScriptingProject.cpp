// ScriptingProject.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <string>
#include <map>
#include <vector>

#include "../Application.h"
#include "../ModuleScripting.h"
#include "../ModuleInput.h"
#include "../ModuleWindow.h"
#include "../GameObject.h"
#include "../ComponentScript.h"
#include "../ComponentTransform.h"

extern "C"
{
	namespace ScriptNames
	{
		void GetScriptNames(Application* engine_app, vector<const char*>* script_names)
		{
			App = engine_app;

			script_names->push_back("Test");
			script_names->push_back("Test2");
		}
	}


	namespace Test
	{
		string test_title;
		int test_int;
		int test_int2;

		void Test_GetPublics(map<const char*, string>* public_chars, map<const char*, int>* public_ints, map<const char*, float>* public_float, map<const char*, bool>* public_bools)
		{
			test_title = "Hello World from Script";
			test_int = 3;
			test_int2 = 5;


			public_chars->insert(pair<const char*, string>("Title", test_title));
		}

		void Test_UpdatePublics(GameObject* game_object)
		{
			ComponentScript* script = (ComponentScript*)game_object->GetComponent(ComponentType::C_SCRIPT);

			test_title = script->public_chars.at("Title");
		}

		void Test_Start(GameObject* game_object)
		{
		}

		void Test_Update(GameObject* game_object)
		{
			App->window->SetTitle(test_title.c_str());

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				float3 tmp_pos = game_object->transform->GetPosition();
				tmp_pos.z += 1;
				game_object->transform->SetPosition(tmp_pos);
			}
		}
	}

	namespace Test2
	{
		void Test2_Start(GameObject* game_object)
		{
		}

		void Test2_Update(GameObject* game_object)
		{
			App->window->SetTitle("Hello World from Script2");
		}
	}
}