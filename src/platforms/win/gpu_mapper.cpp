#include "common/gpu_mapper.h"
#include <windows.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <GL/gl.h>
#include <iostream>

typedef HANDLE (WINAPI * PFNWGLDXOPENDEVICENVPROC) (void *dxDevice);
typedef HANDLE (WINAPI * PFNWGLDXREGISTEROBJECTNVPROC) (HANDLE hDevice, void *dxObject, GLuint name, GLenum type, GLenum access);
typedef BOOL (WINAPI * PFNWGLDXLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE *hObjects);
typedef BOOL (WINAPI * PFNWGLDXUNLOCKOBJECTSNVPROC) (HANDLE hDevice, GLint count, HANDLE *hObjects);
typedef BOOL (WINAPI * PFNWGLDXUNREGISTEROBJECTNVPROC) (HANDLE hDevice, HANDLE hObject);

ID3D11Device* g_d3dDevice = nullptr;
HANDLE g_hInteropDevice = nullptr;
HANDLE g_hInteropObject = nullptr;
GLuint g_glTexture = 0;

PFNWGLDXOPENDEVICENVPROC wglDXOpenDeviceNV = nullptr;
PFNWGLDXREGISTEROBJECTNVPROC wglDXRegisterObjectNV = nullptr;
PFNWGLDXLOCKOBJECTSNVPROC wglDXLockObjectsNV = nullptr;
PFNWGLDXUNLOCKOBJECTSNVPROC wglDXUnlockObjectsNV = nullptr;
PFNWGLDXUNREGISTEROBJECTNVPROC wglDXUnregisterObjectNV = nullptr;

void init_graphics_bridge() {
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    
    if (FAILED(hr)) {
        std::cout << "[GPU Bridge] FAILED to create D3D11 Device!" << std::endl;
        return;
    }

    wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)wglGetProcAddress("wglDXOpenDeviceNV");
    wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)wglGetProcAddress("wglDXRegisterObjectNV");
    wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)wglGetProcAddress("wglDXLockObjectsNV");
    wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)wglGetProcAddress("wglDXUnlockObjectsNV");
    wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)wglGetProcAddress("wglDXUnregisterObjectNV");

    if (wglDXOpenDeviceNV) {
        g_hInteropDevice = wglDXOpenDeviceNV(g_d3dDevice);
        if (!g_hInteropDevice) std::cout << "[GPU Bridge] wglDXOpenDeviceNV failed!" << std::endl;
    } else {
        std::cout << "[GPU Bridge] WGL Extensions not found on this graphics driver!" << std::endl;
    }
}

void platform_unregister_gpu_texture() {
    if (g_hInteropDevice && g_hInteropObject) {
        wglDXUnregisterObjectNV(g_hInteropDevice, g_hInteropObject);
        g_hInteropObject = nullptr;
    }
}

int platform_map_gpu_texture(uint64_t handle_id, int target_gl_id, int width, int height) {
    if (!g_hInteropDevice) return 0;

    platform_unregister_gpu_texture();

    HANDLE shared_handle = reinterpret_cast<HANDLE>(handle_id);
    ID3D11Texture2D* pTex = nullptr;
    HRESULT hr = E_FAIL;

    ID3D11Device1* d3dDevice1 = nullptr;
    if (SUCCEEDED(g_d3dDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&d3dDevice1))) {
        hr = d3dDevice1->OpenSharedResource1(shared_handle, __uuidof(ID3D11Texture2D), (void**)&pTex);
        d3dDevice1->Release();
    }

    if (FAILED(hr) || !pTex) {
        hr = g_d3dDevice->OpenSharedResource(shared_handle, __uuidof(ID3D11Texture2D), (void**)&pTex);
    }

    if (FAILED(hr) || !pTex) {
        std::cout << "[GPU Bridge] ERROR: DirectX failed to open the handle! HRESULT: 0x" 
                  << std::hex << hr << std::dec << std::endl;
        return 0;
    }

    g_glTexture = static_cast<GLuint>(target_gl_id);

    g_hInteropObject = wglDXRegisterObjectNV(g_hInteropDevice, pTex, g_glTexture, 0x0DE1, 0x0000);
    
    if (!g_hInteropObject) {
        return 0;
    }

    pTex->Release();
    return static_cast<int>(g_glTexture);
}

void lock_texture() {
    if (g_hInteropObject) wglDXLockObjectsNV(g_hInteropDevice, 1, &g_hInteropObject);
}

void unlock_texture() {
    if (g_hInteropObject) wglDXUnlockObjectsNV(g_hInteropDevice, 1, &g_hInteropObject);
}
