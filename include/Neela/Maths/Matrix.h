#pragma once

#include "Neela/Maths/Maths.h"
#include "Neela/Maths/Vector.h"
#include "Neela/Interfaces/IPrintable.h"

#include <cassert>
#include <vector>
#include <type_traits>
#include <stdexcept>

namespace Neela
{
	template<typename T>
	struct Matrix : IPrintable
	{
		int32_t m_Rows, m_Columns;
		std::vector<T> m_Matrix;

		//==== Constructors ====

		Matrix(int32_t depth);
		Matrix(int32_t rows, int32_t columns);
		Matrix(int32_t rows, int32_t columns, const std::vector<T>& matrix);

		//==== Methods ====

		bool		IsSquare() const;
		bool		IsIdentity() const;
		bool		IsOrthogonal() const;

		int32_t		GetColumns() const;
		int32_t		GetRows() const;
		int32_t		GetSize() const;
		int32_t		GetDepth() const;

		T			GetValueAt(const Vector2i& coords) const;
		T			GetValueAt(int32_t row, int32_t column) const;
		T			GetDeterminant() const;
		T			GetCofactor(int32_t cutRowIndex, int32_t cutColumnIndex) const;

		Matrix		GetTranspose() const;
		Matrix		GetSubMatrix(int32_t cutRowIndex, int32_t cutColumnIndex) const;
		Matrix		GetSubMatrix(const Vector2i& rowRange, const Vector2i& columnRange) const;
		Matrix		GetCofactorMatrix() const;
		Matrix		GetAdjugate() const;
		Matrix		GetInverse() const;
		Matrix		GetLeftSplit(int32_t splitColIndex) const;
		Matrix		GetRightSplit(int32_t splitColIndex) const;

		template<typename U>
		Matrix& Add(const Matrix<U>& other);

		template<typename U>
		Matrix& Substract(const Matrix<U>& other);

		template<typename U>
		Matrix& Multiply(const Matrix<U>& other);

		template<typename U>
		Matrix& Multiply(const Vector2<U>& vector);

		template<typename U>
		Matrix& Multiply(U scalar);

		Matrix& Transpose();
		Matrix& RowAdd(int32_t rowFrom, int32_t rowTo, T factor = 1);
		Matrix& RowMultiply(int32_t rowIndex, T scalar);
		Matrix& RowSwap(int32_t rowA, int32_t rowB);

		template<typename U>
		Matrix& Augment(const Matrix<U>& other);

		Matrix& Invert();

		Vector2<T>	GetPosition() const;

		//==== IPrintable ====

		const std::string& ToString() const override;

		//==== Static Methods ====

		static Matrix Identity(int32_t depth);

		template<typename U>
		static Matrix Add(const Matrix& A, const Matrix<U>& B);

		template<typename U>
		static Matrix Substract(const Matrix& A, const Matrix<U>& B);

		template<typename U>
		static Matrix Multiply(const Matrix& A, const Matrix<U>& B);

		template<typename U>
		static Matrix Multiply(const Matrix& A, const Vector2<U>& B);

		template<typename U>
		static Matrix Multiply(const Vector2<T>& A, const Matrix<U>& B);

		static Matrix Multiply(const Matrix& A, T scalar);

		template<typename U>
		static Matrix MakeFromPosition(const Vector2<U>& position);
		template<typename U>
		static Matrix MakeFromScale(const Vector2<U>& scale);
		static Matrix MakeFromRotation(float rotationRad);
		template<typename U, typename V>
		static Matrix MakeFrom(const Vector2<U>& position, float rotationRad, const Vector2<V>& scale);

		//==== Operators ====

		template<typename U>
		bool operator==(const Matrix<U>& other) const;

		template<typename U>
		bool operator!=(const Matrix<U>& other) const;

		T& operator[](uint32_t index);
		T& operator[](Vector2i coords);

		template<typename U>
		Matrix operator+(const Matrix<U>& other) const;

		template<typename U>
		Matrix operator-(const Matrix<U>& other) const;

		template<typename U>
		Matrix operator*(const Matrix<U>& other) const;

		template<typename U>
		Matrix operator*(const Vector2<U>& vector) const;

		Matrix operator*(T scalar) const;
		Matrix operator/(T scalar) const;

		template<typename U>
		Matrix& operator+=(const Matrix<U>& other);

		template<typename U>
		Matrix& operator-=(const Matrix<U>& other);

		template<typename U>
		Matrix& operator*=(const Matrix<U>& other);

