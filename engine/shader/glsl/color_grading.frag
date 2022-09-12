#version 310 es

#extension GL_GOOGLE_include_directive : enable

#include "constants.h"

layout(input_attachment_index = 0, set = 0, binding = 0) uniform highp subpassInput in_color;

layout(set = 0, binding = 1) uniform sampler2D color_grading_lut_texture_sampler;

layout(location = 0) out highp vec4 out_color;
/*
void main()
{
    highp ivec2 lut_tex_size = textureSize(color_grading_lut_texture_sampler, 0);

    // 除了 color_grading_LUT.jpg 是 32 外, _COLORS 都是 16
    // x 大小为 _COLORS^2 
    // y轴方向取样绿色
    // x轴方向取样红色
    // x轴/_COLORS取样蓝色
    highp float _COLORS     = float(lut_tex_size.y);
    

    highp vec4 color        = subpassLoad(in_color).rgba;
    
    // texture(color_grading_lut_texture_sampler, uv)

    // 放大色彩值到采样空间
    highp float red         = (_COLORS-1.0) * color.r;
    highp float green       = (_COLORS-1.0) * color.g;
    highp float blue        = (_COLORS-1.0) * color.b;

    // 取蓝色值上下rglut格起点
    highp float blue_f   = floor(blue);
    highp float blue_c   = ceil(blue);

    highp float lutx = float(lut_tex_size.x);

    // 定义采样uv
    highp float v = green / _COLORS;
    highp float u_f = (red + blue_f * _COLORS) / lutx;
    highp float u_c = (red + blue_c * _COLORS) / lutx;

    // 采样颜色
    highp vec3 lut_color_f = texture(
        color_grading_lut_texture_sampler, vec2(u_f,v)).rgb;

    highp vec3 lut_color_c = texture(
        color_grading_lut_texture_sampler, vec2(u_c,v)).rgb;


    highp vec3 mixed_color = mix(lut_color_f,lut_color_c,fract(blue));


    out_color = vec4(mixed_color, color.a);
}
*/

void main()
{
    highp ivec2 lut_tex_size = textureSize(color_grading_lut_texture_sampler, 0);

    
    // x 大小为 _COLORS^2 
    // y轴方向取样绿色
    // x轴方向取样红色
    // x轴/_COLORS取样蓝色
    // highp float _COLORS     = float(lut_tex_size.x)/8;
    
    // 512*512的图，这咋办嘛，先强制定义了
    highp float _COLORS     = float(lut_tex_size.x) / 8.0; // = 64
    highp float lutx = float(lut_tex_size.x);


    highp vec4 color        = subpassLoad(in_color).rgba;
    
    // texture(color_grading_lut_texture_sampler, uv)

    // 放大色彩值到采样空间
    highp float red         = (_COLORS-1.0) * color.r;
    highp float green       = (_COLORS-1.0) * color.g;
    highp float blue        = (_COLORS-1.0) * color.b;

    // 取蓝色值上下rglut格起点
    highp float blue_f   = floor(blue);
    highp float blue_c   = ceil(blue);


    // v，纵向取绿，蓝色已占的行数为v方向偏移
    highp float v_f = (green + floor(blue_f / 8.0)) / lutx;
    highp float v_c = (green + floor(blue_c / 8.0)) / lutx;

    // u，横向取红，蓝色对八格余数为u方向偏移
    // highp float u_f = (red + fmod(blue_f, 8.0)) / lutx;
    // highp float u_c = (red + fmod(blue_c, 8.0)) / lutx;
    highp float u_f = (red + (blue_f - floor(blue_f / 8.0) * 8.0)) / lutx;
    highp float u_c = (red + (blue_c - floor(blue_c / 8.0) * 8.0)) / lutx;

    // 采样颜色
    highp vec3 lut_color_f = texture(
        color_grading_lut_texture_sampler, vec2(u_f,v_f)).rgb;

    highp vec3 lut_color_c = texture(
        color_grading_lut_texture_sampler, vec2(u_c,v_c)).rgb;


    highp vec3 mixed_color = mix(lut_color_f,lut_color_c,fract(blue));


    out_color = vec4(mixed_color, color.a);
}