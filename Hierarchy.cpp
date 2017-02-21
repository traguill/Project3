#include "Hierarchy.h"
#include "Application.h"
#include "Editor.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"

#include "ModuleGOManager.h"
#include "LayerSystem.h"

Hierarchy::Hierarchy()
{
	active = true;
}
Hierarchy::~Hierarchy()
{}

void Hierarchy::Draw(ImGuiWindowFlags flags)
{
	if (!active) return;

	ImGui::SetNextWindowSize(current_size);
	ImGui::SetNextWindowPos(current_position);

	ImGui::Begin("Hierarchy", &active, flags);
	
	DisplayGameObjectsChilds(App->go_manager->root->GetChilds());

	if (ImGui::IsMouseHoveringWindow())
	{
		if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("HierarchyOptions");
		}
		if (ImGui::IsMouseClicked(0))// && been_selected == false)
		{
			if (setting_parent == true)
			{
				if (App->editor->selected.size() > 0)
				{
					App->editor->selected.back()->SetParent(App->go_manager->root);
				}
			}
		}
	}

	if (ImGui::BeginPopup("HierarchyOptions"))
	{
		if (ImGui::Selectable("Copy"))
		{
			if (App->editor->selected.size() > 0)
				App->editor->Copy(App->editor->selected.back());
		}
		if (ImGui::Selectable("Paste"))
		{
			if (App->editor->selected.size() > 0)
				App->editor->Paste(App->editor->selected.back());
			else
				App->editor->Paste(nullptr);
		}
		ImGui::Separator();
		if (ImGui::Selectable("Duplicate"))
		{
			if (App->editor->selected.size() > 0)
				App->editor->Duplicate(App->editor->selected.back());
		}

		if (ImGui::Selectable("Delete"))
		{
			App->editor->RemoveSelected();
		}
		ImGui::Separator();
		if (ImGui::Selectable("Create Empty"))
		{
			GameObject* game_object = (App->editor->selected.size() > 0) ? App->editor->selected.back() : nullptr;
			App->editor->SelectSingle(App->go_manager->CreateGameObject(game_object));
		}

		if (ImGui::Selectable("Create Prefab"))
		{
			if (App->editor->selected.size() > 0)
			{
				App->resource_manager->SavePrefab(App->editor->selected.back());
			}
		}

		if (ImGui::Selectable("Set Parent"))
		{
			setting_parent = true;
		}

		ImGui::EndPopup();
	}
	
	// Quick test for unselecting game objects when not selecting none
	int size_x = current_size.x;
	int size_y = current_size.y - ImGui::GetCursorPosY() - 10;
	ImGui::SetCursorPosX(0);
	if (ImGui::Button("test: unselect GO", ImVec2(size_x, size_y)))
	{
		App->editor->UnselectAll();
	}
	
	ImGui::End();
}

void Hierarchy::DisplayGameObjectsChilds(const std::vector<GameObject*>* childs)
{
	for (std::vector<GameObject*>::const_iterator object = (*childs).begin(); object != (*childs).end(); ++object)
	{
		uint flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (App->editor->IsSelected(*object))
			flags |= ImGuiTreeNodeFlags_Selected;

		if ((*object)->ChildCount() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool open = ImGui::TreeNodeEx((*object)->name.data(), flags);
		bool hasChilds = (*object)->ChildCount() > 0;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			ComponentTransform* transform = (ComponentTransform*)(*object)->GetComponent(C_TRANSFORM);
			App->camera->Center(transform->GetGlobalMatrix().TranslatePart());
		}

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
		{
			if (setting_parent == true)
			{
				if (App->editor->selected.size() > 0)
				{
					App->editor->selected.back()->SetParent(*object);
				}
				setting_parent = false;
			}
			else
			{
				OnClickSelect(*object);
			}
		}

		if (hasChilds > 0 && open)
		{
			DisplayGameObjectsChilds((*object)->GetChilds());
			ImGui::TreePop();
		}
	}
}

void Hierarchy::OnClickSelect(GameObject* game_object)
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
	{
		if (App->editor->IsSelected(game_object))
		{
			App->editor->Unselect(game_object);
		}
		else
		{
			App->editor->AddSelect(game_object);
		}
	}
	else
	{
		App->editor->SelectSingle(game_object);
	}
}