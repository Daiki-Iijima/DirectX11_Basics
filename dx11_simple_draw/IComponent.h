#pragma once

class IComponent {
public:
    virtual void Update() = 0;
    virtual void Render() = 0;
};
