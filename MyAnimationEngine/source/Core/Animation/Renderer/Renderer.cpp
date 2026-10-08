#include "Renderer.h"

RendererAnimationScene::RendererAnimationScene(LayersDataProvider* layersData) : layersData(layersData) {

	TextureSetting ts;
	ts.Min = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.Max = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.WrapX = TextureWrap::CLAMP_TO_EDGE;
	ts.WrapY = TextureWrap::CLAMP_TO_EDGE;

	const int width = 2560;
	const int height = 1440;

	output_texture = new Texture(ts, 0, TextureInternalPixelFormat::RGBA, width, height, TexturePixelFormat::RGBA, TextureDataType::Byte, nullptr);
	renderbuffer_depth = new Renderbuffer(TextureInternalPixelFormat::DEPTH_COMPONENT, width, height);

	output_framebuffer = new Framebuffer;
	output_framebuffer->AttachTexture(*output_texture, FramebufferAttachment::COLOR_ATTACHMENT0);
	output_framebuffer->AttachRenderBuffer(*renderbuffer_depth, FramebufferAttachment::DEPTH_ATTACHMENT);
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

uniform float u_cell       = 1.0;
uniform int   u_major      = 5;
uniform float u_fadeStart  = 10.0;
uniform float u_fadeEnd    = 180.0;
uniform float u_minorWidth = 0.02;
uniform float u_majorWidth = 0.03;

in vec2 v_ndc;
out vec4 fragColor;

vec2 footprint(vec2 p) {
    vec2 ddx = dFdx(p);
    vec2 ddy = dFdy(p);
    return vec2(length(vec2(ddx.x, ddy.x)),
                length(vec2(ddx.y, ddy.y)));
}

vec2 lineFamily(vec2 q, vec2 fw, float halfW) {
    vec2 f  = max(fw, vec2(1e-7));
    vec2 d  = abs(fract(q - 0.5) - 0.5);
    vec2 hw = clamp(vec2(halfW), f, vec2(0.5));
    vec2 m  = vec2(1.0) - smoothstep(max(hw - f, vec2(0.0)), hw + f, d);
    m *= clamp(vec2(halfW) / hw, vec2(0.0), vec2(1.0));
    m *= vec2(1.0) - smoothstep(vec2(0.25), vec2(0.45), f);
    return m;
}

void main() {
    vec4 nc = u_invPV * vec4(v_ndc, -1.0, 1.0);
    vec3 rd = nc.xyz / nc.w - u_camPos;

    float dy = rd.y;
    dy = (abs(dy) < 1e-6) ? ((dy < 0.0) ? -1e-6 : 1e-6) : dy;
    float tt   = -u_camPos.y / dy;
    vec3  P    = u_camPos + rd * tt;
    float dist = tt * length(rd);

    vec2 p  = P.xz;
    vec2 fw = footprint(p);

    if (tt <= 0.0 || dist >= u_fadeEnd) discard;

    vec4 clip  = pv * vec4(P, 1.0);
    float depth = (clip.z / clip.w) * 0.5 + 0.5;
    if (depth <= 0.0 || depth >= 1.0) discard;
    gl_FragDepth = depth;

    float mjr = float(u_major);
    float ic  = 1.0 / u_cell;

    vec2 q   = p * ic;
    vec2 fwq = fw * ic;

    vec2 mnr  = lineFamily(q,       fwq,       u_minorWidth);
    vec2 mjl  = lineFamily(q / mjr, fwq / mjr, u_majorWidth / mjr);

    float minor = max(mnr.x, mnr.y);
    float major = max(mjl.x, mjl.y);

    float fade = 1.0 - smoothstep(u_fadeStart, u_fadeEnd, dist);

    vec2 fwa = max(vec2(fw.y, fw.x), vec2(1e-7));
    vec2 axz = vec2(1.0) - smoothstep(vec2(0.5), vec2(1.5), abs(vec2(p.y, p.x)) / fwa);

    vec3  col   = mix(vec3(0.32), vec3(0.60), major);
    float alpha = max(minor, major);

    col   = mix(col, vec3(0.85, 0.25, 0.25), axz.x);
    alpha = max(alpha, axz.x);
    col   = mix(col, vec3(0.25, 0.35, 0.90), axz.y);
    alpha = max(alpha, axz.y);

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

	pixelReader = new AsyncPixelReader();
	pixelReader->init();

	getColorPixel(0, 0);
}

RendererAnimationScene::~RendererAnimationScene() {
	delete output_texture;
	delete output_framebuffer;
	delete shader;
	delete shader_grid;
}

void RendererAnimationScene::Resize(int width, int height) {
	if (width <= 0 || height <= 0) return;
	if (width == output_texture->GetWidth() && height == output_texture->GetHeight()) return;

	output_framebuffer->Resize(width, height);
	Update();
}


void RendererAnimationScene::UpdateUBO() {

	const float& width = output_texture->GetWidth();
	const float& height = output_texture->GetHeight();

	float aspect = width / height;

	ubo_MainBlock.dt = 0.f;
	ubo_MainBlock.t = 0.f;
	ubo_MainBlock.res = { width,height };
	ubo_MainBlock.pv = glm::perspective(camera.getFOV() * glm::pi<float>()/180.f, aspect, 1.f, 100.f) * camera.getMatrixView();


	inv_pv = glm::inverse(ubo_MainBlock.pv);
	cam_pos = glm::vec3(glm::inverse(camera.getMatrixView())[3]);
}


glm::vec3 RendererAnimationScene::getColorPixel(int x, int y) {
	this->x_picker = std::clamp(x, 0, output_texture->GetWidth());
	this->y_picker = std::clamp(y, 0, output_texture->GetHeight());

	return { (float)currentColor_picker.x / 255.f, (float)currentColor_picker.y / 255.f, (float)currentColor_picker.z / 255.f };
}



void RendererAnimationScene::Draw(float time) {

	pixelReader->fetch(currentColor_picker);


	output_framebuffer->Bind();
	glViewport(0, 0, output_texture->GetWidth(), output_texture->GetHeight());

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear((unsigned int)BufferBit::COLOR | (unsigned int)BufferBit::DEPTH);


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

	glBindTexture(GL_TEXTURE_2D, output_texture->GetID());
	glGenerateMipmap(GL_TEXTURE_2D);

	pixelReader->request(output_framebuffer->GetID(), x_picker, y_picker);

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