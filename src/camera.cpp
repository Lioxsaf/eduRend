#include "Camera.h"

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept{
	/*m_position += direction;*/

	vec3f m_forward = { sin(m_yaw), 0, cos(m_yaw)};  // forward vector || Calculated from when angle is 0, vector should be (0, 0, 1)
	vec3f m_right = { cos(m_yaw), 0 , -sin(m_yaw) };  // Right Vector vector 
	vec3f m_up = { 0, 1, 0 };

	// Movement in viewspace
	vec3f movement = m_forward * direction.z + m_right * direction.x + m_up * direction.y;

	m_position += movement;
}

void Camera::GetMouseMovement(long dx, long dy) {
	m_sens = 0.02f;
	m_yaw += dx * m_sens;
	m_pitch += dy * m_sens;

	const float PITCH_LIMIT = 1.55334f;

	if (m_pitch > PITCH_LIMIT) m_pitch = PITCH_LIMIT;
	if (m_pitch < -PITCH_LIMIT) m_pitch = -PITCH_LIMIT;
}

const linalg :: vec3f& Camera :: GetPosition() const noexcept
{
	// TODO: insert return statement here
	return m_position;
}

mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	mat4f rotation = mat4f::rotation(0, m_yaw, m_pitch);
	rotation.transpose();
	mat4f translation = mat4f::translation(-m_position);


	return rotation * translation;
}



mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}


