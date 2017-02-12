#include "stdafx.h"
#include "Effects.h"

ID3D10Effect*  Effects::m_pEffect = NULL;

ID3D10EffectTechnique*   Effects::m_pTechDrawTexture = NULL;
ID3D10EffectTechnique*   Effects::m_pTechDrawRect = NULL; 
ID3D10EffectTechnique*   Effects::m_pTechFillRect = NULL; 
ID3D10EffectTechnique*   Effects::m_pTechFillRectMatrix = NULL;
ID3D10EffectTechnique*   Effects::m_pTechDrawTextureMatrix = NULL;

ID3D10EffectShaderResourceVariable*  Effects::m_pFxTexture10 = NULL;
ID3D10EffectMatrixVariable*  Effects::m_pFxMatrix = NULL;
ID3D10EffectMatrixVariable*  Effects::m_pFxOrthMatrix = NULL;
ID3D10EffectVectorVariable*  Effects::m_pFxVsDestPos = NULL;

bool Effects::Init(ID3D10Device* pDevice)
{
    // Create Effect
    DWORD dwShaderFlags = /*D3D10_SHADER_ENABLE_STRICTNESS |*/ 
        D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#ifdef _DEBUG
    dwShaderFlags |= D3D10_SHADER_DEBUG|D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    HRESULT  hCompileResult = S_OK;
    ID3D10Blob*  pCompileBlob = NULL;

#ifdef _DEBUG
    TCHAR szPath[MAX_PATH] = {0};
    GetModuleFileName(g_hInstance, szPath,MAX_PATH);
    TCHAR* p = _tcsrchr(szPath, TEXT('\\'));
    if (p)
    {
        *(p+1) = 0;
    }

    _tcscat(szPath, TEXT("..\\..\\ui\\UICompositor\\Src\\d3d10\\d3d10.fx"));

    HRESULT hr = D3DX10CreateEffectFromFile(
            szPath, 
            NULL, 
            NULL, 
            "fx_4_0", 
            dwShaderFlags,
            0, 
            pDevice,
            NULL,
            NULL,
            &m_pEffect,
            &pCompileBlob,
            &hCompileResult);
#else
    // 必须放在 RCDATA 资源当中
    HRESULT hr = D3DX10CreateEffectFromResource(
            g_hInstance, 
            MAKEINTRESOURCE(fx_4_0),
            0,
            NULL, 
            NULL, 
            "fx_4_0", 
            dwShaderFlags,
            0, 
            pDevice,
            NULL, 
            NULL,
            &m_pEffect, 
            &pCompileBlob, 
            &hCompileResult);
#endif	
	UIASSERT(SUCCEEDED(hr));
    if (pCompileBlob)
    {
        char* pErrorDesc = (char*)pCompileBlob->GetBufferPointer();
        MessageBoxA(NULL, pErrorDesc, "load fx failed.", MB_OK|MB_ICONWARNING);
        SAFE_RELEASE(pCompileBlob);
    }

    if (FAILED(hr))
    {
        return false;
    }

    m_pTechDrawTexture = m_pEffect->GetTechniqueByName( "DrawTexture" );
    m_pTechFillRect = m_pEffect->GetTechniqueByName( "FillRect" );
    m_pTechDrawRect = m_pEffect->GetTechniqueByName( "DrawRect" );
    m_pTechFillRectMatrix = m_pEffect->GetTechniqueByName( "FillRectMatrix" );
    m_pTechDrawTextureMatrix = m_pEffect->GetTechniqueByName( "DrawTextureMatrix" );
    m_pFxTexture10 = m_pEffect->GetVariableByName( "g_Texture" )->AsShaderResource();
    m_pFxMatrix = m_pEffect->GetVariableByName( "g_Matrix" )->AsMatrix();
    m_pFxOrthMatrix = m_pEffect->GetVariableByName( "g_orthMatrix" )->AsMatrix();
	m_pFxVsDestPos = m_pEffect->GetVariableByName( "g_vsDestPos" )->AsVector();

	float destPos[4] = {0};
	m_pFxVsDestPos->SetFloatVector(destPos);

    return true;
}

void Effects::Release()
{
    m_pTechDrawTexture = NULL;
    m_pTechDrawRect = NULL;
    m_pTechFillRect = NULL;
    m_pFxTexture10 = NULL;
    m_pTechFillRectMatrix = NULL;
    m_pTechDrawTextureMatrix = NULL;
    m_pFxOrthMatrix = NULL;
    m_pFxMatrix = NULL;
	m_pFxVsDestPos = nullptr;

    SAFE_RELEASE(m_pEffect);
}