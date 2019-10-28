#pragma once

template<typename T> void SAFE_DELETE(T*& a)
{
	delete a;
	a = nullptr;
}
