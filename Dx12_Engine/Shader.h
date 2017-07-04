#pragma once
#include "CoreHeader.h"

class Shader
{
public:
	Shader();
	~Shader();

	std::wstring mVsName_;
	std::wstring mPsName_;

	std::string mVsEntryPoint_;
	std::string mPsEntryPoint_;

	void Initialize();

	HRESULT CompileVS(std::string mVertName, std::string mEntryPoint, std::string targetVersion, uint32_t mCompileFlags);
	HRESULT CompilePS(std::string mPixelName, std::string mEntryPoint, std::string targetVersion, uint32_t mCompileFlags);

	ID3DBlob * pVS_;
	ID3DBlob* pPS_;


};

