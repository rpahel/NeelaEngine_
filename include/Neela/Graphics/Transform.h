#pragma once

#include "Neela/Essentials/Export.h"
#include "Neela/Maths/Vector.h"
#include "Neela/Maths/Matrix.h"
#include "Neela/Interfaces/IPrintable.h"
#include "Neela/Interfaces/ISerializeable.h"
#include "Neela/Interfaces/IInspectable.h"

#include <vector>

namespace Neela
{
	class NEELA_ENGINE_API Transform : public IPrintable, public ISerializeable, public IInspectable
	{
	private:
		Transform* m_Parent;
		std::vector<Transform*> m_Children;
		Vector2f m_RelativePosition;
		Vector2f m_RelativeScale;
		float m_RelativeRotation; // Stored in radians

		uint32_t m_Id = 0;
		uint32_t m_ParentId = 0;

	public:
		//==== Constructors ====

		Transform();
		explicit Transform(Transform* parent);
		explicit Transform(const Vector2f& relativePosition, float rotationDeg = 0, const Vector2f& relativeScale = Vector2f(1));
		explicit Transform(Transform* parent, const Vector2f& relativePosition, float relativeRotationDeg = 0, const Vector2f& relativeScale = Vector2f(1));
		explicit Transform(const Transform&) = delete;
		Transform(Transform&& victim) noexcept;
		~Transform();

		//==== Methods ====

		uint32_t		GetId() const;
		uint32_t		GetParentId() const;

		void			SetId(uint32_t newId);
		void			SetParentId(uint32_t newId);

		void			SetParent(Transform* parent);
		Transform*		GetParent() const;

		void			AddChild(Transform* newChild);
		void			RemoveChild(Transform* newChild);
		bool			HasChild(Transform* childCandidate) const;
		bool			IsChildOf(Transform* parentCandidate) const;
		Transform*		GetChildAt(int index) const;
		uint32_t		GetChildCount() const;

		void			SetRelativePosition(const Vector2f& newPosition);
		void			RelativeTranslate(const Vector2f& deltaPosition);
		const Vector2f& GetRelativePosition() const;

		void			SetRelativeScale(const Vector2f& newScale);
		const Vector2f& GetRelativeScale() const;

		void			SetRelativeRotationDeg(float newRotation);
		void			RotateRad(float deltaRotation);
		void			RotateDeg(float deltaRotation);
		float			GetRelativeRotationDeg() const;

		void			SetRelativeRotationRad(float newRotation);
		float			GetRelativeRotationRad() const;

		void			SetAbsolutePosition(const Vector2f& newPosition);
		void			AbsoluteTranslate(const Vector2f& deltaPosition);
		Vector2f		GetAbsolutePosition() const;

		void			SetAbsoluteScale(const Vector2f& newScale);
		Vector2f		GetAbsoluteScale() const;

		void			SetAbsoluteRotationDeg(float newRotation);
		void			SetAbsoluteRotationRad(float newRotation);
		float			GetAbsoluteRotationDeg() const;
		float			GetAbsoluteRotationRad() const;

		Matrixf			GetRelativePositionMatrix()	 const;
		Matrixf			GetRelativeRotationMatrix()	 const;
		Matrixf			GetRelativeScaleMatrix()	 const;
		Matrixf			GetTransformMatrix() const;

		Vector2f		RelativeToAbsolutePosition(const Vector2f& position) const;
		Vector2f		AbsoluteToRelativePosition(const Vector2f& position) const;
		Vector2f		GetUp()	const;
		Vector2f		GetRight()	const;

		//==== IPrintable ====

		const std::string& ToString() const override;

		//==== ISerializeable ====
		
		void Serialize(std::vector<uint8_t>& byteArray) const override;
		void Serialize(nlohmann::ordered_json& json) const override;

		bool Deserialize(std::vector<uint8_t>& byteArray, std::size_t& offset);
		bool Deserialize(nlohmann::ordered_json& json);

		//==== IInspectable ====

		void PopulateInspector() override;

		//==== Operators ====

		Transform& operator=(const Transform&) = delete;
		Transform& operator=(Transform&& victim) noexcept;

	private:
		//==== Methods ====

		Matrixf	RelativeToAbsolute(const Matrixf& other) const;
		Matrixf	AbsoluteToRelative(const Matrixf& other) const;
		Matrixf	RelativeToAbsoluteMatrix() const;
		Matrixf	AbsoluteToRelativeMatrix() const;
	};
}