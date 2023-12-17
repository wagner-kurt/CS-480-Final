#include "graphics.h"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif



	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up light
	GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 0.2f };
	GLfloat dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat gamb[] = { 0.3f, 0.3f, 0.3f, 0.3f };
	m_light = new Light(m_camera->GetView(), glm::vec3(0.0f, 0.0f, 0.0f), amb, dif, spec, gamb);

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	// Starship
	m_mesh = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png", "assets\\SpaceShip-1-n.jpg");

	// The Sun
	m_sun = new Sphere(64, "assets\\2k_sun.jpg", "assets\\2k_sun-n.jpg");

	// Mercury
	m_mercury = new Sphere(64, "assets\\Mercury.jpg", "assets\\Mercury-n.jpg");

	// Venus
	m_venus = new Sphere(64, "assets\\Venus.jpg", "assets\\Venus-n.jpg");

	// The Earth
	m_earth = new Sphere(64, "assets\\2k_earth_daymap.jpg", "assets\\2k_earth_daymap-n.jpg");
	
	// The Moon
	m_moon = new Sphere(64, "assets\\2k_moon.jpg", "assets\\2k_moon-n.jpg");

	// Mars
	m_mars = new Sphere(64, "assets\\Mars.jpg", "assets\\Mars-n.jpg");

	// Ceres
	m_ceres = new Sphere(64, "assets\\Ceres.jpg", "assets\\Ceres-n.jpg");

	// Jupiter
	m_jupiter = new Sphere(64, "assets\\Jupiter.jpg", "assets\\Jupiter-n.jpg");

	// Saturn
	m_saturn = new Sphere(64, "assets\\Saturn.jpg", "assets\\Saturn-n.jpg");

	// Uranus
	m_uranus = new Sphere(64, "assets\\Uranus.jpg", "assets\\Uranus-n.jpg");

	// Neptune
	m_neptune = new Sphere(64, "assets\\Neptune.jpg", "assets\\Neptune-n.jpg");


	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}


