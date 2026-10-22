#pragma once

#include "glcType.h"
#include <initializer_list>

namespace glc
{
// An owned text texture, not an entry in the shared D3DXFont manager.
interface ILcxFont
{
	virtual ~ILcxFont()=default;
	virtual int Draw(const RECT& rect, bool clipped, DWORD color) = 0;
	virtual int Draw(const RECT& rect, DWORD color=0xFFFFFFFF) = 0;
	virtual int Draw(const VEC2i& pos, DWORD color=0xFFFFFFFF) = 0;
	virtual int Draw(std::initializer_list<long> coords, DWORD color=0xFFFFFFFF) = 0;
	virtual int SetText(const char* str) = 0;
	virtual void* GetTexture() const = 0;
	virtual VEC2i GetSize() const = 0;
};

} // namepace glc

