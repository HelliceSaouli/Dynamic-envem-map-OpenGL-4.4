#include<iostream>

#include "Camera/vCamera.h"
#include "Transforms/Transforms.h"
#include "Window/Display.h"
#include "Window/Fps.h"
#include "Input/input.h"
#include "Texture/TextureGL.h"
#include "shaders/Shader.h"
#include "Geometry/Mesh.h"


#include "util.h"

int main(void)
{

	//3- reconstruc and render
	Fps FramePerSc;
	input in;
	in.HideMouse(false);
	Display _display(800,600,"Dynamic Envirnoment map");

	Shader _shader("shader programs/SimpleReflection");
	Shader _Simple("shader programs/Simple");
	Shader _SimpleDynamic("shader programs/SimpleReflDynamic",OTHERS,COMPLEX); //render to frame buffer 6 times using geomtry shader
	Shader _CubeShader("shader programs/CubeMap",CUBE_MAP);
	Shader _CubeMapInside("shader programs/CubeMapInsideFBO",CUBE_MAP,COMPLEX); // draw the static envirnement in FBO using Geomtry shader

	GLint attribut[3];
	GLint attribut1[3];
	GLint attribut2[3];

	attribut[0] = _shader.GetAttributeLocation("position");
	attribut[1] = _shader.GetAttributeLocation("texCoord");
	attribut[2] = _shader.GetAttributeLocation("normal");

	attribut1[0] = _Simple.GetAttributeLocation("position");
	attribut1[1] = _Simple.GetAttributeLocation("texCoord");
	attribut1[2] = _Simple.GetAttributeLocation("normal");

	attribut2[0] = _CubeShader.GetAttributeLocation("position");



	Mesh Sphere("resources/Models/Sphere.obj",attribut);
	Mesh Sphere2("resources/Models/bunny.obj",attribut1);
	Mesh CubeMap("resources/Models/Cube.obj",attribut2); //watch out attribute array should not be the same

	TextureGL _SkyBoxMap;
	TextureGL _FBOCubeMap;
	// load texture to save in opengl us samplerCubemap
	std::string ImagesPaths[6]; // they must be in the same order like inside the function cubemaptexture()
	ImagesPaths[0] = "resources/textures/Storforsen2/negz.jpg";	//front
	ImagesPaths[1] = "resources/textures/Storforsen2/posz.jpg"; // back
	ImagesPaths[2] = "resources/textures/Storforsen2/negy.jpg"; //top // i have to change it to be negative
	ImagesPaths[3] = "resources/textures/Storforsen2/posy.jpg"; //bottom
	ImagesPaths[4] = "resources/textures/Storforsen2/negx.jpg"; // left
	ImagesPaths[5] = "resources/textures/Storforsen2/posx.jpg"; // right

	_SkyBoxMap.CubeMapTexture(ImagesPaths); // build our cub map for opengl
	_FBOCubeMap.CreateFBOCubeMap(600,600);
	Transforms modelpos;
	Transforms modelpos2;

	glm::vec3 CamPos    = glm::vec3(0.0,0.0,3.0);
	glm::vec3 CamFront  = glm::vec3(0.0,0.0,-1);
	vCamera   camera(CamPos,glm::vec3(0.0,0.0,0.0),50.0f,800/600,0.01f,10000.0f);




	float phi   = 0.0;
	float theta = -90.0;
	float mouseSpeed = 1.8;
	float walkspeed  = 4.0;

	FramePerSc.Start(); // start counting fps
	while(!in.QUIT())
	{
		_display.Clear(0.0f,0.0,0.0f,0.0f);

		in.updateEvents();

		if(in.mouseMotion() && in.getMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			// calculate the new angle across the vertical and horizantal vectors
			phi   += in.getMouseY() * mouseSpeed * FramePerSc.deltatime();
			theta += in.getMouseX() * mouseSpeed * FramePerSc.deltatime();

			if(phi > 89.0f)
				phi =  89.0f;
			if(phi < -89.0f)
				phi = -89.0f;

			float phiRadian   = DegreeToRad(phi);
			float thetaRadian = DegreeToRad(theta);

			 CamFront = glm::normalize(glm::vec3(
				cos(phiRadian) * cos(thetaRadian),
			    sin(phiRadian),
			    cos(phiRadian) * sin(thetaRadian)
			));


				camera.setTarget(CamPos + CamFront);
				camera.ViewMatrixUpdate();
			//modelpos.rotate(phiRadian,thetaRadian,0.0);
			//modelpos.update();
			//camera.setModelMatrix(modelpos.getModelMatrix());
		}

		if(in.getKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			//walkspeed++;
			//modelpos.translate(0.0,0.0, walkspeed * FramePerSc.deltatime());
			//modelpos.update();
			//camera.setModelMatrix(modelpos.getModelMatrix());

			CamPos += walkspeed * FramePerSc.deltatime() * CamFront;
			camera.setPosition(CamPos);
			camera.setTarget(CamPos + CamFront);
			camera.ViewMatrixUpdate();
		}

		if(in.getKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			//walkspeed--;
			//modelpos.translate(0.0,0.0,walkspeed * FramePerSc.deltatime());
			//modelpos.update();
			//camera.setModelMatrix(modelpos.getModelMatrix());
			CamPos -=  walkspeed * FramePerSc.deltatime() * CamFront;
			camera.setPosition(CamPos);
			camera.setTarget(CamPos + CamFront);
			camera.ViewMatrixUpdate();
		}

		if(in.getKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		{
			//walkspeed++;
			//modelpos.translate(0.0,0.0, walkspeed * FramePerSc.deltatime());
			//modelpos.update();
			//camera.setModelMatrix(modelpos.getModelMatrix());

			CamPos += glm::normalize(glm::cross(CamFront, glm::vec3(0.0,1.0,0.0))) * walkspeed * FramePerSc.deltatime();
			camera.setPosition(CamPos);
			camera.setTarget(CamPos + CamFront);
			camera.ViewMatrixUpdate();
		}

		if(in.getKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		{
			//walkspeed--;
			//modelpos.translate(0.0,0.0,walkspeed * FramePerSc.deltatime());
			//modelpos.update();
			//camera.setModelMatrix(modelpos.getModelMatrix());
			CamPos -= glm::normalize(glm::cross(CamFront, glm::vec3(0.0,1.0,0.0))) * walkspeed * FramePerSc.deltatime();
			camera.setPosition(CamPos);
			camera.setTarget(CamPos + CamFront);
			camera.ViewMatrixUpdate();
		}

		if(in.getKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			 in.exit();
		}

		// render the cube map
	glDepthMask(GL_FALSE);
		camera.update();
		_CubeShader.bind();
		_SkyBoxMap.BindCubeMapTexture();
			CubeMap.Draw();
		_CubeShader.Update(camera,CUBE_MAP);
		_CubeShader.deleteShader();
		_SkyBoxMap.UnbindCubeMapTexutre(); // we don't need this cube map to be drown in the reflected object we need the one in frame buffer
	glDepthMask(GL_TRUE);


		// render the rest of the objects inside FBO
		// the other object drawn in created FBO
	_FBOCubeMap.BindFBOAsRenderTaget();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// to this tric it tooke me 1 and
		glDepthMask(GL_FALSE);
			_CubeMapInside.bind();
			_SkyBoxMap.BindCubeMapTexture();
				CubeMap.Draw();
			_CubeMapInside.Update(camera,CUBE_MAP,COMPLEX);
			_CubeMapInside.deleteShader();
			_SkyBoxMap.UnbindCubeMapTexutre();
		glDepthMask(GL_TRUE);

		  // i have to use tclear when i  have this 2 command in createFBO function : glBindTexture(GL_TEXTURE_CUBE_MAP,0); glBindFramebuffer(GL_FRAMEBUFFER, 0);
			modelpos2.translate(-2.0,0.0,-10.0);
			modelpos2.update();
			camera.setModelMatrix(modelpos2.getModelMatrix());
			_SimpleDynamic.bind();
			//camera.setPosition(glm::vec3(0.0,0.0,-10.0));// the same position us the object mirror // this line coz camera to jump^all over the place so i need to set position manulay tell i fix this later
			_SimpleDynamic.Update(camera,OTHERS,COMPLEX);// changing this from it's original palce got me some result
			 Sphere2.Draw();
			_SimpleDynamic.deleteShader();
	_FBOCubeMap.UbindFBOAsRenderTarget();

		// remmeber to alwz put ur transforms inside the loop this not the old opengl
		// the mirror object drawn in default fbo

		modelpos.scale(1.5,1.5,1.5);
		modelpos.translate(0.0,0.0,-10.0);
		modelpos.update();
		camera.setModelMatrix(modelpos.getModelMatrix());
		camera.update();
		_shader.bind();
		_FBOCubeMap.BindCubeMapTexture();
		 	 Sphere.Draw();
		_shader.Update(camera);
		_shader.deleteShader();
		_FBOCubeMap.UnbindCubeMapTexutre();

		// the other object drawen in default fbo
		modelpos2.translate(-2.0,0.0,-10.0);
		modelpos2.update();
		camera.setModelMatrix(modelpos2.getModelMatrix());
		camera.update();
		_Simple.bind();
			Sphere2.Draw();
		_Simple.Update(camera);
		_Simple.deleteShader();


		FramePerSc.Count();// count
		_display.Update();
		std::cout<<"FPS :"<<(int)FramePerSc.getFPS()<<std::endl;
	}
	_FBOCubeMap.DeleteFrameBuffer();
	return 0;
}
