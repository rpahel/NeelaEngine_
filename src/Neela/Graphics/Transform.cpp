#include "Neela/Graphics/Transform.h"
#include "Neela/Maths/Maths.h"

#define TRANSFORM_SERIALIZE_VERSION 1

namespace Neela
{
	//=====================================================================================================
	//==== PUBLIC CONSTRUCTORS
	//=====================================================================================================

	Transform::Transform() : Transform(nullptr, Vector2f(0), 0, Vector2f(1)) {}

	Transform::Transform(Transform* parent) : Transform(parent, Vector2f(0), 0, Vector2f(1)) {}

	Transform::Transform(const Vector2f& relativePosition, float rotationDeg, const Vector2f& relativeScale) : Transform(nullptr, relativePosition, rotationDeg, relativeScale) {}

	Transform::Transform(Transform* parent, const Vector2f& relativePosition, float relativeRotationDeg, const Vector2f& relativeScale) :
		m_Parent(parent),
		m_RelativePosition(relativePosition),
		m_RelativeRotation(relativeRotationDeg * Maths::DEG2RAD),
		m_RelativeScale(relativeScale)
	{
		if (m_Parent)
		{
			m_Parent->AddChild(this);
			m_ParentId = m_Parent->m_Id;
		}

		m_Id = Maths::RandomInt() + Maths::RandomInt();
	}

	Transform::Transform(Transform&& victim) noexcept :
		m_Parent(victim.m_Parent),
		m_Children(victim.m_Children),
		m_RelativePosition(victim.m_RelativePosition),
		m_RelativeRotation(victim.m_RelativeRotation),
		m_RelativeScale(victim.m_RelativeScale),
		m_Id(victim.m_Id),
		m_ParentId(victim.m_ParentId)
	{
		victim.m_Parent = nullptr;
		victim.m_Children.clear();
	}

	Transform::~Transform()
	{
		if (m_Parent)
			m_Parent->RemoveChild(this);

		if (!m_Children.empty())
		{
			for (Transform* child : m_Children)
			{
				child->SetParent(nullptr);
			}

			m_Children.clear();
		}
	}

	//=====================================================================================================
	//==== PUBLIC METHODS
	//=====================================================================================================

	uint32_t Transform::GetId() const
	{
		return m_Id;
	}

	uint32_t Transform::GetParentId() const
	{
		return m_ParentId;
	}

	void Transform::SetId(uint32_t newId)
	{
		m_Id = newId;
	}

	void Transform::SetParentId(uint32_t newId)
	{
		m_ParentId = newId;
	}

	void Transform::SetParent(Transform* parent)
	{
		if (parent == m_Parent)
			return;

		if (m_Parent)
			m_Parent->RemoveChild(this);

		m_Parent = parent;

		if (m_Parent)
		{
			m_Parent->AddChild(this);
			m_ParentId = m_Parent->m_Id;
		}
		else
		{
			m_ParentId = 0;
		}
	}

	Transform* Transform::GetParent() const
	{
		return m_Parent;
	}

