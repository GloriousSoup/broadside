varying vec4 Color;
varying vec2 Tex;

void main() {
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	Color = gl_Color;
	Tex = vec2(gl_MultiTexCoord0);
	//Tex.xy = mod( gl_Vertex.xy * 0.1, 1.0 );
}
