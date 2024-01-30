#pragma once

//  ImGuiを使用してゲーム上でコンポーネントのプロパティを表示、編集できるようにするためのクラス
//  純粋仮想クラス(インターフェイス)として実装する

class IComponentUIDebugView {
public:
    virtual void ComponentUIRender() = 0;
    virtual int GetRenderPriority() = 0;
};