	void Transform::AddChild(Transform* newChild)
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); it++)
		{
			if (*it == newChild)
				return;
		}

		m_Children.push_back(newChild);
	}

	void Transform::RemoveChild(Transform* newChild)
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); it++)
		{
			if (*it == newChild)
			{
				m_Children.erase(it);
				return;
			}
		}
	}

	bool Transform::HasChild(Transform* childCandidate) const
	{
		if (m_Children.empty())
			return false;

		for (auto it = m_Children.begin(); it != m_Children.end(); it++)
		{
			if (*it == childCandidate)
				return true;
		}

		return false;
	}

	bool Transform::IsChildOf(Transform* parentCandidate) const
	{
		return m_Parent == parentCandidate;
	}

	Transform* Transform::GetChildAt(int index) const
	{
		if (index >= m_Children.size())
			return nullptr;

		return m_Children[index];
	}

	uint32_t Transform::GetChildCount() const
	{
		return m_Children.size();
	}

	void Transform::SetRelativePosition(const Vector2f& newPosition)
	{
		m_RelativePosition = newPosition;
	}

	void Transform::RelativeTranslate(const Vector2f& deltaPosition)
	{
		m_RelativePosition += deltaPosition;
	}

	const Vector2f& Transform::GetRelativePosition() const
	{
		return m_RelativePosition;
	}

	void Transform::SetRelativeScale(const Vector2f& newScale)
	{
		m_RelativeScale = newScale;
	}

	const Vector2f& Transform::GetRelativeScale() const
	{
		return m_RelativeScale;
	}

	void Transform::SetRelativeRotationDeg(float newRotation)
	{
		m_RelativeRotation = newRotation * Maths::DEG2RAD;
	}

	void Transform::RotateRad(float deltaRotation)
	{
		m_RelativeRotation += deltaRotation;
	}

	void Transform::RotateDeg(float deltaRotation)
	{
		m_RelativeRotation += deltaRotation * Maths::DEG2RAD;
	}

	float Transform::GetRelativeRotationDeg() const
	{
		return m_RelativeRotation * Maths::RAD2DEG;
	}

	void Transform::SetRelativeRotationRad(float newRotation)
	{
		m_RelativeRotation = newRotation;
	}

	float Transform::GetRelativeRotationRad() const
	{
		return m_RelativeRotation;
	}

	void Transform::SetAbsolutePosition(const Vector2f& newPosition)
	{
		if (m_Parent)
		{
			m_RelativePosition = m_Parent->AbsoluteToRelativePosition(newPosition);
			return;
		}

		m_RelativePosition = newPosition;
	}

	void Transform::AbsoluteTranslate(const Vector2f& deltaPosition)
	{
		if (m_Parent)
		{
			m_RelativePosition = m_Parent->AbsoluteToRelativePosition(GetAbsolutePosition() + deltaPosition);
			return;
		}

		m_RelativePosition += deltaPosition;
	}

	Vector2f Transform::GetAbsolutePosition() const
	{
		if (m_Parent)
			return m_Parent->RelativeToAbsolutePosition(m_RelativePosition);

		return m_RelativePosition;
	}

	void Transform::SetAbsoluteScale(const Vector2f& newScale)
	{
		if (m_Parent)
		{
			m_RelativeScale = Vector2f::Divide(newScale, m_Parent->GetAbsoluteScale());
			return;
		}

		m_RelativeScale = newScale;
	}

	Vector2f Transform::GetAbsoluteScale() const
	{
		if (m_Parent)
			return m_Parent->GetAbsoluteScale() * m_RelativeScale;

		return m_RelativeScale;
	}

	void Transform::SetAbsoluteRotationDeg(float newRotation)
	{
		if (m_Parent)
		{
			m_RelativeRotation = (newRotation - m_Parent->GetAbsoluteRotationDeg()) * Maths::DEG2RAD;
			return;
		}

		m_RelativeRotation = newRotation * Maths::DEG2RAD;
	}

	void Transform::SetAbsoluteRotationRad(float newRotation)
	{
		if (m_Parent)
		{
			m_RelativeRotation = newRotation - m_Parent->GetAbsoluteRotationRad();
			return;
		}

		m_RelativeRotation = newRotation;
	}

	float Transform::GetAbsoluteRotationDeg() const
	{
		if (m_Parent)
			return (m_Parent->GetAbsoluteRotationRad() + m_RelativeRotation) * Maths::RAD2DEG;

		return m_RelativeRotation * Maths::RAD2DEG;
	}

	float Transform::GetAbsoluteRotationRad() const
	{
		if (m_Parent)
			return m_Parent->GetAbsoluteRotationRad() + m_RelativeRotation;

		return m_RelativeRotation;
	}

	Matrixf Transform::GetRelativePositionMatrix() const
	{
		return Matrixf::MakeFromPosition(m_RelativePosition);
	}

	Matrixf Transform::GetRelativeRotationMatrix() const
	{
		return Matrixf::MakeFromRotation(m_RelativeRotation);
	}

	Matrixf Transform::GetRelativeScaleMatrix() const
	{
		return Matrixf::MakeFromScale(m_RelativeScale);
	}

	Matrixf Transform::GetTransformMatrix() const
	{
		return Matrixf::MakeFrom(m_RelativePosition, m_RelativeRotation, m_RelativeScale);
	}

	Vector2f Transform::RelativeToAbsolutePosition(const Vector2f& position) const
	{
		return RelativeToAbsolute(Matrixf::MakeFromPosition(position)).GetPosition();
	}

	Vector2f Transform::AbsoluteToRelativePosition(const Vector2f& position) const
	{
		return AbsoluteToRelative(Matrixf::MakeFromPosition(position)).GetPosition();
	}

	Vector2f Transform::GetUp() const
	{
		return Vector2f::Up().RotateRad(GetAbsoluteRotationRad());
	}

	Vector2f Transform::GetRight() const
	{
		return Vector2f::Right().RotateRad(GetAbsoluteRotationRad());
	}

	//=====================================================================================================
	//==== IPRINTABLE
	//=====================================================================================================

	const std::string& Transform::ToString() const
	{
		nlohmann::ordered_json json;
		Serialize(json);
		return json.dump(2);
	}

	//=====================================================================================================
	//==== ISERIALIZEABLE
	//=====================================================================================================

	void Transform::Serialize(std::vector<uint8_t>& byteArray) const
	{
		Serializer::Serialize<uint8_t>(byteArray, TRANSFORM_SERIALIZE_VERSION);

		GetRelativePosition().Serialize(byteArray);
		GetRelativeScale().Serialize(byteArray);
		Serializer::Serialize<float>(byteArray, GetRelativeRotationDeg());

		Serializer::Serialize<uint32_t>(byteArray, GetId());
		Serializer::Serialize<uint32_t>(byteArray, GetParentId());
	}

	void Transform::Serialize(nlohmann::ordered_json& json) const
	{
		GetRelativePosition().Serialize(json["Relative Position"]);
		GetRelativeScale().Serialize(json["Relative Scale"]);

		json["Relative Rotation"] = GetRelativeRotationDeg();

		json["ID"] = GetId();
		json["Parent ID"] = GetParentId();
	}

	bool Transform::Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset)
	{
		const uint8_t version = Serializer::Deserialize<uint8_t>(byteArray, offset);
		if (version > TRANSFORM_SERIALIZE_VERSION)
		{
			fmt::print(stderr, fg(fmt::color::red), "Transform::Deserialize() -> Transform has unsupported version {} (Current version is {}).\n", version, TRANSFORM_SERIALIZE_VERSION);
			return false;
		}

		Vector2f pos;
		pos.Deserialize(byteArray, offset);
		SetRelativePosition(pos);

		Vector2f scale;
		scale.Deserialize(byteArray, offset);
		SetRelativeScale(scale);

		SetRelativeRotationDeg(Serializer::Deserialize<float>(byteArray, offset));

		SetId(Serializer::Deserialize<uint32_t>(byteArray, offset));

		SetParentId(Serializer::Deserialize<uint32_t>(byteArray, offset));

		return true;
	}

	bool Transform::Deserialize(nlohmann::ordered_json& json)
	{
		Vector2f pos;
		pos.Deserialize(json["Relative Position"]);
		SetRelativePosition(pos);

		Vector2f scale;
		scale.Deserialize(json["Relative Scale"]);
		SetRelativeScale(scale);

		SetRelativeRotationDeg(json.value("Relative Rotation", 0.0F));

		SetId(json.value("ID", 0));

		SetParentId(json.value("Parent ID", 0));

		return true;
	}

	//=====================================================================================================
	//==== IINSPECTABLE
	//=====================================================================================================

	void Transform::PopulateInspector()
	{
		if (!ImGui::TreeNode("Transform"))
			return;

		float posArray[2] = { m_RelativePosition.X, m_RelativePosition.Y };
		if (ImGui::DragFloat2("Position", posArray, 1, 0, 0, "%.2f"))
			SetRelativePosition(Vector2f(posArray[0], posArray[1]));

		float rot = m_RelativeRotation;
		if (ImGui::SliderAngle("Rotation", &rot, 0, 360))
			SetRelativeRotationRad(rot);

		float scaleArray[2] = { m_RelativeScale.X, m_RelativeScale.Y };
		if (ImGui::DragFloat2("Scale", scaleArray, 0.01f, 0, 0, "%.2f"))
			SetRelativeScale(Vector2f(scaleArray[0], scaleArray[1]));

		if (ImGui::TreeNode("Transform matrix"))
		{
			Matrix transformMatrix = GetTransformMatrix();
			ImGui::Text("%.2f %.2f %.2f", transformMatrix[Vector2i(0, 0)], transformMatrix[Vector2i(1, 0)], transformMatrix[Vector2i(2, 0)]);
			ImGui::Text("%.2f %.2f %.2f", transformMatrix[Vector2i(0, 1)], transformMatrix[Vector2i(1, 1)], transformMatrix[Vector2i(2, 1)]);
			ImGui::Text("%.2f %.2f %.2f", transformMatrix[Vector2i(0, 2)], transformMatrix[Vector2i(1, 2)], transformMatrix[Vector2i(2, 2)]);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//=====================================================================================================
	//==== PUBLIC OPERATORS
	//=====================================================================================================

	Transform& Transform::operator=(Transform&& victim) noexcept
	{
		m_Parent = victim.m_Parent;
		m_Children = victim.m_Children;
		m_RelativePosition = victim.m_RelativePosition;
		m_RelativeRotation = victim.m_RelativeRotation;
		m_RelativeScale = victim.m_RelativeScale;
		m_Id = victim.m_Id;
		m_ParentId = victim.m_ParentId;

		victim.m_Parent = nullptr;
		victim.m_Children.clear();

		return *this;
	}

	//=====================================================================================================
	//==== PRIVATE METHODS
	//=====================================================================================================

	Matrixf Transform::RelativeToAbsolute(const Matrixf& other) const
	{
		return RelativeToAbsoluteMatrix() * other;
	}

	Matrixf Transform::AbsoluteToRelative(const Matrixf& other) const
	{
		return AbsoluteToRelativeMatrix() * other;
	}

	Matrixf Transform::RelativeToAbsoluteMatrix() const
	{
		return m_Parent ? m_Parent->RelativeToAbsoluteMatrix() * GetTransformMatrix() : GetTransformMatrix();
	}

	Matrixf Transform::AbsoluteToRelativeMatrix() const
	{
		return RelativeToAbsoluteMatrix().GetInverse();
	}
}