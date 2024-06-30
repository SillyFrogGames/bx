#include "bx/engine/core/math.hpp"

#define GLM_LANG_STL11_FORCED
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_QUAT_DATA_XYZW

#include <glm/glm.hpp>

#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// FIXME: There is a bug with glm when GLM_FORCE_QUAT_DATA_XYZW is defined
#define GLM_PATCH_QUAT_DATA_XYZW

#ifdef GLM_PATCH_QUAT_DATA_XYZW
static Quat QuatFromGLM(glm::quat q)
{
	return Quat(q.x, q.y, q.z, q.w);
}

static glm::quat QuatToGLM(const Quat& q)
{
	return glm::quat(q.w, q.x, q.y, q.z);
}
#else
static Quat QuatFromGLM(glm::quat q)
{
	return Quat::FromValuePtr(glm::value_ptr(q));
}

static glm::quat QuatToGLM(const Quat& q)
{
	return glm::make_quat(q.data);
}
#endif

f32 Vec2::At(i32 i) const
{
	return data[i];
}

f32 Vec2::SqrMagnitude() const
{
	return Vec2::Dot(*this, *this);
}

f32 Vec2::Magnitude() const
{
	return sqrtf(SqrMagnitude());
}

Vec2 Vec2::Normalized() const
{
	f32 magnitude = Magnitude();
	if (magnitude > SAFE_DIV_EPSILON)
	{
		f32 invMagnitude = 1.0 / magnitude;
		return (*this) * invMagnitude;
	}
	else
	{
		return *this;
	}
}

Vec2 Vec2::Abs() const
{
	return Vec2(fabsf(x), fabsf(y));
}

void Vec2::Set(f32 x, f32 y)
{
	data[0] = x;
	data[1] = y;
}

Vec2 Vec2::PlusF32(f32 rhs) const
{
	return Vec2(x + rhs, y + rhs);
}

