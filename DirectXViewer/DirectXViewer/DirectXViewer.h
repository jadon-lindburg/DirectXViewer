#pragma once

#include "debug.h"

#include <Windows.h>
#include <array>
#include <cstdint>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>

#include "colors.h"


using namespace DirectX;


namespace DirectXViewer
{
#pragma region Defines
	using filepath_t = std::array<char, 260>;

#define IBUFFER_FORMAT DXGI_FORMAT_R32_UINT
#pragma endregion


#pragma region Structs
	// DXV vertex data container
	struct DXVVERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT3 norm;
		XMFLOAT4 color;
		XMFLOAT2 uv;
	};

	// DXV mesh data container
	struct DXVMESHDATA
	{
		uint32_t		vertexCount = 0;
		uint32_t		indexCount = 0;
		DXVVERTEX*		vertices = nullptr;
		uint32_t*		indices = nullptr;

		~DXVMESHDATA()
		{
			delete[] vertices;
			delete[] indices;
		}
	};

	// DXV rendering mesh
	// NOTES:
	//  Must be contained in a DXVOBJECT along with a DXVMATERIAL to be added to scene
	struct DXVMESH
	{
		uint32_t		vertexCount = 0;
		uint32_t		indexCount = 0;
		ID3D11Buffer*	vertexBuffer_p = nullptr;
		ID3D11Buffer*	indexBuffer_p = nullptr;

		~DXVMESH()
		{
			if (indexBuffer_p) indexBuffer_p->Release();
			if (vertexBuffer_p) vertexBuffer_p->Release();
		}
	};

	// DXV material data container
	struct DXVMATERIALDATA
	{
		struct COMPONENT
		{
			XMFLOAT3	value = { 0.0f, 0.0f, 0.0f };
			float		factor = 0.0f;
			char		filepath[260];
		};

		enum ComponentType_e
		{
			Diffuse = 0
			, Emissive
			, Specular
			, Normalmap
			, Count
		};

		COMPONENT operator[](int i) { return components[i]; }
		const COMPONENT operator[](int i) const { return components[i]; }

		COMPONENT components[ComponentType_e::Count];
	};

	// DXV rendering material
	// NOTES:
	//  Must be contained in a DXVOBJECT along with a DXVMESH to be added to scene
	struct DXVMATERIAL
	{
		struct COMPONENT
		{
			XMFLOAT3					value = { 0.0f, 0.0f, 0.0f };
			float						factor = 0.0f;
			ID3D11Resource*				texture_p = nullptr;
			ID3D11ShaderResourceView*	textureView_p = nullptr;
		};

		enum ComponentType_e
		{
			Diffuse = 0
			, Emissive
			, Specular
			, NormalMap
			, Count
		};

		~DXVMATERIAL()
		{
			for (int32_t i = ComponentType_e::Count - 1; i >= 0; i--)
			{
				if (components[i].textureView_p) components[i].textureView_p->Release();
				if (components[i].texture_p) components[i].texture_p->Release();
			}
		}

		COMPONENT operator[](int i) { return components[i]; }
		const COMPONENT operator[](int i) const { return components[i]; }

		COMPONENT components[ComponentType_e::Count];
	};

	// DXV rendering object
	// NOTES:
	//  Must have both mesh and material to be used by scene
	struct DXVOBJECT
	{
		XMMATRIX		modeling = XMMatrixIdentity();
		DXVMESH*		mesh_p = nullptr;
		DXVMATERIAL*	material_p = nullptr;

		~DXVOBJECT()
		{
			material_p = nullptr;
			mesh_p = nullptr;
		}
	};

	// Vertex shader constant buffer data container
	struct DXVCBUFFER_VS
	{
		XMMATRIX	world;
		XMMATRIX	view;
		XMMATRIX	projection;
		XMMATRIX	worldIT;
	};

	// Pixel shader constant buffer data container
	struct DXVCBUFFER_PS
	{
		XMFLOAT4	cam_pos;
		XMFLOAT3	light_pos;
		XMFLOAT3	light_color;
		float		light_power;
		float		surface_shininess;
	};
#pragma endregion


