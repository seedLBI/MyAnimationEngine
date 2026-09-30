#ifndef RENDERER_H
#define RENDERER_H

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Camera/Global/Global.Camera.h"

#include "Utils/OpenGL/Framebuffer/Framebuffer.h"
#include "Utils/OpenGL/Texture/Texture.h"
#include <Utils/OpenGL/Renderbuffer/Renderbuffer.h>

#include <Utils/OpenGL/Shaders/Shader.h>
#include <Utils/OpenGL/UBO/UBO.h>



#include <glm/glm.hpp>

class RendererAnimationScene {
public:
	RendererAnimationScene(LayersDataProvider* layersData);
	~RendererAnimationScene();

	void Draw(float time);
	void Update();

	Texture& getTexture();
	Framebuffer& getFramebuffer();
	Camera_Global& getCamera();

private:
	LayersDataProvider* layersData;
	Framebuffer* output_framebuffer;
	Renderbuffer* renderbuffer_depth;
	Texture* output_texture;
	Shader* shader;

	Shader* shader_grid;
	glm::mat4 inv_pv;
	glm::vec3 cam_pos;
	GLuint emptyVAO;

	Camera_Global camera;

	struct UBO_MainBlock {
		glm::mat4 pv;
		float t;
		float dt;
		glm::vec2 res;
	} ubo_MainBlock;

	void UpdateUBO();

	UBO<UBO_MainBlock> ubo;

};




#endif