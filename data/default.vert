varying vec4 Color;

void main() {
	gl_Position = gl_Vertex;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	Color = gl_Color;
}
