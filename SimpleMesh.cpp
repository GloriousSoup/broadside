#include "render.h"

GLuint GLReadTexture(const char* pFilename);

GLMeshList* LoadSceneMeshes(const char* pName, float fFPS, float fMoveRate)
{
	GLMeshList* pList =  new GLMeshList();
	
	pList->Import(pName);
	pList->m_fMoveRate = fMoveRate;
	pList->m_fMeshFPS = fFPS;
	
	return pList;
}

extern const char* ConvertFilename(const char* p);
void GLMeshList::Import(const char* pFilename)
{
	int magic, version;
	//pFilename = ConvertFilename(pFilename);
	FILE* f = fopen(pFilename, "rb");
	if (f == NULL) return;
	
	fread(&magic, 1, 4, f); if (magic != 0x71712727) return;
	fread(&version, 1, 4, f); if (version < 1) return;
	fread(&m_nMeshCount, 1, 4, f);
	fread(&m_fMeshFPS, 1, 4, f);
	fread(&m_fMoveRate, 1, 4, f);
	
	//printf("MeshList %s version %d meshes %d\n", pFilename, version, m_nMeshCount);
	int i;
	for (i = 0; i < m_nMeshCount; i ++)
	{
		m_pMeshes[i] = new GLMeshContainer();
		m_pMeshes[i]->Import(f);
	}
	fclose(f);
}


#define ENABLE_VAOS 0
#define ENABLE_VBOS 1
#define ENABLE_IBOS 1

void GLMeshContainer::Import(FILE* f)
{
	int magic, version;
	fread(&magic, 1, 4, f); if (magic != 0x13120981) return;
	fread(&version, 1, 4, f); if (version < 1) return;
	//printf(" sub-mesh: version %d\n", version);
	fread(&m_nTotalVerts, 1, 4, f);
	fread(&m_nTotalInds, 1, 4, f);
	fread(&m_nParts, 1, 4, f);
	fread(&m_nVertexStride, 1, 4, f);
	fread(&m_nNormalOffs, 1, 4, f);
	fread(&m_nUVOffs, 1, 4, f);
	m_pVertices = new float[m_nVertexStride*m_nTotalVerts];
	m_pIndices = new INDEX[m_nTotalInds];
	m_pParts = new GLMeshPart[m_nParts];
	fread(m_pVertices, 1, m_nVertexStride*sizeof(float)*m_nTotalVerts, f);
	fread(m_pIndices, 1, sizeof(INDEX)*m_nTotalInds, f);
	int i;
	for (i = 0; i < m_nParts; i ++)
	{
		fread(&m_pParts[i].m_indexOffset, 1, 4, f);
		fread(&m_pParts[i].m_triangleCount, 1, 4, f);
		fread(&m_pParts[i].m_diffuse, 1, 4*4, f);
		//m_pParts[i].m_diffuseName = NULL;
		m_pParts[i].m_diffuseTex = 0xFFFFFFFF;
		if (version >= 2)
		{
			char tempName[256];
			fread(tempName, 1, 256, f);
			//if (tempName[0])
				//printf("tex name %s\n", tempName);
			//m_pParts[i].m_diffuseName = strdup(tempName);
			m_pParts[i].m_diffuseTex = GLReadTexture(tempName);
		}
	}
	
	m_vbo = 0;
	m_ibo = 0;
	m_vao = 0;
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	GLuint vao = 0;
#if ENABLE_VAOS
	if (ENABLE_VAOS)
	{
		glBindVertexArrayOES(0);
		glGenVertexArraysOES(1, &vao);
		glBindVertexArrayOES(vao);
	}
#endif
	
	if (ENABLE_VBOS)
	{
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_nVertexStride*sizeof(float)*m_nTotalVerts, m_pVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_pVertices = NULL;
	}
	if (ENABLE_IBOS)
	{
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nTotalInds * sizeof(INDEX), m_pIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		m_pIndices = NULL;
	}
	
	if (vao)
	{
		BeginDraw();
		m_vao = vao;
		EndDraw();
	}
}

const int VERTEX_STRIDE = 3;
const int NORMAL_STRIDE = 3;
const int UV_STRIDE = 2;
void GLMeshContainer::BeginDraw()
{
#if ENABLE_VAOS
	if (m_vao != 0)
		glBindVertexArrayOES(m_vao);
	else
#endif
	{
		GLfloat* pBase = (GLfloat*)m_pVertices;
		
		if (m_vbo != 0) glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		
		glEnableVertexAttribArray(ATTR_VERTEX);
		glVertexAttribPointer(ATTR_VERTEX, VERTEX_STRIDE, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * m_nVertexStride, pBase+0);
		
		// Set normal array.
		if (m_nNormalOffs)
		{
			glEnableVertexAttribArray(ATTR_NORMAL);
			glVertexAttribPointer(ATTR_NORMAL, NORMAL_STRIDE, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * m_nVertexStride, pBase+m_nNormalOffs);
		}
		else
			glDisableVertexAttribArray(ATTR_NORMAL);
		
		// Set UV array.
		if (m_nUVOffs)
		{
			glEnableVertexAttribArray(ATTR_UV);
			glVertexAttribPointer(ATTR_UV, UV_STRIDE, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * m_nVertexStride, pBase+m_nUVOffs);
		}
		else
			glDisableVertexAttribArray(ATTR_UV);
		
		glDisableVertexAttribArray(ATTR_COLOR);
	}
	if (m_ibo != 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
}

void GLMeshContainer::EndDraw()
{
#if ENABLE_VAOS
	if (m_vao != 0) glBindVertexArrayOES(0);
#endif
	if (m_vbo != 0) glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (m_ibo != 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLMeshContainer::Draw(int n)
{
	INDEX* pInds = (INDEX*)m_pIndices;
	pInds += m_pParts[n].m_indexOffset;
	int nElementCount = m_pParts[n].m_triangleCount * 3;
	glDrawElements(GL_TRIANGLES, nElementCount, INDEX_TYPE, pInds);
}

