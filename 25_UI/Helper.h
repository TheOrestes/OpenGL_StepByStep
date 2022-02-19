#pragma once

const int START_WINDOW_WIDTH = 1280;
const int START_WINDOW_HEIGHT = 800;

const int SHADOWMAP_SIZE = 1024;

template<typename T> void SAFE_DELETE(T*& a)
{
	delete a;
	a = nullptr;
}
