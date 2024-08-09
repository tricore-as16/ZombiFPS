#pragma once
#include "PlayerStateBase.h"

/// <summary>
/// プレイヤーがリロードしている状態
/// </summary>
class PlayerReloadState : public PlayerStateBase
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    PlayerReloadState();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="playerModelHandle">プレイヤーのモデルハンドル</param>
    /// <param name="previousStateData">前のステートの情報</param>
    PlayerReloadState(int& playerModelHandle, AnimationData previousStateData);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~PlayerReloadState();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    void Update() override;

private:

    /// <summary>
    /// リロードステートの座標修正
    /// </summary>
    void UpdateOffsetValue() override;

    // アニメーション
    static constexpr float  PlayAnimationSpeed = 0.5f;      // アニメーション速度
    static constexpr float  AnimationBlendSpeed = 0.1f;     // アニメーションのブレンド率変化速度
    static constexpr float  ReloadAnimationSpeed = 0.05f;   // リロードアニメーションの適用速度
    static constexpr VECTOR ReloadAnimationOffsetValue = { 0.0f,-1.0f,0.0f };  // リロードステート時のずらし量
};