		template<typename U>
		Matrix& operator*=(const Vector2<U>& vector);

		Matrix& operator*=(T scalar);
		Matrix& operator/=(T scalar);

		template<typename U>
		operator Matrix<U>() const;
	};

	using Matrixf = Matrix<float>;
	using Matrixd = Matrix<double>;
	//using Matrixi = Matrix<int>; Useless

	//====================================================================================
	//==== PUBLIC CONSTRUCTORS
	//====================================================================================

	template<typename T>
	inline Matrix<T>::Matrix(int32_t depth) : Matrix(depth, depth, std::vector<T>(depth* depth, 0)) {}

	template<typename T>
	inline Matrix<T>::Matrix(int32_t rows, int32_t columns) : Matrix(rows, columns, std::vector<T>(rows* columns, 0)) {}

	template<typename T>
	inline Matrix<T>::Matrix(int32_t rows, int32_t columns, const std::vector<T>& matrix)
	{
		static_assert(std::is_arithmetic_v<T>, "Matrix<T>::Matrix -> T is not arithmetic.");

		assert(rows >= 0);
		assert(columns >= 0);
		assert(matrix.size() == rows * columns);

		m_Rows = rows;
		m_Columns = columns;

		m_Matrix.clear();
		m_Matrix = matrix;
	}

	//====================================================================================
	//==== PUBLIC METHODS
	//====================================================================================

	template<typename T>
	inline bool Matrix<T>::IsSquare() const
	{
		if (m_Rows * m_Columns > 0 && m_Rows == m_Columns)
			return true;

		return false;
	}

	template<typename T>
	inline bool Matrix<T>::IsIdentity() const
	{
		if (!IsSquare())
			return false;

		int i = 0;
		for (uint32_t y = 0; y < m_Rows; y++)
		{
			for (uint32_t x = 0; x < m_Columns; x++)
			{
				if (y == x)
				{
					if (!Maths::Equals(m_Matrix[i], 1))
						return false;
				}
				else
				{
					if (!Maths::Equals(m_Matrix[i], 0))
						return false;
				}

				i++;
			}
		}

		return true;
	}

	template<typename T>
	inline bool Matrix<T>::IsOrthogonal() const
	{
		if (!IsSquare())
			return false;

		if (GetTranspose() == GetInverse())
			return true;

		return false;
	}

	template<typename T>
	inline int32_t Matrix<T>::GetColumns() const
	{
		return m_Columns;
	}

	template<typename T>
	inline int32_t Matrix<T>::GetRows() const
	{
		return m_Rows;
	}

	template<typename T>
	inline int32_t Matrix<T>::GetSize() const
	{
		return m_Rows * m_Columns;
	}

	template<typename T>
	inline int32_t Matrix<T>::GetDepth() const
	{
		assert(IsSquare());

		return m_Columns;
	}

	template<typename T>
	inline T Matrix<T>::GetValueAt(const Vector2i& coords) const
	{
		assert(coords.X >= 0 && coords.X < m_Columns);
		assert(coords.Y >= 0 && coords.Y < m_Rows);

		return m_Matrix[coords.Y * m_Columns + coords.X];
	}

	template<typename T>
	inline T Matrix<T>::GetValueAt(int32_t row, int32_t column) const
	{
		return GetValueAt(Vector2i(column, row));
	}

	template<typename T>
	inline T Matrix<T>::GetDeterminant() const
	{
		assert(IsSquare());

		if (m_Columns == 1)
			return m_Matrix[0];

		if (m_Columns == 2)
			return m_Matrix[0] * m_Matrix[3] - m_Matrix[1] * m_Matrix[2]; // ad - bc

		float det = 0;
		for (int32_t y = 0; y < m_Rows; y++)
			det += GetCofactor(y, 0) * GetValueAt(y, 0);

		return det;
	}

