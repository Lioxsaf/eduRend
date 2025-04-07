
#include "Scene.h"
#include "QuadModel.h"
#include "OBJModel.h"
#include "cube.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	// + init other CBuffers
	InitLightCameraBuffer();
	InitMaterialBuffer();


}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)


	m_camera->MoveTo({ 0, 0, 5 });

	// Create objects
	/*m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);*/
	m_cube = new Cube(m_dxdevice, m_dxdevice_context);
	m_cube2 = new Cube(m_dxdevice, m_dxdevice_context);
	m_cube3 = new Cube(m_dxdevice, m_dxdevice_context);

	// Red
	m_cube->material.AmbientColour = vec3f(1.0f, 0.0f, 0.0f);
	m_cube->material.DiffuseColour = vec3f(1.0f, 0.0f, 0.0f);
	m_cube->material.SpecularColour = vec3f(1.0f, 0.0f, 0.0f);

	// blue

	m_cube2->material.AmbientColour = vec3f(0.0f, 0.0f, 1.0f);
	m_cube2->material.DiffuseColour = vec3f(0.0f, 0.0f, 1.0f);
	m_cube2->material.SpecularColour = vec3f(0.0f, 0.0f, 1.0f);

	// purple

	m_cube3->material.AmbientColour = vec3f(1.0f, 0.0f, 1.0f);
	m_cube3->material.DiffuseColour = vec3f(1.0f, 0.0f, 1.0f);
	m_cube3->material.SpecularColour = vec3f(1.0f, 0.0f, 1.0f);



	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context);

}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	// Basic camera control
	if (input_handler.IsKeyPressed(Keys::Up) || input_handler.IsKeyPressed(Keys::W))
		m_camera->Move({ 0.0f, 0.0f, -m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Down) || input_handler.IsKeyPressed(Keys::S))
		m_camera->Move({ 0.0f, 0.0f, m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Right) || input_handler.IsKeyPressed(Keys::D))
		m_camera->Move({ m_camera_velocity * dt, 0.0f, 0.0f });
	if (input_handler.IsKeyPressed(Keys::Left) || input_handler.IsKeyPressed(Keys::A))
		m_camera->Move({ -m_camera_velocity * dt, 0.0f, 0.0f });


	m_camera->GetMouseMovement(input_handler.GetMouseDeltaX(), input_handler.GetMouseDeltaY());

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	 /*Quad model-to-world transformation*/
	//m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
	//	mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
	//	mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// cube model-to-world transofmration

	// local variables
	float moonOrbit = 1.5f * dt;
	float earthOrbit = 1.5f * dt;

	float moonSelfRotation = 30.0f * dt;
	float earthSelfRotation = 3.0f * dt;

	cube2OrbitRotation += earthOrbit;
	cube3OrbitRotation += moonOrbit;

	cube2SelfRotation += earthSelfRotation;
	cube3SelfRotation += moonSelfRotation;



	mat4f sunTransform = mat4f::translation(0, 0, 0) * mat4f::scaling(1.0f);

	m_cube_transform = sunTransform;

	mat4f earthTransform = sunTransform * 
		mat4f::rotation(cube2OrbitRotation, 0.0f, 1.0f, 0.0f) * 
		mat4f::translation(2.0f, 0, 0) * 
		mat4f::rotation(cube2SelfRotation, 0.0f, 1.0f, 0.0f) * 
		mat4f::scaling(0.25f);

	mat4f moonTransform = earthTransform *
		mat4f::rotation(cube3OrbitRotation, 0.0f, 1.0f, 0.0f) *
		mat4f::translation(3.0f, 0, 0) *
		mat4f::rotation(cube3SelfRotation, 0.0f, 1.0f, 0.0f) *
		mat4f::scaling(0.6f);


	
	m_cube2_transform = earthTransform;

	m_cube3_transform = moonTransform;




	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	// Increment the rotation angle.
	m_angle += m_angular_velocity * dt;

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}

	UpdateLightCameraBuffer();
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
	m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_lightCamera_buffer);
	m_dxdevice_context->PSSetConstantBuffers(2, 1, &m_material_buffer);




	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();

	// Load matrices + the Quad's transformation to the device and render it
	/*UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	m_quad->Render();*/

	UpdateTransformationBuffer(m_cube_transform, m_view_matrix, m_projection_matrix);
	UpdateMaterialBuffer(m_cube -> material);
	m_cube->Render();

	UpdateTransformationBuffer(m_cube2_transform, m_view_matrix, m_projection_matrix);
	UpdateMaterialBuffer(m_cube2->material);
	m_cube2->Render();

	UpdateTransformationBuffer(m_cube3_transform, m_view_matrix, m_projection_matrix);
	UpdateMaterialBuffer(m_cube3->material);
	m_cube3->Render();


	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	UpdateMaterialBuffer(m_sponza->material);
	m_sponza->Render();
}

void OurTestScene::Release()
{
	/*SAFE_DELETE(m_quad);*/
	SAFE_DELETE(m_cube);
	SAFE_DELETE(m_cube2);
	SAFE_DELETE(m_cube3);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_camera);

	SAFE_RELEASE(m_transformation_buffer);
	// + release other CBuffers
	SAFE_RELEASE(m_lightCamera_buffer)
	SAFE_RELEASE(m_material_buffer)
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::InitLightCameraBuffer() 
{
	HRESULT hr;
	D3D11_BUFFER_DESC lightCameraBuffer = { 0 };
	lightCameraBuffer.Usage = D3D11_USAGE_DYNAMIC;
	lightCameraBuffer.ByteWidth = sizeof(LightCameraBuffer);
	lightCameraBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCameraBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCameraBuffer.MiscFlags = 0;
	lightCameraBuffer.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCameraBuffer, nullptr, &m_lightCamera_buffer));
}

void OurTestScene::InitMaterialBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC materialBuffer = { 0 };
	materialBuffer.Usage = D3D11_USAGE_DYNAMIC;
	materialBuffer.ByteWidth = sizeof(MaterialBuffer);
	materialBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBuffer.MiscFlags = 0;
	materialBuffer.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBuffer, nullptr, &m_material_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}


void OurTestScene::UpdateLightCameraBuffer() 
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_lightCamera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCameraBuffer* lightCamera = (LightCameraBuffer*)resource.pData;
	vec3f cameraPos = m_camera -> GetPosition();
	lightCamera -> cameraPos = vec4f (cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	lightCamera->lightColorPos = vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	lightCamera->lightPos = vec4f(10.0f, 5.0f, 10.0f * sin(m_angle), 1.0f);
	m_dxdevice_context->Unmap(m_lightCamera_buffer, 0);
}

void OurTestScene::UpdateMaterialBuffer(const Material& material) 
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);	
	MaterialBuffer* buffer = (MaterialBuffer*)resource.pData;
	buffer->ambientColor = vec4f(material.AmbientColour, 1.0f);
	buffer->diffuseColor = vec4f(material.AmbientColour, 1.0f);
	buffer->specularColor = vec4f(material.SpecularColour, 1.0f);
	buffer->shininess = 32.0f; // Nice shiny plastic/metal look
	buffer->padding = (0.0f, 0.0f, 0.0f);
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}