void Graphics::HierarchicalUpdate2(double dt) {

	std::vector<float> speed, dist, rotSpeed, scale;
	glm::vec3 rotVector;
	glm::mat4 localTransform;

	// position of the sun	
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));  // sun's coordinate
	localTransform = modelStack.top();		// The sun origin
	localTransform *= glm::rotate(glm::mat4(1.0f), (float)dt/4, glm::vec3(0.f, 1.f, 0.f));
	localTransform *= glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	if (m_sun != NULL) {
		m_sun->Update(localTransform);
	}

	// set light position to center of sun, update with camera view matrix
	m_light->m_lightPosition = glm::vec3(m_sun->GetModel()[3]);
	m_light->Update(m_camera->GetView());

	// position of mercury
	speed = { 0.05f, 0.05f, 0.05f };
	dist = { 4.0f, 0.0f, 4.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.177f, 0.177f, 0.177f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(0.034f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mercury != NULL) {
		m_mercury->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of venus
	speed = { 0.03f, 0.03f, 0.03f };
	dist = { 6.0f, 0.0f, 6.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { -1., -1., -1. };	// retrograde rotation for venus
	scale = { 0.339f, 0.339f, 0.339f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(2.64f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_venus != NULL) {
		m_venus->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of the earth
	speed = { 0.02f, 0.02f, 0.02f };
	dist = { 9.0f, 0.0f, 9.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.351f, 0.351f, 0.351f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(23.439f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_earth != NULL) {
		m_earth->Update(localTransform);
	}

	// position of the moon
	speed = { 0.075f, 0.075f, 0.075f };
	dist = { 2.25f, 0.25f, 2.25f }; // inclined orbit
	rotVector = { 0.0, 1., 0. };
	rotSpeed = { .25, .25, .25 };
	scale = { 0.162f, 0.162f, 0.162f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store moon-planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_moon != NULL) {
		m_moon->Update(localTransform);
	}

	modelStack.pop();	// back to the earth coordinates
	modelStack.pop();	// back to the sun coordinates

	// position of mars
	speed = { 0.012f, 0.012f, 0.012f };
	dist = { 12.0f, 0.0f, 12.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.249f, 0.249f, 0.249f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(25.19f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mars != NULL) {
		m_mars->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of ceres
	speed = { 0.0072f, 0.0072f, 0.0072f };
	dist = { 18.0f, 0.0f, 18.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.120f, 0.120f, 0.120f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(4.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_ceres != NULL) {
		m_ceres->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of jupiter
	speed = { 0.0042f, 0.0042f, 0.0042f };
	dist = { 22.0f, 0.0f, 22.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.574f, 0.574f, 0.574f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(3.13f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_jupiter != NULL) {
		m_jupiter->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of saturn
	speed = { 0.0024f, 0.0024f, 0.0024f };
	dist = { 27.0f, 0.0f, 27.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.562f, 0.562f, 0.562f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(26.73f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn != NULL) {
		m_saturn->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of uranus
	speed = { 0.0014f, 0.0014f, 0.0014f };
	dist = { 32.0f, 0.0f, 32.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.450f, 0.450f, 0.450f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(97.77f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_uranus != NULL) {
		m_uranus->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of neptune
	speed = { 0.0008f, 0.0008f, 0.0008f };
	dist = { 37.0f, 0.0f, 37.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.438f, 0.438f, 0.438f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(28.32f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_neptune != NULL) {
		m_neptune->Update(localTransform);
	}
	modelStack.pop();	// back to the sun coordinates

	// position of the space ship
	speed = { 2., 2., 2. };
	dist = { 22., 22., 22. };
	rotVector = { 0.,1.,1. };
	rotSpeed = { 2., 2., 2. };
	scale = { .01f, .01f, .01f };
	localTransform = modelStack.top();

	//find and rotate by angle between previous and current point in orbit
	glm::vec3 prevShipPos = glm::vec3(m_mesh->GetModel()[3]);
	glm::vec3 newShipPos = glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]);
	glm::vec3 shipRotAxis = glm::cross(prevShipPos, newShipPos);
	float shipRotAngle = glm::acos(glm::dot(glm::normalize(prevShipPos), glm::normalize(newShipPos)));

	localTransform *= glm::translate(glm::mat4(1.f), newShipPos);
	modelStack.push(localTransform);			// store spaceship-moon-planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), shipRotAngle, shipRotAxis);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_mesh != NULL) {
		m_mesh->Update(localTransform);
	}

	modelStack.pop(); 	// back to the sun coordinates

	modelStack.pop();	// empty stack

}


void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist, 
	std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2])
	);
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}

void Graphics::Render()
{
	//clear the screen
	glClearColor(0.5, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	glProgramUniform4fv(m_shader->GetShaderProgram(), m_globalAmbLoc, 1, m_light->m_globalAmbient);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightALoc, 1, m_light->m_lightAmbient);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightDLoc, 1, m_light->m_lightDiffuse);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightSLoc, 1, m_light->m_lightSpecular);
	glProgramUniform3fv(m_shader->GetShaderProgram(), m_lightPosLoc, 1, m_light->m_lightPositionViewSpace);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

	// Render the objects
	/*if (m_cube != NULL){
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render(m_positionAttrib,m_normalAttrib);
	}*/

	
	// Render Ship
	if (m_mesh != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_mesh->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));
		if (m_mesh->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_mesh->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mesh->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_mesh->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	/*if (m_pyramid != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_pyramid->GetModel()));
		m_pyramid->Render(m_positionAttrib, m_normalAttrib);
	}*/

	// Render Sun
	if (m_sun != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_sun->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
		if (m_sun->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_sun->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_sun->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_sun->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Mercury
	if (m_mercury != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_mercury->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury->GetModel()));
		if (m_mercury->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mercury->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_mercury->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mercury->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_mercury->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Venus
	if (m_venus != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_venus->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus->GetModel()));
		if (m_venus->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_venus->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_venus->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_venus->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_venus->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Earth
	if (m_earth != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_earth->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));
		if (m_earth->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_earth->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_earth->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_earth->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Moon
	if (m_moon != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_moon->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
		if (m_moon->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_moon->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_moon->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_moon->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Mars
	if (m_mars != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_mars->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars->GetModel()));
		if (m_mars->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mars->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_mars->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mars->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_mars->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Ceres
	if (m_ceres != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_ceres->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_ceres->GetModel()));
		if (m_ceres->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ceres->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);			
		}
		if (m_ceres->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_ceres->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_ceres->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Jupiter
	if (m_jupiter != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_jupiter->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter->GetModel()));
		if (m_jupiter->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_jupiter->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_jupiter->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_jupiter->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Saturn
	if (m_saturn != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_saturn->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn->GetModel()));
		if (m_saturn->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_saturn->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_saturn->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Uranus
	if (m_uranus != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_uranus->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus->GetModel()));
		if (m_uranus->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_uranus->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_uranus->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_uranus->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_uranus->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	// Render Neptune
	if (m_neptune != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView()* m_neptune->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune->GetModel()));
		if (m_neptune->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_neptune->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_neptune->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_neptune->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_neptune->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}


	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}


bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	// Locate the normal matrix in the shader
	m_normalMatrix = m_shader->GetUniformLocation("normMatrix");
	if (m_normalMatrix == INVALID_UNIFORM_LOCATION) {
		printf("m_normalMatrix not found\n");
		anyProblem = false;
	}

	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == INVALID_UNIFORM_LOCATION)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the normal vertex attribute
	m_normalAttrib = m_shader->GetAttribLocation("v_normal");
	if (m_normalAttrib == INVALID_UNIFORM_LOCATION)
	{
		printf("v_normal attribute not found\n");
		anyProblem = false;
	}

	// Locate the tc vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_tc");
	if (m_tcAttrib == INVALID_UNIFORM_LOCATION)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}
	/*
	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		//anyProblem = false;
	}*/

	hasN = m_shader->GetUniformLocation("hasNormalMap");
	if (hasN == INVALID_UNIFORM_LOCATION) {
		printf("NormalMap uniform bad\n");
		anyProblem = false;
	}

	m_globalAmbLoc = m_shader->GetUniformLocation("GlobalAmbient");
	if (m_globalAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("GlobalAmbient not found\n");
		anyProblem = false;
	}
	
	m_lightALoc = m_shader->GetUniformLocation("light.ambient");
	if (m_lightALoc == INVALID_UNIFORM_LOCATION) {
		printf("LightAmbient not found\n");
		anyProblem = false;
	}
	
	m_lightDLoc = m_shader->GetUniformLocation("light.diffuse");
	if (m_lightDLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightDiffuse not found\n");
		anyProblem = false;
	}
	
	m_lightSLoc = m_shader->GetUniformLocation("light.spec");
	if (m_lightSLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightSpecular not found\n");
		anyProblem = false;
	}
	
	m_lightPosLoc = m_shader->GetUniformLocation("light.position");
	if (m_lightPosLoc == INVALID_UNIFORM_LOCATION) {
		printf("LightPosition not found\n");
		anyProblem = false;
	}

	m_mAmbLoc = m_shader->GetUniformLocation("material.ambient");
	if (m_mAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("MaterialAmbient not found\n");
		anyProblem = false;
	}
	
	m_mDiffLoc = m_shader->GetUniformLocation("material.diffuse");
	if (m_mDiffLoc == INVALID_UNIFORM_LOCATION) {
		printf("MaterialDiffuse not found\n");
		anyProblem = false;
	}
	
	m_mSpecLoc = m_shader->GetUniformLocation("material.spec");
	if (m_mSpecLoc == INVALID_UNIFORM_LOCATION) {
		printf("MaterialSpecular not found\n");
		anyProblem = false;
	}
	
	m_mShineLoc = m_shader->GetUniformLocation("material.shininess");
	if (m_mShineLoc == INVALID_UNIFORM_LOCATION) {
		printf("MaterialShininess not found\n");
		anyProblem = false;
	}
	

	return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}

