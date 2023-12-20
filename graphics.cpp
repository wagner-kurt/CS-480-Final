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

	m_skyboxshader = new SkyboxShader();
	if (!m_skyboxshader->Initialize()) {
		printf("Skybox shader failed to initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	if (!m_skyboxshader->AddShader(GL_VERTEX_SHADER)) {
		printf("Skybox vertex shader failed to initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	if (!m_skyboxshader->AddShader(GL_FRAGMENT_SHADER)) {
		printf("Skybox fragment shader failed to initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Shader Failed to Finalize\n");
		return false;
	}

	if (!m_skyboxshader->Finalize()) {
		printf("Skybox shader failed to finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	// Cube map texture loading/preparation
	cubeMapTextureID = loadCubeMap();
	setupSkybox();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Starship
	m_mesh = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png", "assets\\SpaceShip-1-n.jpg");

	// The Sun
	m_sun = new Sphere(64, "assets\\2k_sun.jpg", "assets\\2k_sun-n.jpg");

	// Comet
	m_comet = new Sphere(64, "assets\\Eris.jpg", "assets\\Eris-n.jpg");

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

	// Asteroid
	m_asteroid = new Sphere * [500];
	for (int i = 0; i < asteroidCount; i++) {
		m_asteroid[i] = new Sphere(8, "assets\\Haumea.jpg", "assets\\Haumea-n.jpg");
	}

	// Set random positions in a ring for each asteroid
	srand(glfwGetTime());
	// area between Mars and Jupiter
	float radius = 17.0f;
	float offset = 3.0f;
	float scale[] = {0.06f, 0.04f, 0.08f};
	for (unsigned int i = 0; i < asteroidCount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)500 * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// random potato shape and size for each asteroid
		float scaleX = scale[0] + (static_cast<float>(std::rand()) / RAND_MAX) * 2 * 0.01f - 0.01f;
		float scaleY = scale[1] + (static_cast<float>(std::rand()) / RAND_MAX) * 2 * 0.01f - 0.01f;
		float scaleZ = scale[2] + (static_cast<float>(std::rand()) / RAND_MAX) * 2 * 0.01f - 0.01f;
		model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));

		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		if (m_asteroid[i] != NULL) {
			m_asteroid[i]->Update(model);
		}
	}

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
	planetLoc[0] = glm::vec3(m_sun->GetModel()[3]);

	// set light position to center of sun, update with camera view matrix
	m_light->m_lightPosition = glm::vec3(m_sun->GetModel()[3]);
	m_light->Update(m_camera->GetView());

	// position of comet
	speed = { 0.5f, 0.5f, 0.5f };
	dist = { 4.0f, 10.0f, 20.0f };
	rotVector = { 0.,1.,0. };
	rotSpeed = { .01, .01, .01 };
	scale = { 0.08f, 0.08f, 0.08f };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(-cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(0.034f), glm::vec3(1.0f, 0.0f, 0.0f)); //axial tilt
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_comet != NULL) {
		m_comet->Update(localTransform);
	}
	planetLoc[11] = glm::vec3(m_comet->GetModel()[3]);
	modelStack.pop();	// back to the sun coordinates

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
	planetLoc[1] = glm::vec3(m_mercury->GetModel()[3]);
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
	planetLoc[2] = glm::vec3(m_venus->GetModel()[3]);
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
	planetLoc[3] = glm::vec3(m_earth->GetModel()[3]);

	// position of the moon
	speed = { 0.2f, 0.2f, 0.2f };
	dist = { 1.5f, 0.25f, 1.5f }; // inclined orbit
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
	planetLoc[4] = glm::vec3(m_moon->GetModel()[3]);
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
	planetLoc[5] = glm::vec3(m_mars->GetModel()[3]);
	modelStack.pop();	// back to the sun coordinates

	// asteroids
	for (unsigned int i = 0; i < asteroidCount; i++) {
		// get initial position
		glm::vec3 initialPos = glm::vec3(m_asteroid[i]->GetModel()[3]);
		glm::vec3 initialScale = glm::vec3(
			m_asteroid[i]->GetModel()[0][0],
			m_asteroid[i]->GetModel()[1][1],
			m_asteroid[i]->GetModel()[2][2]
		);
		glm::mat4 model = glm::mat4(1.0f);

		// radius (different for each asteroid)
		float radius = glm::length(initialPos);
		float angle = -0.00001f * (float)dt;
		
		// continue circular motion based on previous position and speed * dt
		float x = initialPos.x * cos(angle) - initialPos.z * sin(angle);
		float y = initialPos.y;
		float z = initialPos.x * sin(angle) + initialPos.z * cos(angle);
		model *= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		model *= glm::scale(initialScale);

		if (m_asteroid[i] != NULL) {
			m_asteroid[i]->Update(model);
		}
	}

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
	planetLoc[6] = glm::vec3(m_ceres->GetModel()[3]);
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
	planetLoc[7] = glm::vec3(m_jupiter->GetModel()[3]);
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
	planetLoc[8] = glm::vec3(m_saturn->GetModel()[3]);
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
	planetLoc[9] = glm::vec3(m_uranus->GetModel()[3]);
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
	planetLoc[10] = glm::vec3(m_neptune->GetModel()[3]);
	modelStack.pop();	// back to the sun coordinates

	modelStack.pop();	// empty stack

	scale = { .01f, .01f, .01f };

	localTransform = glm::inverse(m_camera->GetOrigView());
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mesh != NULL) {
		m_mesh->Update(localTransform);
	}
	
	if (orbiting) {
		speed = { 0.5f, 0.5f, 0.5f };
		glm::vec3 dists[12] = { {3.5f, 0.0f, 3.5f},
								{1.0f, 0.0f, 1.0f},
								{1.5f, 0.0f, 1.5f},
								{1.5f, 0.0f, 1.5f},
								{1.0f, 0.0f, 1.0f},
								{1.0f, 0.0f, 1.0f},
								{1.0f, 0.0f, 1.0f},
								{2.0f, 0.0f, 2.0f},
								{2.0f, 0.0f, 2.0f},
								{2.0f, 0.0f, 2.0f},
								{2.0f, 0.0f, 2.0f}, 
								{1.0f, 0.0f, 1.0f} };
		glm::vec3 cam = planetLoc[orbitIndex] + glm::vec3(-cos(speed[0] * dt) * dists[orbitIndex][0], sin(speed[1] * dt) * dists[orbitIndex][1], sin(speed[2] * dt) * dists[orbitIndex][2]);
		m_camera->SetPosition(cam);
		m_camera->Rotate(0.174f, 0.f, 0.f);
	}
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

	// Render skybox
	m_skyboxshader->Enable();
	glUniformMatrix4fv(m_skyboxProjMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_skyboxViewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));
	GLint skyboxSampler = m_skyboxshader->GetUniformLocation("samp");
	if (skyboxSampler == INVALID_UNIFORM_LOCATION)
	{
		printf("Skybox sampler not found\n");
	}
	glUniform1i(skyboxSampler, 0);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(skyboxVAO);
	glEnableVertexAttribArray(m_skyboxPositionAttrib);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glVertexAttribPointer(m_skyboxPositionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisableVertexAttribArray(m_skyboxPositionAttrib);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	// Start the shader program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	// Program light uniforms
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_globalAmbLoc, 1, m_light->m_globalAmbient);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightALoc, 1, m_light->m_lightAmbient);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightDLoc, 1, m_light->m_lightDiffuse);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightSLoc, 1, m_light->m_lightSpecular);
	glProgramUniform3fv(m_shader->GetShaderProgram(), m_lightPosLoc, 1, m_light->m_lightPositionViewSpace);

	// Render the objects
	/*if (m_cube != NULL){
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render(m_positionAttrib,m_normalAttrib);
	}*/

	// Render Asteroids
	setMaterialRock();
	for (unsigned int i = 0; i < asteroidCount; i++) {
		if (m_asteroid != NULL) {
			glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_asteroid[i]->GetModel())))));
			glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_asteroid[i]->GetModel()));
			if (m_asteroid[i]->hasTex) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_asteroid[0]->getTextureID());
				GLuint sampler = m_shader->GetUniformLocation("samp");
				if (sampler == INVALID_UNIFORM_LOCATION)
				{
					printf("Sampler Not found not found\n");
				}
				glUniform1i(sampler, 0);
				glUniform1i(hasN, false);
			}
			if (m_asteroid[i]->hasNormal) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_asteroid[0]->getNormalID());
				GLuint sampler = m_shader->GetUniformLocation("samp1");
				if (sampler == INVALID_UNIFORM_LOCATION) {
					printf("Sampler not found\n");
				}
				glUniform1i(sampler, 1);
				glUniform1i(hasN, true);
			}
			m_asteroid[i]->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
		}
	}
	
	// Render Ship
	setMaterialShip();
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
		if(thirdPer)
			m_mesh->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	/*if (m_pyramid != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_pyramid->GetModel()));
		m_pyramid->Render(m_positionAttrib, m_normalAttrib);
	}*/

	

	// Render Sun
	setMaterialSun();
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

	// Render Comet
	setMaterialComet();
	if (m_comet != NULL) {
		glUniformMatrix4fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(glm::mat3(m_camera->GetView() * m_comet->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_comet->GetModel()));
		if (m_comet->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_comet->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			glUniform1i(hasN, false);
		}
		if (m_comet->hasNormal) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_comet->getNormalID());
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 1);
			glUniform1i(hasN, true);
		}
		m_comet->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, hasN);
	}

	setMaterialRock();

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

	setMaterialGas();
	
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

	m_skyboxProjMatrix = m_skyboxshader->GetUniformLocation("proj_matrix");
	if (m_skyboxProjMatrix == INVALID_UNIFORM_LOCATION) {
		printf("skybox projmatrix not found\n");
		anyProblem = false;
	}

	m_skyboxViewMatrix = m_skyboxshader->GetUniformLocation("v_matrix");
	if (m_skyboxViewMatrix == INVALID_UNIFORM_LOCATION) {
		printf("skybox vmatrix not found\n");
		anyProblem = false;
	}

	m_skyboxPositionAttrib = m_skyboxshader->GetAttribLocation("position");
	if (m_skyboxPositionAttrib == INVALID_UNIFORM_LOCATION) {
		printf("skybox position not found\n");
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

void Graphics::ToggleView(bool thrPer)
{
	thirdPer = thrPer;
	m_camera->ToggleView(thrPer);
}

void Graphics::ToggleOrbit(bool orb)
{
	orbiting = orb;
	glm::vec3 shipCoord = m_camera->GetPosition();
	
	float min = glm::distance(shipCoord, planetLoc[0]);
	orbitIndex = 0;
	for (int i = 1; i < 12; i++)
	{
		float dist = glm::distance(shipCoord, planetLoc[i]);
		if (min > dist)
		{
			orbitIndex = i;
			min = dist;
		}
	}
}

void Graphics::setMaterialSun() {
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient_sun);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff_sun);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec_sun);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess_sun);
}

