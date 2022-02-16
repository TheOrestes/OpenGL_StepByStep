#pragma once

#include <vector>

class StaticObject;

class StaticObjectManager
{
public:
	static StaticObjectManager& getInstance()
	{
		static StaticObjectManager instance;
		return instance;
	}

	~StaticObjectManager();

	void			GatherStaticObject(StaticObject* _object);
	StaticObject*	GetStaticObjectAt(int i);
	
	void			RenderAll();
	void			RenderAllToShadowmap();
	void			UpdateAll(float dt);

	inline uint32_t	GetStaticObjectCount() { return m_vecStaticObjects.size(); }

private:
	StaticObjectManager();
	std::vector<StaticObject*> m_vecStaticObjects;
};