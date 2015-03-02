uniform float gTime;
varying vec4 Color;
varying vec3 Normal;
varying vec3 Pos;
varying vec2 Tex;

void main() {
	Pos = gl_Vertex.xyz;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	Color = gl_Color;
	Normal = gl_Normal;
	Tex = vec2(gl_MultiTexCoord0);
	Pos = gl_Vertex.xyz;
}