void Graphics::setMaterialComet() {
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient_comet);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff_comet);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec_comet);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess_comet);
}

void Graphics::setMaterialRock() {
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient_rock);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff_rock);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec_rock);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess_rock);
}

void Graphics::setMaterialGas() {
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient_gas);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff_gas);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec_gas);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess_gas);
}

void Graphics::setMaterialShip() {
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient_ship);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff_ship);
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec_ship);
	glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess_ship);
}

GLuint Graphics::loadCubeMap() {
	GLuint textureID;
	// load each face of cubemap
	std::string xp = "assets\\Cube-xp1.jpg";
	std::string xn = "assets\\Cube-xn1.jpg";
	std::string yp = "assets\\Cube-yp1.jpg";
	std::string yn = "assets\\Cube-yn1.jpg";
	std::string zp = "assets\\Cube-zp1.jpg";
	std::string zn = "assets\\Cube-zn1.jpg";

	// get texture id
	textureID = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(), zp.c_str(), zn.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (textureID == 0) {
		printf("Error loading cubemap\n");
		return -1;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// set parameters (reduce seams)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Graphics::setupSkybox() {
	// 6 faces of skybox
	skyboxVertices = { 
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// buffer setup
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * skyboxVertices.size(), &skyboxVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}