	template<typename T>
	inline T Matrix<T>::GetCofactor(int32_t cutRowIndex, int32_t cutColumnIndex) const
	{
		return Maths::Pow(-1, static_cast<float>(cutRowIndex + cutColumnIndex)) * GetSubMatrix(cutRowIndex, cutColumnIndex).GetDeterminant();
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetTranspose() const
	{
		Matrix retMatrix(m_Columns, m_Rows);

		int i = 0;
		for (size_t y = 0; y < m_Rows; y++)
		{
			for (size_t x = 0; x < m_Columns; x++)
			{
				retMatrix[Vector2i(y, x)] = m_Matrix[i];
				i++;
			}
		}

		return retMatrix;
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetSubMatrix(int32_t cutRowIndex, int32_t cutColumnIndex) const
	{
		assert(cutRowIndex >= 0 && cutRowIndex < m_Rows);
		assert(cutColumnIndex >= 0 && cutColumnIndex < m_Columns);
		assert(m_Matrix.size() == m_Rows * m_Columns);

		Matrix retMatrix(m_Rows - 1, m_Columns - 1);

		int32_t i = 0;
		for (int32_t y = 0; y < m_Rows; y++)
		{
			if (y == cutRowIndex)
				continue;

			for (int32_t x = 0; x < m_Columns; x++)
			{
				if (x == cutColumnIndex)
					continue;

				retMatrix[i] = GetValueAt(y, x);
				i++;
			}
		}

		return retMatrix;
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetSubMatrix(const Vector2i& rowRange, const Vector2i& columnRange) const
	{
		assert(rowRange.X >= 0 && rowRange.Y >= rowRange.X && rowRange.Y <= m_Rows);
		assert(columnRange.X >= 0 && columnRange.Y >= columnRange.X && columnRange.Y <= m_Columns);

		Matrix retMatrix(Maths::Abs(rowRange.X - rowRange.Y), Maths::Abs(columnRange.X - columnRange.Y));

		for (int32_t y = rowRange.X, r = 0; y < rowRange.Y; y++, r++)
			for (int32_t x = columnRange.X, c = 0; x < columnRange.Y; x++, c++)
				retMatrix[Vector2i(c, r)] = GetValueAt(y, x);

		return retMatrix;
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetCofactorMatrix() const
	{
		assert(IsSquare());

		Matrix retMatrix(GetDepth());

		uint32_t i = 0;
		for (uint32_t y = 0; y < m_Rows; y++)
		{
			for (uint32_t x = 0; x < m_Columns; x++)
			{
				retMatrix[i] = GetCofactor(y, x);
				i++;
			}
		}

		return retMatrix;
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetAdjugate() const
	{
		return GetCofactorMatrix().GetTranspose();
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetInverse() const
	{
		assert(IsSquare());
		assert(GetDeterminant() != 0);

		Matrix<T> retMatrix(*this);
		retMatrix.Augment(Matrix::Identity(GetDepth()));

		for (int32_t y = 0; y < m_Rows; y++)
		{
			int32_t highestRow = y;
			T highestValue = 0;
			for (int32_t row = y; row < m_Rows; row++)
			{
				T value = Maths::Abs(retMatrix[Vector2i(y, row)]);
				if (value > highestValue)
				{
					highestValue = value;
					highestRow = row;
				}
			}

			assert((highestValue != 0 || (y == m_Rows - 1)) && "Matrix<T>::GetInverse() -> Column of zeroes encountered.");

			retMatrix.RowSwap(y, highestRow);

			const T div = retMatrix.GetValueAt(Vector2i(y, y));
			if (div == 0)
				continue;

			retMatrix.RowMultiply(y, 1 / div);

			for (int32_t row = 0; row < m_Rows; row++)
			{
				if (row == y)
					continue;

				const T f = -retMatrix[Vector2i(y, row)];

				retMatrix.RowAdd(y, row, f);
			}
		}

		return retMatrix.GetRightSplit(static_cast<int32_t>(retMatrix.m_Columns * 0.5f));
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetLeftSplit(int32_t splitColIndex) const
	{
		assert(splitColIndex > 0 && splitColIndex < m_Columns);

		return GetSubMatrix(Vector2i(0, m_Rows), Vector2i(0, splitColIndex + 1));
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::GetRightSplit(int32_t splitColIndex) const
	{
		assert(splitColIndex > 0 && splitColIndex < m_Columns);

		return GetSubMatrix(Vector2i(0, m_Rows), Vector2i(splitColIndex, m_Columns));
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Add(const Matrix<U>& other)
	{
		assert(m_Rows == other.m_Rows && m_Columns == other.m_Columns);

		for (int32_t i = 0; i < m_Matrix.size(); i++)
			m_Matrix[i] += other.m_Matrix[i];

		return *this;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Substract(const Matrix<U>& other)
	{
		return Add(other * -1);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Multiply(const Matrix<U>& other)
	{
		Matrix tmp = Multiply(*this, other);

		m_Rows = tmp.m_Rows;
		m_Columns = tmp.m_Columns;
		m_Matrix = tmp.m_Matrix;

		return *this;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Multiply(const Vector2<U>& other)
	{
		Matrix tmp = Multiply(*this, other);

		m_Rows = tmp.m_Rows;
		m_Columns = tmp.m_Columns;
		m_Matrix = tmp.m_Matrix;

		return *this;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Multiply(U scalar)
	{
		for (int32_t i = 0; i < m_Matrix.size(); i++)
			m_Matrix[i] *= scalar;

		return *this;
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::Transpose()
	{
		Matrix tmp = GetTranspose();

		m_Rows = tmp.m_Rows;
		m_Columns = tmp.m_Columns;
		m_Matrix = tmp.m_Matrix;

		return *this;
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::RowAdd(int32_t rowFrom, int32_t rowTo, T factor)
	{
		assert(rowFrom >= 0 && rowTo >= 0);
		assert(rowFrom < m_Rows && rowTo < m_Rows);

		for (int32_t col = 0; col < m_Columns; col++)
			operator[](Vector2i(col, rowTo)) += factor * operator[](Vector2i(col, rowFrom));

		return *this;
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::RowMultiply(int32_t rowIndex, T scalar)
	{
		assert(rowIndex >= 0);

		if (scalar == 1)
			return *this;

		for (int32_t x = 0; x < m_Columns; x++)
			operator[](Vector2i(x, rowIndex)) *= scalar;

		return *this;
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::RowSwap(int32_t rowA, int32_t rowB)
	{
		assert(rowA >= 0 && rowB >= 0);
		assert(rowA < m_Rows && rowB < m_Rows);

		if (rowA == rowB)
			return *this;

		for (int32_t i = 0; i < m_Columns; i++)
			std::swap(operator[](Vector2i(i, rowA)), operator[](Vector2i(i, rowB)));

		return *this;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::Augment(const Matrix<U>& other)
	{
		assert(IsSquare());
		assert(m_Rows == other.m_Rows);

		Matrix newMatrix(m_Rows, m_Columns + other.m_Columns);

		for (int32_t y = 0; y < m_Rows; y++)
			for (int32_t x = 0; x < newMatrix.m_Columns; x++)
				newMatrix[Vector2i(x, y)] = x < m_Columns ? GetValueAt(y, x) : other.GetValueAt(y, x - m_Columns);

		m_Columns = newMatrix.m_Columns;
		m_Matrix = newMatrix.m_Matrix;

		return *this;
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::Invert()
	{
		m_Matrix = GetInverse().m_Matrix;
		return *this;
	}

	template<typename T>
	inline Vector2<T> Matrix<T>::GetPosition() const
	{
		assert(m_Rows == 3 && m_Columns == 3);
		return Vector2<T>(GetValueAt(Vector2i(2, 0)), GetValueAt(Vector2i(2, 1)));
	}

	//====================================================================================
	//==== IPRINTABLE
	//====================================================================================

	template <typename T>
	const std::string& Matrix<T>::ToString() const
	{
		std::string returnString;

		for (int y = 0; y < m_Rows; ++y)
		{
			std::string rowString = "| ";
			for (int x = 0; x < m_Columns; ++x)
			{
				rowString += fmt::format("{: 09.3f} | ", static_cast<double>(GetValueAt(y, x)));
			}
			returnString.append(rowString += "\n");
		}

		return returnString;
	}

	//====================================================================================
	//==== PUBLIC STATIC METHODS
	//====================================================================================

	template<typename T>
	inline Matrix<T> Matrix<T>::Identity(int32_t depth)
	{
		assert(depth > 0);

		Matrix ret(depth);

		for (int32_t y = 0; y < depth; y++)
			for (int32_t x = 0; x < depth; x++)
				ret[Vector2i(x, y)] = static_cast<T>((y == x ? 1 : 0));

		return ret;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::Add(const Matrix<T>& A, const Matrix<U>& B)
	{
		return Matrix(A).Add(B);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::Substract(const Matrix<T>& A, const Matrix<U>& B)
	{
		return Matrix(A).Substract(B);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::Multiply(const Matrix<T>& A, const Matrix<U>& B)
	{
		assert(A.m_Columns == B.m_Rows);

		Matrix retMatrix(A.m_Rows, B.m_Columns);

		for (int32_t y = 0; y < retMatrix.m_Rows; y++)
		{
			for (int32_t x = 0; x < retMatrix.m_Columns; x++)
			{
				for (int32_t k = 0; k < A.m_Columns; k++)
				{
					retMatrix[Vector2i(x, y)] += A.GetValueAt(Vector2i(k, y)) * B.GetValueAt(Vector2i(x, k));
				}
			}
		}

		return retMatrix;
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::Multiply(const Matrix<T>& A, const Vector2<U>& B)
	{
		return Multiply(A, Matrix::MakeFromPosition(B));
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::Multiply(const Vector2<T>& A, const Matrix<U>& B)
	{
		return Multiply(Matrix::MakeFromPosition(A), B);
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::Multiply(const Matrix<T>& A, T scalar)
	{
		return Matrix(A).Multiply(scalar);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::MakeFromPosition(const Vector2<U>& position)
	{
		return
			Matrix(
				3,
				3,
				{
					1, 0, position.X,
					0, 1, position.Y,
					0, 0, 1
				}
		);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::MakeFromScale(const Vector2<U>& scale)
	{
		return
			Matrix(
				3,
				3,
				{
					scale.X , 0, 0,
					0, scale.Y, 0,
					0, 0, 1
				}
		);
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::MakeFromRotation(float rotationRad)
	{
		return
			Matrix(
				3,
				3,
				{
					cos(rotationRad), -sin(rotationRad), 0,
					sin(rotationRad), cos(rotationRad), 0,
					0, 0, 1
				}
		);
	}

	template<typename T>
	template<typename U, typename V>
	inline Matrix<T> Matrix<T>::MakeFrom(const Vector2<U>& position, float rotationRad, const Vector2<V>& scale)
	{
		return
			Matrixf::Multiply(
				Matrixf::MakeFromPosition(position),
				Matrixf::Multiply(
					Matrixf::MakeFromRotation(rotationRad),
					Matrixf::MakeFromScale(scale)
				)
			);
	}

	//====================================================================================
	//==== PUBLIC OPERATORS
	//====================================================================================

	template<typename T>
	template<typename U>
	inline bool Matrix<T>::operator==(const Matrix<U>& other) const
	{
		if (m_Rows == other.m_Rows
			&& m_Columns == other.m_Columns
			&& m_Matrix == other.m_Matrix)
			return true;

		return false;
	}

	template<typename T>
	template<typename U>
	inline bool Matrix<T>::operator!=(const Matrix<U>& other) const
	{
		if (m_Rows == other.m_Rows
			&& m_Columns == other.m_Columns
			&& m_Matrix == other.m_Matrix)
			return false;

		return true;
	}

	template<typename T>
	inline T& Matrix<T>::operator[](uint32_t index)
	{
		return m_Matrix[index];
	}

	template<typename T>
	inline T& Matrix<T>::operator[](Vector2i coords)
	{
		assert(coords.X >= 0 && coords.X < m_Columns);
		assert(coords.Y >= 0 && coords.Y < m_Rows);

		return m_Matrix[coords.Y * m_Columns + coords.X];
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::operator+(const Matrix<U>& other) const
	{
		return Matrix::Add(*this, other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::operator-(const Matrix<U>& other) const
	{
		return Matrix::Substract(*this, other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::operator*(const Matrix<U>& other) const
	{
		return Matrix::Multiply(*this, other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T> Matrix<T>::operator*(const Vector2<U>& other) const
	{
		return Matrix::Multiply(*this, other);
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::operator*(T scalar) const
	{
		return Matrix::Multiply(*this, scalar);
	}

	template<typename T>
	inline Matrix<T> Matrix<T>::operator/(T scalar) const
	{
		return Matrix::Multiply(*this, scalar == 0 ? INFINITY : 1.0 / scalar);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::operator+=(const Matrix<U>& other)
	{
		return Add(other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::operator-=(const Matrix<U>& other)
	{
		return Substract(other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::operator*=(const Matrix<U>& other)
	{
		return Multiply(other);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>& Matrix<T>::operator*=(const Vector2<U>& vector)
	{
		return Multiply(vector);
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::operator*=(T scalar)
	{
		return Multiply(scalar);
	}

	template<typename T>
	inline Matrix<T>& Matrix<T>::operator/=(T scalar)
	{
		return Multiply(scalar == 0 ? INFINITY : 1.0 / scalar);
	}

	template<typename T>
	template<typename U>
	inline Matrix<T>::operator Matrix<U>() const
	{
		std::vector<U> newMatrix(this->m_Matrix.size());

		size_t i = 0;
		for (auto it = this->m_Matrix.begin(); it != this->m_Matrix.end(); i++, it++)
			newMatrix[i] = static_cast<U>(*it);

		return Matrix<U>(this->m_Rows, this->m_Columns, newMatrix);
	}
}
