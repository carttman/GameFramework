#pragma once

#include <cassert>

// SceneBase.h

class SceneBase
{
public:
    SceneBase(const char* type, const char* id)
        : m_Type(type), m_ID(id) {
    }
    virtual ~SceneBase() {}

    virtual void OnEnter() = 0;

    virtual void OnLeave() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render() = 0;

    const char* GetType() const { return m_Type; }

    const char* GetID() const { return m_ID; }

protected:
    const char* m_Type = "Base";
    const char* m_ID = "None";

    SceneBase() = delete; // 기본 생성자 금지
    SceneBase(const SceneBase&) = delete; // 복사 생성자 금지
    SceneBase& operator=(const SceneBase&) = delete; // 대입 연산자 금지
};