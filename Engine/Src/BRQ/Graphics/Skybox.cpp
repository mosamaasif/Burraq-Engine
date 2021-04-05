#include <BRQ.h>

#include "Skybox.h"
#include "Mesh.h"

namespace BRQ {

	Skybox::Skybox()
		: m_Size(100.0f) {
	}

	Skybox::Skybox(F32 size)
		: m_Size(size) {
	}

	void Skybox::Load() {

		Init();
	}

	void Skybox::DestroyMesh() {

		m_SkyboxMesh.DestroyMesh();
	}

	void Skybox::Init() {

		MeshData meshData;

		meshData.Verticies = {

			-m_Size, -m_Size,  m_Size,
			-m_Size,  m_Size,  m_Size,
			 m_Size, -m_Size,  m_Size,
		     m_Size,  m_Size,  m_Size,

			-m_Size, -m_Size, -m_Size,
			 m_Size, -m_Size, -m_Size,
			-m_Size,  m_Size, -m_Size,
			 m_Size,  m_Size, -m_Size,

			-m_Size, -m_Size, -m_Size,
			-m_Size,  m_Size, -m_Size,
			-m_Size, -m_Size,  m_Size,
			-m_Size,  m_Size,  m_Size,

			 m_Size, -m_Size,  m_Size,
			 m_Size,  m_Size,  m_Size,
			 m_Size, -m_Size, -m_Size,
			 m_Size,  m_Size, -m_Size,

			-m_Size,  m_Size,  m_Size,
			-m_Size,  m_Size, -m_Size,
			 m_Size,  m_Size,  m_Size,
			 m_Size,  m_Size, -m_Size,

			-m_Size, -m_Size, -m_Size,
			-m_Size, -m_Size,  m_Size,
			 m_Size, -m_Size, -m_Size,
			 m_Size, -m_Size,  m_Size
		};

		meshData.Indicies.reserve(6 * 6);

		U32 offset = 0;

		for (U32 i = 0; i < 6 * 6; i++) {
			
			// CCW { 0, 2, 1, 2, 3, 1 }

			meshData.Indicies.push_back(0 + offset);
			meshData.Indicies.push_back(2 + offset);
			meshData.Indicies.push_back(1 + offset);
			meshData.Indicies.push_back(2 + offset);
			meshData.Indicies.push_back(3 + offset);
			meshData.Indicies.push_back(1 + offset);

			offset += 4;
		}

		m_SkyboxMesh.LoadMesh(meshData);
	}
}