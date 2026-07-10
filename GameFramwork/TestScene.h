#pragma once
#include "SceneBase.h"
#include <iostream>
class TestScene : public SceneBase
{
public:
    TestScene(const char* type, const char* id) :SceneBase(type, id) {}

    ~TestScene() override {}

    void OnEnter() override { std::cout << m_ID << ": Enter\n"; }

    void OnLeave() override { std::cout << m_ID << ": Leave\n"; }

    void Update(float dt) override { std::cout << m_ID << ": Update\n"; }

    void Render() override { std::cout << m_ID << ": Render\n"; }
};