Vec2 Vec2::Plus(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::Negate() const
{
	return Vec2(-x, -y);
}

Vec2 Vec2::MinusF32(f32 rhs) const
{
	return Vec2(x - rhs, y - rhs);
}

Vec2 Vec2::Minus(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::MulF32(f32 rhs) const
{
	return Vec2(x * rhs, y * rhs);
}

Vec2 Vec2::Mul(const Vec2& rhs) const
{
	return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::DivF32(f32 rhs) const
{
	f32 invRhs = 1.0 / rhs;
	return Vec2(x * invRhs, y * invRhs);
}

Vec2 Vec2::Div(const Vec2& rhs) const
{
	return Vec2(x / rhs.x, y / rhs.y);
}

f32 Vec2::Dot(const Vec2& a, const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

Vec2 Vec2::Lerp(const Vec2& a, const Vec2& b, f32 t)
{
	return Math::Lerp(a, b, t);
}

void Vec2::Normalize(Vec2& v)
{
	v = v.Normalized();
}

Vec2 Vec2::FromValuePtr(f32* vptr)
{
	Vec2 v;
	memcpy(v.data, vptr, sizeof(Vec2));
	return v;
}

f32 Vec3::At(i32 i) const
{
	return data[i];
}

f32 Vec3::SqrMagnitude() const
{
	return Vec3::Dot(*this, *this);
}

f32 Vec3::Magnitude() const
{
	return sqrtf(SqrMagnitude());
}

Vec3 Vec3::Normalized() const
{
	f32 magnitude = Magnitude();
	if (magnitude > SAFE_DIV_EPSILON)
	{
		f32 invMagnitude = 1.0 / magnitude;
		return (*this) * invMagnitude;
	}
	else
	{
		return *this;
	}
}

Vec3 Vec3::Abs() const
{
	return Vec3(fabsf(x), fabsf(y), fabsf(z));
}

void Vec3::Set(f32 x, f32 y, f32 z)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

Vec3 Vec3::PlusF32(f32 rhs) const
{
	return Vec3(x + rhs, y + rhs, z + rhs);
}

Vec3 Vec3::Plus(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 Vec3::Negate() const
{
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::MinusF32(f32 rhs) const
{
	return Vec3(x - rhs, y - rhs, z - rhs);
}

Vec3 Vec3::Minus(const Vec3& rhs) const
{
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 Vec3::MulF32(f32 rhs) const
{
	return Vec3(x * rhs, y * rhs, z * rhs);
}

Vec3 Vec3::Mul(const Vec3& rhs) const
{
	return Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vec3 Vec3::DivF32(f32 rhs) const
{
	f32 invRhs = 1.0 / rhs;
	return Vec3(x * invRhs, y * invRhs, z * invRhs);
}

Vec3 Vec3::Div(const Vec3& rhs) const
{
	return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
}

f32 Vec3::Dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, f32 t)
{
	return Math::Lerp(a, b, t);
}

void Vec3::Normalize(Vec3& v)
{
	v = v.Normalized();
}

Vec3 Vec3::Cross(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

Vec3 Vec3::FromValuePtr(f32* vptr)
{
	Vec3 v;
	memcpy(v.data, vptr, sizeof(Vec3));
	return v;
}

f32 Vec4::At(i32 i) const
{
	return data[i];
}

f32 Vec4::SqrMagnitude() const
{
	return Vec4::Dot(*this, *this);
}

f32 Vec4::Magnitude() const
{
	return sqrtf(SqrMagnitude());
}

Vec4 Vec4::Normalized() const
{
	f32 magnitude = Magnitude();
	if (magnitude > SAFE_DIV_EPSILON)
	{
		f32 invMagnitude = 1.0 / magnitude;
		return (*this) * invMagnitude;
	}
	else
	{
		return *this;
	}
}

Vec4 Vec4::Abs() const
{
	return Vec4(fabsf(x), fabsf(y), fabsf(z), fabsf(w));
}

void Vec4::Set(f32 x, f32 y, f32 z, f32 w)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

Vec4 Vec4::PlusF32(f32 rhs) const
{
	return Vec4(x + rhs, y + rhs, z + rhs, w + rhs);
}

Vec4 Vec4::Plus(const Vec4& rhs) const
{
	return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Vec4 Vec4::Negate() const
{
	return Vec4(-x, -y, -z, -w);
}

Vec4 Vec4::MinusF32(f32 rhs) const
{
	return Vec4(x - rhs, y - rhs, z - rhs, w - rhs);
}

Vec4 Vec4::Minus(const Vec4& rhs) const
{
	return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Vec4 Vec4::MulF32(f32 rhs) const
{
	return Vec4(x * rhs, y * rhs, z * rhs, w * rhs);
}

Vec4 Vec4::Mul(const Vec4& rhs) const
{
	return Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
}

Vec4 Vec4::DivF32(f32 rhs) const
{
	f32 invRhs = 1.0 / rhs;
	return Vec4(x * invRhs, y * invRhs, z * invRhs, w * invRhs);
}

Vec4 Vec4::Div(const Vec4& rhs) const
{
	return Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
}

f32 Vec4::Dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Vec4 Vec4::Lerp(const Vec4& a, const Vec4& b, f32 t)
{
	return Math::Lerp(a, b, t);
}

void Vec4::Normalize(Vec4& v)
{
	v = v.Normalized();
}

Vec4 Vec4::FromValuePtr(f32* vptr)
{
	Vec4 v;
	memcpy(v.data, vptr, sizeof(Vec4));
	return v;
}

f32 Color::At(i32 i) const
{
	return data[i];
}

Color Color::FromValuePtr(f32* vptr)
{
	Color v;
	memcpy(v.data, vptr, sizeof(Color));
	return v;
}

Vec4i Vec4i::FromValuePtr(i32* vptr)
{
	Vec4i v;
	memcpy(v.data, vptr, sizeof(Vec4i));
	return v;
}

f32 Quat::At(i32 i) const
{
	return data[i];
}

Quat Quat::Normalized() const
{
	glm::quat q = glm::normalize(QuatToGLM(*this));
	return QuatFromGLM(q);
}

f32 Quat::SqrMagnitude() const
{
	return x * x + y * y + z * z + w * w;
}

f32 Quat::Magnitude() const
{
	return sqrt(SqrMagnitude());
}

Quat Quat::Inverse() const
{
	f32 magnitude = Magnitude();
	if (magnitude > SAFE_DIV_EPSILON)
	{
		f32 invMagnitude = 1.0 / magnitude;
		return Quat(
			-x * invMagnitude,
			-y * invMagnitude,
			-z * invMagnitude,
			-w * invMagnitude
		);
	}
	else
	{
		return *this;
	}
}

Quat Quat::PlusQuat(const Quat& rhs) const
{
	return Quat(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Quat Quat::PlusF32(f32 rhs) const
{
	return Quat(x + rhs, y + rhs, z + rhs, w + rhs);
}

Quat Quat::Negate() const
{
	return Quat(-x, -y, -z, -w);
}

Quat Quat::MulQuat(const Quat& rhs) const
{
	return Quat(
		w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
		w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
		w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
		w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
	);
}

Vec3 Quat::MulVec3(const Vec3& rhs) const
{
	Vec3 qv(x, y, z);
	Vec3 t = Vec3::Cross(qv, rhs) * 2.0;
	return rhs + t * w + Vec3::Cross(qv, t);
}

Quat Quat::MulF32(f32 rhs) const
{
	return Quat(x * rhs, y * rhs, z * rhs, w * rhs);
}

Quat Quat::DivF32(f32 rhs) const
{
	f32 invRhs = 1.0 / rhs;
	return Quat(x * invRhs, y * invRhs, z * invRhs, w * invRhs);
}

Vec3 Quat::EulerAngles() const
{
	Vec3 euler;
	f32 sinrCosp = 2.0 * (w * x + y * z);
	f32 cosrCosp = 1.0 - 2.0 * (x * x + y * y);
	euler.x = Math::Degrees(std::atan2(sinrCosp, cosrCosp));
	f32 sinp = 2.0 * (w * y - z * x);
	if (std::abs(sinp) >= 1.0)
		euler.y = Math::Degrees(std::copysignf(Math::PI_2, sinp));
	else
		euler.y = Math::Degrees(std::asin(sinp));
	f32 sinyCosp = 2.0 * (w * z + x * y);
	f32 cosyCosp = 1.0 - 2.0 * (y * y + z * z);
	euler.z = Math::Degrees(std::atan2f(sinyCosp, cosyCosp));
	return euler;
}

Quat Quat::Euler(f32 x, f32 y, f32 z)
{
	Vec3 halfEuler = Vec3(x, y, z) * 0.5;
	f32 cr = cosf(Math::Radians(halfEuler.x));
	f32 sr = sinf(Math::Radians(halfEuler.x));
	f32 cy = cosf(Math::Radians(halfEuler.z));
	f32 sy = sinf(Math::Radians(halfEuler.z));
	f32 cp = cosf(Math::Radians(halfEuler.y));
	f32 sp = sinf(Math::Radians(halfEuler.y));
	return Quat(
		sr * cp * cy - cr * sp * sy,
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy,
		cr * cp * cy + sr * sp * sy
	);
}

Quat Quat::AngleAxis(f32 angleInDegrees, const Vec3& axis)
{
	f32 rangle = Math::Radians(angleInDegrees);
	f32 sha = sinf(rangle * 0.5);
	return Quat(axis.x * sha, axis.y * sha, axis.z * sha, cosf(rangle * 0.5));
}

f32 Quat::Dot(const Quat& a, const Quat& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Quat Quat::Slerp(const Quat& a, const Quat& b, f32 t)
{
	Quat z = b;

	f32 cosTheta = Dot(a, b);
	if (cosTheta < 0)
	{
		z = -b;
		cosTheta = -cosTheta;
	}

	if (cosTheta > 1 - SAFE_DIV_EPSILON)
	{
		return Quat(
			Math::Lerp(a.x, z.x, t),
			Math::Lerp(a.y, z.y, t),
			Math::Lerp(a.z, z.z, t),
			Math::Lerp(a.w, z.w, t)
		);
	}
	else
	{
		f32 angle = acosf(cosTheta);
		return (a * sin((1.0 - t) * angle) + z * sin(t * angle)) / sinf(angle);
	}
}

Quat Quat::FromMat4(const Mat4& _matrix)
{
	glm::mat4 matrix = glm::make_mat4(_matrix.data);
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale, rotation, translation, skew, perspective);
	return QuatFromGLM(rotation);

	// TODO: why does this break the gauntlet game?? (used in Mat4::Decompose)
	// Source: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/#google_vignette
	/*Quat q;
	Mat4 a = _matrix.Transpose();
	float trace = a(0, 0) + a(1, 1) + a(2, 2);
	if (trace > 0.0)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (a(2, 1) - a(1, 2)) * s;
		q.y = (a(0, 2) - a(2, 0)) * s;
		q.z = (a(1, 0) - a(0, 1)) * s;
	}
	else
	{
		if (a(0, 0) > a(1, 1) && a(0, 0) > a(2, 2))
		{
			float s = 2.0f * sqrtf(1.0f + a(0, 0) - a(1, 1) - a(2, 2));
			q.w = (a(2, 1) - a(1, 2)) / s;
			q.x = 0.25f * s;
			q.y = (a(0, 1) + a(1, 0)) / s;
			q.z = (a(0, 2) + a(2, 0)) / s;
		}
		else if (a(1, 1) > a(2, 2))
		{
			float s = 2.0f * sqrtf(1.0f + a(1, 1) - a(0, 0) - a(2, 2));
			q.w = (a(0, 2) - a(2, 0)) / s;
			q.x = (a(0, 1) + a(1, 0)) / s;
			q.y = 0.25f * s;
			q.z = (a(1, 2) + a(2, 1)) / s;
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + a(2, 2) - a(0, 0) - a(1, 1));
			q.w = (a(1, 0) - a(0, 1)) / s;
			q.x = (a(0, 2) + a(2, 0)) / s;
			q.y = (a(1, 2) + a(2, 1)) / s;
			q.z = 0.25f * s;
		}
	}
	return q;*/
}

Quat Quat::FromValuePtr(f32* vptr)
{
	Quat q;
	memcpy(q.data, vptr, sizeof(Quat));
	return q;
}

f32 Mat4::At(i32 i) const
{
	return data[i];
}

f32 Mat4::At(i32 i, i32 j) const
{
	return columns[i][j];
}

Mat4 Mat4::Mul(const Mat4& rhs) const
{
	Mat4 result;
	for (u32 y = 0; y < 4; y++)
	{
		for (u32 x = 0; x < 4; x++)
		{
			f64 sum = 0.0;
			for (u32 e = 0; e < 4; e++)
				sum += data[x + e * 4] * rhs[e + y * 4];
			result[x + y * 4] = sum;
		}
	}
	return result;
}

Vec4 Mat4::MulVec4(const Vec4& rhs) const
{
	return Vec4(this->columns[0].x * rhs.x + this->columns[1].x * rhs.y +
		this->columns[2].x * rhs.z + this->columns[3].x * rhs.w,
		this->columns[0].y * rhs.x + this->columns[1].y * rhs.y +
		this->columns[2].y * rhs.z + this->columns[3].y * rhs.w,
		this->columns[0].z * rhs.x + this->columns[1].z * rhs.y +
		this->columns[2].z * rhs.z + this->columns[3].z * rhs.w,
		this->columns[0].w * rhs.x + this->columns[1].w * rhs.y +
		this->columns[2].w * rhs.z + this->columns[3].w * rhs.w);
}

Mat4 Mat4::Transpose() const
{
	Mat4 result;
	for (i32 i = 0; i < 4; i++)
	{
		for (i32 j = 0; j < 4; j++)
		{
			result(i, j) = this->At(j, i);
		}
	}
	return result;
}

Mat4 Mat4::Inverse() const
{
	const f32 inv[16] = { data[5] * data[10] * data[15] -
								data[5] * data[11] * data[14] -
								data[9] * data[6] * data[15] +
								data[9] * data[7] * data[14] +
								data[13] * data[6] * data[11] -
								data[13] * data[7] * data[10],
							-data[1] * data[10] * data[15] +
								data[1] * data[11] * data[14] +
								data[9] * data[2] * data[15] -
								data[9] * data[3] * data[14] -
								data[13] * data[2] * data[11] +
								data[13] * data[3] * data[10],
							data[1] * data[6] * data[15] -
								data[1] * data[7] * data[14] -
								data[5] * data[2] * data[15] +
								data[5] * data[3] * data[14] +
								data[13] * data[2] * data[7] -
								data[13] * data[3] * data[6],
							-data[1] * data[6] * data[11] +
								data[1] * data[7] * data[10] +
								data[5] * data[2] * data[11] -
								data[5] * data[3] * data[10] -
								data[9] * data[2] * data[7] +
								data[9] * data[3] * data[6],
							-data[4] * data[10] * data[15] +
								data[4] * data[11] * data[14] +
								data[8] * data[6] * data[15] -
								data[8] * data[7] * data[14] -
								data[12] * data[6] * data[11] +
								data[12] * data[7] * data[10],
							data[0] * data[10] * data[15] -
								data[0] * data[11] * data[14] -
								data[8] * data[2] * data[15] +
								data[8] * data[3] * data[14] +
								data[12] * data[2] * data[11] -
								data[12] * data[3] * data[10],
							-data[0] * data[6] * data[15] +
								data[0] * data[7] * data[14] +
								data[4] * data[2] * data[15] -
								data[4] * data[3] * data[14] -
								data[12] * data[2] * data[7] +
								data[12] * data[3] * data[6],
							data[0] * data[6] * data[11] -
								data[0] * data[7] * data[10] -
								data[4] * data[2] * data[11] +
								data[4] * data[3] * data[10] +
								data[8] * data[2] * data[7] -
								data[8] * data[3] * data[6],
							data[4] * data[9] * data[15] -
								data[4] * data[11] * data[13] -
								data[8] * data[5] * data[15] +
								data[8] * data[7] * data[13] +
								data[12] * data[5] * data[11] -
								data[12] * data[7] * data[9],
							-data[0] * data[9] * data[15] +
								data[0] * data[11] * data[13] +
								data[8] * data[1] * data[15] -
								data[8] * data[3] * data[13] -
								data[12] * data[1] * data[11] +
								data[12] * data[3] * data[9],
							data[0] * data[5] * data[15] -
								data[0] * data[7] * data[13] -
								data[4] * data[1] * data[15] +
								data[4] * data[3] * data[13] +
								data[12] * data[1] * data[7] -
								data[12] * data[3] * data[5],
							-data[0] * data[5] * data[11] +
								data[0] * data[7] * data[9] +
								data[4] * data[1] * data[11] -
								data[4] * data[3] * data[9] -
								data[8] * data[1] * data[7] +
								data[8] * data[3] * data[5],
							-data[4] * data[9] * data[14] +
								data[4] * data[10] * data[13] +
								data[8] * data[5] * data[14] -
								data[8] * data[6] * data[13] -
								data[12] * data[5] * data[10] +
								data[12] * data[6] * data[9],
							data[0] * data[9] * data[14] -
								data[0] * data[10] * data[13] -
								data[8] * data[1] * data[14] +
								data[8] * data[2] * data[13] +
								data[12] * data[1] * data[10] -
								data[12] * data[2] * data[9],
							-data[0] * data[5] * data[14] +
								data[0] * data[6] * data[13] +
								data[4] * data[1] * data[14] -
								data[4] * data[2] * data[13] -
								data[12] * data[1] * data[6] +
								data[12] * data[2] * data[5],
							data[0] * data[5] * data[10] -
								data[0] * data[6] * data[9] -
								data[4] * data[1] * data[10] +
								data[4] * data[2] * data[9] +
								data[8] * data[1] * data[6] -
								data[8] * data[2] * data[5] };

	Mat4 result = *this;
	const f32 det = data[0] * inv[0] + data[1] * inv[4] +
		data[2] * inv[8] + data[3] * inv[12];
	if (det != 0.0)
	{
		const f32 invdet = 1.0 / det;
		for (u32 i = 0; i < 16; i++)
		{
			result[i] = inv[i] * invdet;
		}
	}
	return result;
}

Mat4 Mat4::Identity()
{
	static Mat4 s_identity = Mat4(
		Vec4(1, 0, 0, 0),
		Vec4(0, 1, 0, 0),
		Vec4(0, 0, 1, 0),
		Vec4(0, 0, 0, 1)
	);
	return s_identity;
}

Mat4 Mat4::Zero()
{
	return Mat4(
		Vec4(0, 0, 0, 0),
		Vec4(0, 0, 0, 0),
		Vec4(0, 0, 0, 0),
		Vec4(0, 0, 0, 0)
	);
}

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
	glm::vec3 e = glm::make_vec3(eye.data);
	glm::vec3 c = glm::make_vec3(center.data);
	glm::vec3 u = glm::make_vec3(up.data);
	glm::mat4 m = glm::lookAt(e, c, u);
	return Mat4::FromValuePtr(glm::value_ptr(m));
}

Mat4 Mat4::Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
	Mat4 result = Mat4::Identity();
	result(0, 0) = 2.0 / (right - left);
	result(1, 1) = 2.0 / (top - bottom);
	result(2, 2) = -2.0 / (zFar - zNear);
	result(3, 0) = (left + right) / (right - left);
	result(3, 1) = (bottom + top) / (top - bottom);
	result(3, 2) = -(zFar + zNear) / (zFar - zNear);
	return result;
}

Mat4 Mat4::Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar)
{
	Mat4 result = Mat4::Identity();
	f32 q = 1.0 / tanf(Math::Radians(0.5 * fov));
	f32 a = q / aspect;
	f32 b = (zNear + zFar) / (zNear - zFar);
	f32 c = (2.0f * zNear * zFar) / (zNear - zFar);
	result(0, 0) = a;
	result(1, 1) = q;
	result(2, 2) = b;
	result(2, 3) = -1.0;
	result(3, 2) = c;
	return result;
}

Mat4 Mat4::Translation(const Vec3& translation)
{
	Mat4 result = Mat4::Identity();
	result(3, 0) = translation.x;
	result(3, 1) = translation.y;
	result(3, 2) = translation.z;
	return result;
}

Mat4 Mat4::Rotation(f32 angle, const Vec3& axis)
{
	Mat4 result = Mat4::Identity();
	f32 r = Math::Radians(angle);
	f32 c = cosf(r);
	f32 s = sinf(r);
	f32 omc = 1.0 - c;
	result(0, 0) = axis.x * omc + c;
	result(0, 1) = axis.y * axis.x * omc + axis.z * s;
	result(0, 2) = axis.z * axis.x * omc - axis.y * s;
	result(1, 0) = axis.x * axis.y * omc - axis.z * s;
	result(1, 1) = axis.y * omc + c;
	result(1, 2) = axis.y * axis.z * omc + axis.x * s;
	result(2, 0) = axis.x * axis.z * omc + axis.y * s;
	result(2, 1) = axis.y * axis.z * omc - axis.x * s;
	result(2, 2) = axis.z * omc + c;
	return result;
}

Mat4 Mat4::Rotation(const Quat& rotation)
{
	Mat4 result = Mat4::Identity();
	result[0] = 1.0 - 2.0 * rotation.y * rotation.y - 2.0 * rotation.z * rotation.z;
	result[4] = 2.0 * rotation.x * rotation.y - 2.0 * rotation.w * rotation.z;
	result[8] = 2.0 * rotation.x * rotation.z + 2.0 * rotation.w * rotation.y;
	result[1] = 2.0 * rotation.x * rotation.y + 2.0 * rotation.w * rotation.z;
	result[5] = 1.0 - 2.0 * rotation.x * rotation.x - 2.0 * rotation.z * rotation.z;
	result[9] = 2.0 * rotation.y * rotation.z - 2.0 * rotation.w * rotation.x;
	result[2] = 2.0 * rotation.x * rotation.z - 2.0 * rotation.w * rotation.y;
	result[6] = 2.0 * rotation.y * rotation.z + 2.0 * rotation.w * rotation.x;
	result[10] = 1.0 - 2.0 * rotation.x * rotation.x - 2.0 * rotation.y * rotation.y;
	return result;
}

Mat4 Mat4::Scale(const Vec3& scale)
{
	Mat4 result = Mat4::Identity();
	result(0, 0) = scale.x;
	result(1, 1) = scale.y;
	result(2, 2) = scale.z;
	return result;
}

Mat4 Mat4::TRS(const Vec3& translation, const Quat& rotation, const Vec3& scale)
{
	return Mat4::Translation(translation) * Mat4::Rotation(rotation) * Mat4::Scale(scale);
}

void Mat4::Decompose(const Mat4& matrix, Vec3& pos, Quat& rot, Vec3& scl)
{
	pos = Vec3(matrix.columns[3][0], matrix.columns[3][1], matrix.columns[3][2]);
	rot = Quat::FromMat4(matrix);
	scl = Vec3(
		Vec3(matrix.columns[0][0], matrix.columns[0][1], matrix.columns[0][2]).Magnitude(),
		Vec3(matrix.columns[1][0], matrix.columns[1][1], matrix.columns[1][2]).Magnitude(),
		Vec3(matrix.columns[2][0], matrix.columns[2][1], matrix.columns[2][2]).Magnitude()
	);
}

Mat4 Mat4::FromValuePtr(f32* vptr)
{
	Mat4 m;
	memcpy(m.data, vptr, sizeof(Mat4));
	return m;
}