#include "Renderer.h"

RendererAnimationScene::RendererAnimationScene(LayersDataProvider* layersData) : layersData(layersData) {

	TextureSetting ts;
	ts.Min = TextureFilter::LINEAR;
	ts.Max = TextureFilter::LINEAR;
	ts.WrapX = TextureWrap::CLAMP_TO_EDGE;
	ts.WrapY = TextureWrap::CLAMP_TO_EDGE;

	const int width = 2560;
	const int height = 1440;

	output_texture = new Texture(ts, 0, TextureInternalPixelFormat::RGBA8, width, height, TexturePixelFormat::RGBA, TextureDataType::_BYTE, nullptr);
	renderbuffer_depth = new Renderbuffer(TextureInternalPixelFormat::DEPTH_COMPONENT, width, height);

	output_framebuffer = new Framebuffer;
	output_framebuffer->AttachTexture(*output_texture, Buffer_Attachment::COLOR_ATTACHMENT0);
	output_framebuffer->AttachRenderBuffer(*renderbuffer_depth, Buffer_Attachment::DEPTH_ATTACHMENT);
	output_framebuffer->SetCountDrawBuffers(1);
	

	shader = new Shader( "SHADER_DEFAULT_RENDERER",
R"VERTEX(

#version 330 core

layout(location = 0) in vec3 localPos;
layout(location = 1) in vec2 localUV;

layout (std140) uniform MainBlock{
	mat4 pv;
	float t;
	float dt;
	vec2 res;
};

uniform mat4 u_model;
uniform vec4 u_color;

out vec4 color;


void main(){
	gl_Position = pv * u_model * vec4(localPos,1.0);
	color = u_color;
}

)VERTEX",



R"FRAG(
#version 330

in vec4 color;

out vec4 fragColor;

void main() {
    fragColor = color;
}

)FRAG");


	shader_grid = new Shader("SHADER_GRID_VIEWPORT",
R"VERTEX(
#version 330 core
out vec2 v_ndc;
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    v_ndc = p * 2.0 - 1.0;
    gl_Position = vec4(v_ndc, 0.0, 1.0);
}
)VERTEX",

R"FRAG(
#version 330 core

layout (std140) uniform MainBlock {
    mat4  pv;
    float t;
    float dt;
    vec2  res;
};

uniform mat4 u_invPV;
uniform vec3 u_camPos;

uniform float u_cell      = 1.0;
uniform int   u_major     = 5;
uniform float u_fadeStart = 10.0;
uniform float u_fadeEnd   = 180.0;

in vec2 v_ndc;
out vec4 fragColor;

float gridLine(vec2 q) {
    vec2 w = fwidth(q);
    vec2 a = abs(fract(q - 0.5) - 0.5) / w;
    return 1.0 - min(min(a.x, a.y), 1.0);
}

