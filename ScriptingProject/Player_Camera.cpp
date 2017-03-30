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
#include "../SDL/include/SDL_scancode.h"
#include "../PhysBody3D.h"
#include "../ComponentCollider.h"
#include "../ModuleGOManager.h"
#include "../ComponentCamera.h"
#include "../Globals.h"

namespace Player_Camera
{
	float Player_Camera_distrance_y = 0.0;
	float Player_Camera_distrance_z = 0.0;
	GameObject* Player_Camera_target = nullptr;
	float Player_Camera_inclination_separation_y = 0.0;
	float Player_Camera_inclination_separation_z = 0.0;

	void Player_Camera_GetPublics(map<const char*, string>* public_chars, map<const char*, int>* public_ints, map<const char*, float>* public_float, map<const char*, bool>* public_bools, map<const char*, GameObject*>* public_gos)
	{
		public_float->insert(pair<const char*, float>("dist_y", Player_Camera_distrance_y));
		public_float->insert(pair<const char*, float>("dist_z", Player_Camera_distrance_z));
		public_gos->insert(pair<const char*, GameObject*>("target", nullptr));
		public_float->insert(pair<const char*, float>("inclination separation max and min y", Player_Camera_inclination_separation_y));
		public_float->insert(pair<const char*, float>("inclination separation max and min z", Player_Camera_inclination_separation_z));
	}

	void Player_Camera_UpdatePublics(GameObject* game_object)
	{
		ComponentScript* Player_Camera_script = (ComponentScript*)game_object->GetComponent(ComponentType::C_SCRIPT);

		Player_Camera_distrance_y = Player_Camera_script->public_floats.at("dist_y");
		Player_Camera_distrance_z = Player_Camera_script->public_floats.at("dist_z");
		Player_Camera_target = Player_Camera_script->public_gos.at("target");
		Player_Camera_inclination_separation_y = Player_Camera_script->public_floats.at("inclination separation max and min y");
		Player_Camera_inclination_separation_z = Player_Camera_script->public_floats.at("inclination separation max and min z");
	}

	void Player_Camera_Start(GameObject* game_object)
	{
	}

	void Player_Camera_Update(GameObject* game_object)
	{
		Quat Player_Camera_rot = game_object->transform->GetRotation();
		float3 Player_Camera_euler_rot = game_object->transform->GetRotationEuler();
		game_object->transform->SetRotation(float3(0.0, Player_Camera_euler_rot.y, 0.0));

		if (Player_Camera_target != nullptr)
		{
			game_object->transform->SetRotation(float3(0.0, Player_Camera_target->transform->GetRotationEuler().y, 0.0));

			float rotation_x = Player_Camera_target->transform->GetRotationEuler().x;
			if (rotation_x > 90)
				rotation_x = 90;
			else if (rotation_x < -90)
				rotation_x = -90;

			float3 Player_Camera_target_pos = float3::zero;
			Player_Camera_target_pos += (float3(game_object->transform->GetForward().Normalized().x, 0.0, game_object->transform->GetForward().Normalized().z) * (Player_Camera_distrance_z + rotation_x / 90 * Player_Camera_inclination_separation_z));
			Player_Camera_target_pos += (float3(0.0, Player_Camera_distrance_y + rotation_x / 90 * Player_Camera_inclination_separation_y, 0.0));

			Player_Camera_target_pos += Player_Camera_target->transform->GetPosition();

			game_object->transform->SetPosition(Player_Camera_target_pos);

			ComponentCamera* Player_Camera_cam = (ComponentCamera*)game_object->GetComponent(ComponentType::C_CAMERA);
			if (Player_Camera_cam != nullptr)
			{
				Player_Camera_cam->LookAt(Player_Camera_target->transform->GetPosition());
			}
		}
	}
}