#pragma region Getters
	// Returns the default world matrix
	XMMATRIX GetDefaultWorldMatrix();

	// Returns the default view matrix
	XMMATRIX GetDefaultViewMatrix();

	// Returns the default projection matrix
	XMMATRIX GetDefaultProjectionMatrix();

	// Returns a pointer to the device
	ID3D11Device* GetDevice();

	// Returns a pointer to the device context
	ID3D11DeviceContext* GetDeviceContext();

	// Returns a pointer to the swap chain
	IDXGISwapChain* GetSwapChain();

	// Returns the last error message generated
	const char* GetLastError();
#pragma endregion

#pragma region Setters
	// Sets the default world matrix
	void SetDefaultWorldMatrix(XMMATRIX _m);

	// Sets the default view matrix
	void SetDefaultViewMatrix(XMMATRIX _m);

	// Sets the default projection matrix
	void SetDefaultProjectionMatrix(XMMATRIX _m);

	// Sets the world matrix in the D3D vertex shader constant buffer
	void SetCurrentWorldMatrix(XMMATRIX _m);

	// Sets the view matrix in the D3D vertex shader constant buffer
	void SetCurrentViewMatrix(XMMATRIX _m);

	// Sets the projection matrix in the D3D vertex shader constant buffer
	void SetCurrentProjectionMatrix(XMMATRIX _m);


	// Sets the current D3D vertex buffer
	void D3DSetVertexBuffer(ID3D11Buffer** _vbuffer_pp);

	// Sets the current D3D index buffer
	void D3DSetIndexBuffer(ID3D11Buffer* _ibuffer_p);

	// Sets the current D3D diffuse material
	void D3DSetDiffuseMaterial(ID3D11ShaderResourceView* _material_p);

	// Sets the current D3D emissive material
	void D3DSetEmissiveMaterial(ID3D11ShaderResourceView* _material_p);

	// Sets the current D3D specular material
	void D3DSetSpecularMaterial(ID3D11ShaderResourceView* _material_p);

	// Sets the current D3D normal map material
	void D3DSetNormalMapMaterial(ID3D11ShaderResourceView* _material_p);

	// Sets the color to clear the screen to
	void D3DSetClearToColor(float _color[4]);


	// Sets the current D3D vertex resources from a DXVMESH
	void DXVSetMesh(DXVMESH* _mesh_p);

	// Sets the current D3D pixel resources from a DXVMATERIAL
	void DXVSetMaterial(DXVMATERIAL* _material_p);

	// Sets the current D3D drawing resources from a DXVOBJECT
	void DXVSetObject(DXVOBJECT* _object_p);
#pragma endregion

#pragma region Basic Functions
	// Creates and initializes automatically created D3D and DXV resources
	// Additional resources must be created manually after this function is called
	HRESULT Init(HWND* _hWnd_p);

	// Handles updates for automatically created D3D and DXV resources
	// Additional updates must be done manually after this function is called
	void Update(const MSG* _msg);

	// Frees memory used by automatically created D3D and DXV resources
	// Additional heap memory must be cleared manually before this function is called
	void Cleanup();
#pragma endregion

#pragma region Draw Functions
	// Handles drawing of active DXV resources
	// Additional draws must be done manually after this function is called
	// NOTES:
	//  Pass false if you are manually drawing anything, then call Present() after manual draws
	void Draw(bool _present = true);

	// Presents the scene to the window
	// NOTES:
	//  Calling this function is only needed if you are drawing anything manually
	void Present(UINT _syncInterval = 1, UINT _flags = 0);


	// Draw raw vertices of current D3D vertex buffer
	void D3DDraw(uint32_t _numVerts);

	// Draw indexed vertices of current D3D vertex and index buffer
	void D3DDrawIndexed(uint32_t _numInds);
#pragma endregion

