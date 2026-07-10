#pragma once

#define MAX_SCENES 16
#include "SceneBase.h"


class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void RegisterScene(SceneBase* scene);

    void ChangeScene(const char* id);

    void Update(float deltaTime);

    void Render();

private:
    struct SceneEntry
    {
        const char* id;
        SceneBase* ptr;
    };

    SceneEntry m_SceneTable[MAX_SCENES];
    int        m_SceneCount;

    SceneBase* m_CurrentScene;
};
