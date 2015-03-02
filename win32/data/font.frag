varying vec4 Color;
varying vec2 Tex;
uniform sampler2D gSampler;

void main() {
	vec4 textureColor = texture2D(gSampler, Tex);
	gl_FragColor = Color;
	gl_FragColor = Color * textureColor;
}