#pragma region Mesh/Material/Object Functions
	// Loads mesh data from file into a DXVMESHDATA
	HRESULT DXVLoadMeshData(const char* _filepath, DXVMESHDATA** _meshdata_pp);

	// Creates and stores a DXVMESH from a DXVMESHDATA
	HRESULT DXVCreateMesh(const DXVMESHDATA* _meshdata_p, DXVMESH** _mesh_pp);

	// Loads mesh data from file into a DXVMESHDATA and creates a DXVMESH from it
	HRESULT DXVLoadAndCreateMesh(const char* _filepath, DXVMESHDATA** _meshdata_pp, DXVMESH** _mesh_pp);


	// Loads material data from file into a DXVMATERIALDATA
	// NOTES:
	//  If the .mat file passed in contains multiple materials, this function will return an array of DXVMATERIALDATAs
	HRESULT DXVLoadMaterialData(const char* _filepath, DXVMATERIALDATA** _matdata_pp);

	// Creates and stores a DXVMATERIAL from a DXVMATERIALDATA
	HRESULT DXVCreateMaterial(const DXVMATERIALDATA* _matdata_p, DXVMATERIAL** _material_pp);

	// Loads material data from file into a DXVMATERIALDATA	and creates a DXVMATERIAL from it
	// NOTES:
	//  Should only be used for .mat files with only one material in them
	HRESULT DXVLoadAndCreateMaterial(const char* _filepath, DXVMATERIALDATA** _matdata_pp, DXVMATERIAL** _material_pp);


	// Loads mesh and material data into a DXVMESHDATA and DXVMATERIALDATA,
	// creates a DXVMESH and DXVMATERIAL from them, and stores them in the passed in DXVOBJECT
	HRESULT DXVLoadAndCreateObject(
		const char* _mesh_filepath, const char* _mat_filepath,
		DXVMESHDATA** _meshdata_pp, DXVMESH** _mesh_pp,
		DXVMATERIALDATA** _matdata_pp, DXVMATERIAL** _material_pp,
		DXVOBJECT* _object_p);
#pragma endregion

#pragma region Scene Functions
	// Adds an object to the scene
	void AddObjectToScene(DXVOBJECT* _obj_p);

	// Returns the specified object from the scene if index is valid
	DXVOBJECT* GetObjectFromScene(uint16_t _i);

	// Checks the scene for the object specified and removes it if found
	void RemoveObjectFromScene(DXVOBJECT* _obj_p);
#pragma endregion

#pragma region D3D Helper Functions
	// Creates and stores a D3D11 depth stencil and corresponding depth stencil view
	HRESULT D3DCreateDepthStencilView(uint32_t _w, uint32_t _h, ID3D11Texture2D** _depthStencil_pp, ID3D11DepthStencilView** _depthStencilView_pp);

	// Creates and stores a D3D11 sampler state
	// DEFAULTS:
	//  _addressU = D3D11_TEXTURE_ADDRESS_WRAP
	//  _addressV = D3D11_TEXTURE_ADDRESS_WRAP
	//  _addressW = D3D11_TEXTURE_ADDRESS_CLAMP (W coord is not used by DirectXViewer)
	//  _comp = D3D11_COMPARISON_NEVER
	//  _minLod = 0.0f
	//  _maxLod = D3D11_FLOAT_MAX
	HRESULT D3DCreateSamplerState(
		ID3D11SamplerState** _samplerState_pp, D3D11_FILTER _filter,
		D3D11_TEXTURE_ADDRESS_MODE _addressU = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_MODE _addressV = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC _comp = D3D11_COMPARISON_NEVER, float _minLod = 0.0f, float _maxLod = D3D11_FLOAT32_MAX);

	// Creates and stores a D3D11 constant buffer
	HRESULT D3DCreateConstantBuffer(uint32_t _bytewidth, ID3D11Buffer** _cbuffer_pp);

	// Creates and stores a D3D11 vertex buffer from a DXV vertex array
	HRESULT D3DCreateVertexBuffer(uint32_t _vertCount, DXVVERTEX* _verts, ID3D11Buffer** _vbuffer_pp);

	// Creates and stores a D3D11 index buffer from a DXV vertex array
	HRESULT D3DCreateIndexBuffer(uint32_t _indCount, uint32_t* _inds, ID3D11Buffer** _ibuffer_pp);

	// Initializes a D3D11 viewport
	// DEFAULTS:
	//  _topLeftX = 0
	//  _topLeftY = 0
	//  _minDepth = 0.0f
	//  _maxDepth = 1.0f
	void D3DConfigureViewport(D3D11_VIEWPORT* _viewport_p, float _w, float _h, float _topLeftX = 0.0f, float _topLeftY = 0.0f, float _minDepth = 0.0f, float _maxDepth = 1.0f);


	// Updates the vertex shader constant buffer subresource
	void UpdateVSConstantBuffer();

	// Updates the pixel shader constant buffer subresource
	void UpdatePSConstantBuffer();
#pragma endregion

}
