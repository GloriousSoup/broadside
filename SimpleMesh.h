#ifndef __SIMPLEMESH_H__
#define __SIMPLEMESH_H__

#include "CApp.h"

typedef unsigned short INDEX;
#define INDEX_TYPE GL_UNSIGNED_SHORT
class GLMeshPart
{
public:
	int m_indexOffset;
	int m_triangleCount;
	float m_diffuse[4];
	const char* m_diffuseName;
	GLuint m_diffuseTex;
};
class GLMeshContainer
{
public:
	int m_nTotalVerts;
	int m_nTotalInds;
	int m_nParts;
	int m_nVertexStride;
	int m_nNormalOffs;
	int m_nUVOffs;
	float* m_pVertices;
	INDEX* m_pIndices;
	GLMeshPart* m_pParts;
	GLuint m_vbo, m_ibo, m_vao;
	
	GLMeshContainer()
	{
	}
	void BeginDraw();
	void EndDraw();
	int PartCount() const { return m_nParts; }
	const float* PartDiffuse(int n)
	{
		return m_pParts[n].m_diffuse;
	}
	const char* PartDiffuseName(int n)
	{
		return m_pParts[n].m_diffuseName;
	}
	GLuint PartDiffuseTexture(int n)
	{
		return m_pParts[n].m_diffuseTex;
	}
	void SetDiffuseToUniform(int n, GLuint uniform, float alpha = 1.0f)
	{
		const float* pDiffuse = PartDiffuse(n);
		float pGreen[] = { 0,1,0,1 };
		if (pDiffuse == NULL)
			pDiffuse = pGreen;
        float newDiffuse[] = {pDiffuse[0], pDiffuse[1], pDiffuse[2], pDiffuse[3]*alpha};
		glUniform4fv(uniform, 1, newDiffuse);
	}
	void Draw(int n);
	void Import(FILE* f);
};

class GLMeshList
{
public:
	GLMeshContainer* m_pMeshes[256];
	int m_nMeshCount;
	float m_fMeshFPS;
	float m_fMoveRate;
	GLMeshList() { m_nMeshCount = 0; m_fMeshFPS = 12.0f; m_fMoveRate = 0.0f; }
	//void AddMesh(FbxNode* pNode) { m_pMeshes[m_nMeshCount++] = new GLMeshContainer(pNode); }
	int MeshCount() { return m_nMeshCount; }
	float MeshFPS() { return m_fMeshFPS; }
	float MeshMoveRate() { return m_fMoveRate; }
	void Import(const char* pFilename);
	GLMeshContainer* Mesh(int n) { return m_pMeshes[n]; }
};

GLMeshList* LoadSceneMeshes(const char* pName, float fFPS = 0.0f, float fMoveRate = 0.0f);

#endif // __SIMPLEMESH_H__
