#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_Texture;
in vec2 v_TexCoordinate;

void main() {
    // 根据得到的线性雾化因子，计算实际输出的雾化效果
  outColor =  texture(s_Texture, v_TexCoordinate);
//  outColor = vec4(0.2,0.2,0.3,1);
}
