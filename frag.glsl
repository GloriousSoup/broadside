varying vec4 Color;
varying vec3 Normal;
varying vec3 Pos;
varying vec2 Tex;
uniform sampler2D gSampler;

void main() {
	vec4 textureColor = texture2D(gSampler, Tex);
	//if( textureColor.a < 0.1)
	//	discard;
	//float l = dot( Normal, vec3( 1.0, 1.0, -1.0 ) );
	//float b = 0.5 + 0.5 * clamp( l, 0.0, 1.0 );
	gl_FragColor = Color * textureColor;
}