void main() {
    vec4 f = u_invPV * vec4(v_ndc,  1.0, 1.0);
    vec4 n = u_invPV * vec4(v_ndc, -1.0, 1.0);
    vec3 ro = u_camPos;
    vec3 rd = normalize(f.xyz / f.w - n.xyz / n.w);

    if (abs(rd.y) < 1e-6) discard;
    float dist = -ro.y / rd.y;
    if (dist <= 0.0) discard;

    vec3 P = ro + rd * dist;
    vec2 p = P.xz;

    vec4 clip  = pv * vec4(P, 1.0);
    float depth = (clip.z / clip.w) * 0.5 + 0.5;
    if (depth <= 0.0 || depth >= 1.0) discard;
    gl_FragDepth = depth;

    float dfw = max(fwidth(p.x), fwidth(p.y));

    float minor = gridLine(p / u_cell)
                * (1.0 - smoothstep(0.35, 0.65, dfw / u_cell));
    float major = gridLine(p / (u_cell * float(u_major)))
                * (1.0 - smoothstep(0.35, 0.65, dfw / (u_cell * float(u_major))));

    float fade = 1.0 - smoothstep(u_fadeStart, u_fadeEnd, dist);

    float ax = 1.0 - min(abs(p.y) / (dfw * 1.5), 1.0);
    float az = 1.0 - min(abs(p.x) / (dfw * 1.5), 1.0);

    vec3  col   = mix(vec3(0.32), vec3(0.60), major);
    float alpha = max(minor, major);

    col   = mix(col, vec3(0.85, 0.25, 0.25), ax);
    alpha = max(alpha, ax);
    col   = mix(col, vec3(0.25, 0.35, 0.90), az);
    alpha = max(alpha, az);

    alpha *= fade;
    if (alpha < 0.004) discard;

    fragColor = vec4(col, alpha);
}
)FRAG");


	ubo.BindUBO(0);
	ubo.SetName("MainBlock");

	ubo.BindProgramm(*shader);
	shader->bindUniformBlock("MainBlock", 0);

	ubo.BindProgramm(*shader_grid);
	shader->bindUniformBlock("MainBlock", 0);

	UpdateUBO();
	ubo.Update(ubo_MainBlock);

	glGenVertexArrays(1, &emptyVAO);

}

RendererAnimationScene::~RendererAnimationScene() {
	output_texture->~Texture();
	delete output_texture;

	output_framebuffer->~Framebuffer();
	delete output_framebuffer;
}

void RendererAnimationScene::UpdateUBO() {

	const float& width = output_texture->GetWidth();
	const float& height = output_texture->GetHeight();

	float aspect = width / height;

	ubo_MainBlock.dt = 0.f;
	ubo_MainBlock.t = 0.f;
	ubo_MainBlock.res = { width,height };
	ubo_MainBlock.pv = glm::perspective(camera.getFOV() * glm::pi<float>()/180.f, aspect, 0.1f,100.f) * camera.getMatrixView();


	inv_pv = glm::inverse(ubo_MainBlock.pv);
	cam_pos = glm::vec3(glm::inverse(camera.getMatrixView())[3]);
}

void RendererAnimationScene::Draw(float time) {
	output_framebuffer->Bind();
	

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(BufferBit::COLOR_BUFFER_BIT | BufferBit::DEPTH_BUFFER_BIT);
	glClearColor(0.f, 1.f, 0.f, 1.f);

	shader->use();
	auto& layers = layersData->GetData().layers;
	for (size_t i = 0; i < layers.size(); i++) {
		auto& layer = layers[i];
		bool is_visible = layer.get()->GetLayerInfo().isVisible;
		if (is_visible == false)
			continue;


		auto c = layer.get()->GetProperties().get<TPColor>("color").kr.evaluate(time);
		shader->set4Float("u_color", { c[0],c[1],c[2],c[3]});

		auto p = layer.get()->GetProperties().get<TP3Float>("pos").kr.evaluate(time);
		auto s = layer.get()->GetProperties().get<TP3Float>("scale").kr.evaluate(time);

		shader->setMat4("u_model", glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(s[0], s[1], s[2])), glm::vec3(p[0], p[1], p[2])));
		layer->Draw(time);
	}


	shader_grid->use();
	glBindVertexArray(emptyVAO);
	shader_grid->setMat4("u_invPV", inv_pv);
	shader_grid->set3Float("u_camPos", { cam_pos.x, cam_pos.y, cam_pos.z });
	glDepthMask(GL_FALSE);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDepthMask(GL_TRUE);

	output_framebuffer->UnBind();

}

void RendererAnimationScene::Update() {
	UpdateUBO();
	ubo.Update(ubo_MainBlock);
}

Texture& RendererAnimationScene::getTexture() {
	return *output_texture;
}

Framebuffer& RendererAnimationScene::getFramebuffer() {
	return *output_framebuffer;
}

Camera_Global& RendererAnimationScene::getCamera() {
	return this->camera;
}