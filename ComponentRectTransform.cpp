#include "Application.h"
#include "ComponentRectTransform.h"
#include "ModuleResourceManager.h"
#include "ResourceFileMesh.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "imgui\imgui.h"
#include "ModuleRenderer3D.h"

#include <stack>

ComponentRectTransform::ComponentRectTransform(ComponentType type, GameObject* game_object) : Component(type, game_object)
{
	local_position.Set(100.0f, 100.0f, 0.0f);
	global_position.Set(0.0f, 0.0f, 0.0f);
	rect_size.Set(100.0f, 100.0f);
	size.Set(1.0f, 1.0f,1.0f);
	transform_matrix = transform_matrix.FromTRS(local_position, Quat::identity, size);
	CalculateFinalTransform();
	GeneratePlane();
}

void ComponentRectTransform::GeneratePlane()
{
	std::string prim_path = "Resources/Primitives/2147000003.msh";
	ResourceFileMesh* tmp_rc = (ResourceFileMesh*)App->resource_manager->LoadResource(prim_path, ResourceFileType::RES_MESH);
	plane = new ResourceFileMesh(*tmp_rc);
	plane->mesh = new Mesh();
	plane->mesh->num_vertices = 4;
	plane->mesh->vertices = new float[plane->mesh->num_vertices * 3];
	plane->mesh->num_indices = 6;
	plane->mesh->indices = new uint[plane->mesh->num_indices];
	plane->mesh->num_uvs = 4;
	plane->mesh->uvs = new float[plane->mesh->num_uvs * 2];
	ResizePlane();

}



void ComponentRectTransform::Update(float dt)
{
	if (apply_transformation)
	{
		transform_matrix = transform_matrix.FromTRS(local_position, Quat::identity, size);
		CalculateFinalTransform();
		ResizePlane();
		apply_transformation = false;
	}

	//App->renderer3D->DrawUI(game_object);
}

void ComponentRectTransform::OnInspector(bool debug)
{
	string str = (string("Rect Transform") + string("##") + std::to_string(uuid));
	if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("delete##rectTransform");
		}

		if (ImGui::BeginPopup("delete##rectTransform"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				Remove();
			}
			ImGui::EndPopup();
		}

		if (ImGui::DragFloat2("Position ##pos", local_position.ptr(),1.000f))
		{
			apply_transformation = true;
		}
		if (ImGui::DragFloat2("Rect ##rect", rect_size.ptr(), 1.000f))
		{
			apply_transformation = true;
		}

		if (ImGui::DragFloat2("Size ##size", size.ptr(), 1.000f))
		{
			apply_transformation = true;
		}
	}
}

void ComponentRectTransform::CalculateFinalTransform()
{
	GameObject* game_object = GetGameObject();
	if (game_object)
	{
		if (game_object->GetParent())
		{
			ComponentRectTransform* parent_transform = (ComponentRectTransform*)game_object->GetParent()->GetComponent(C_RECT_TRANSFORM);
			
			if (parent_transform != nullptr) 
			{
				final_transform_matrix = parent_transform->final_transform_matrix * transform_matrix;

				std::vector<GameObject*>::const_iterator go_childs = game_object->GetChilds()->begin();
				for (go_childs; go_childs != game_object->GetChilds()->end(); ++go_childs)
				{
					ComponentRectTransform* transform = (ComponentRectTransform*)(*go_childs)->GetComponent(C_RECT_TRANSFORM);
					if (transform)
					{
						transform->CalculateFinalTransform();
					}
				}
			}
			else
			{
				final_transform_matrix = transform_matrix;
			}
		}
		else
		{
			final_transform_matrix = transform_matrix;
		}
	}
	else
	{
		LOG("Error: Component created but not attached to any gameobject");
	}
}

void ComponentRectTransform::OnTransformModified()
{
	apply_transformation = true;
}

math::float4x4 ComponentRectTransform::GetFinalTransform()
{
	return final_transform_matrix;
}


void ComponentRectTransform::Move(const math::vec &movement)
{
	local_position += movement;
	apply_transformation = true;
}

Mesh * ComponentRectTransform::GetMesh()
{
	return plane->mesh;
}

bool ComponentRectTransform::ContainsPoint(const math::float2 &point) const
{
	if (point.x < global_position.x || point.x > global_position.x + size.x || point.y < global_position.y || point.y > global_position.y + size.y)
		return false;
	return true;
}


const math::vec &ComponentRectTransform::GetLocalPos() const
{
	return local_position;
}

void ComponentRectTransform::SetSize(const float2 & size)
{
	rect_size = size;
}

float2 ComponentRectTransform::GetRectSize() const
{
	return rect_size;
}

void ComponentRectTransform::ResizePlane()
{
	plane->mesh->vertices[0] = 0.0f;
	plane->mesh->vertices[1] = 0.0f;
	plane->mesh->vertices[2] = 0.0f;
	plane->mesh->vertices[3] = rect_size.x;
	plane->mesh->vertices[4] = 0.0f;
	plane->mesh->vertices[5] = 0.0f;
	plane->mesh->vertices[6] = 0.0f;
	plane->mesh->vertices[7] = rect_size.y;
	plane->mesh->vertices[8] = 0.0f;
	plane->mesh->vertices[9] = rect_size.x;
	plane->mesh->vertices[10] = rect_size.y;
	plane->mesh->vertices[11] = 0.0f;

	
	plane->mesh->indices[0] = 0;
	plane->mesh->indices[1] = 2;
	plane->mesh->indices[2] = 1;
	plane->mesh->indices[3] = 1;
	plane->mesh->indices[4] = 2;
	plane->mesh->indices[5] = 3;

	

	plane->mesh->uvs[0] = 0.0f;
	plane->mesh->uvs[1] = 1.0f;
	plane->mesh->uvs[2] = 1.0f;
	plane->mesh->uvs[3] = 1.0f;
	plane->mesh->uvs[4] = 0.0f;
	plane->mesh->uvs[5] = 0.0f;
	plane->mesh->uvs[6] = 1.0f;
	plane->mesh->uvs[7] = 0.0f;

	plane->ReLoadInMemory();
}

void ComponentRectTransform::Save(Data & file) const
{
	Data data;
	data.AppendInt("type", type);
	data.AppendUInt("UUID", uuid);
	data.AppendBool("active", active);
	data.AppendFloat2("rect_size", rect_size.ptr());
	data.AppendMatrix("matrix", transform_matrix);

	file.AppendArrayValue(data);
}

void ComponentRectTransform::Load(Data & conf)
{
	uuid = conf.GetUInt("UUID");
	active = conf.GetBool("active");

	transform_matrix = conf.GetMatrix("matrix");
	rect_size = conf.GetFloat2("rect_size");
	local_position = transform_matrix.TranslatePart();
	size = transform_matrix.GetScale();
	ResizePlane();
	CalculateFinalTransform();
}

void ComponentRectTransform::Remove()
{
	Component* u = (Component*)game_object->GetComponent(C_UI_IMAGE);
	if(u != nullptr)
		game_object->RemoveComponent(u);
	
	Component* c = (Component*)game_object->GetComponent(C_CANVAS);
	if (c != nullptr)
		game_object->RemoveComponent(c);

	game_object->RemoveComponent(this);
}


