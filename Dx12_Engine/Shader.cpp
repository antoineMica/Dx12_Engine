#include "Shader.h"



Shader::Shader()
{
}


Shader::~Shader()
{
}


HRESULT Shader::CompileVS(std::string mVertName, std::string mEntryPoint, std::string mTargetVersion, uint32_t mCompileFlags)
{
	mVsName_ = std::wstring(mVertName.begin(), mVertName.end());
	mVsEntryPoint_ = mEntryPoint;
	
	//compile shader
	return D3DCompileFromFile(mVsName_.c_str(), nullptr, nullptr, mEntryPoint.c_str(), mTargetVersion.c_str(), mCompileFlags, 0, &pVS_, nullptr);

}

HRESULT Shader::CompilePS(std::string mPixelName, std::string mEntryPoint, std::string mTargetVersion, uint32_t mCompileFlags)
{
	mPsName_ = std::wstring(mPixelName.begin(), mPixelName.end());
	mPsEntryPoint_ = mEntryPoint;


	//compile shader
	return D3DCompileFromFile(mPsName_.c_str(), nullptr, nullptr, mPsEntryPoint_.c_str(), mTargetVersion.c_str(), mCompileFlags, 0, &pPS_, nullptr);

}
