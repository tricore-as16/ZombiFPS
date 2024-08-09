#include "PlayerReloadState.h"

/// <summary>
/// デフォルトコンストラクタ
/// </summary>
PlayerReloadState::PlayerReloadState()
{
    // 処理なし
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="playerModelHandle">プレイヤーのモデルハンドル</param>
/// <param name="previousStateData">前のステートの情報</param>
PlayerReloadState::PlayerReloadState(int& playerModelHandle, AnimationData previousStateData)
    : PlayerStateBase(playerModelHandle,previousStateData)
{
    playAnimationSpeed  = PlayAnimationSpeed;
    animationBlendSpeed = AnimationBlendSpeed;

    // 新しいアニメーションを再生する
    PlayNewAnimation(AnimationStateType::Idle);
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerReloadState::~PlayerReloadState()
{
    // 処理なし
}

/// <summary>
/// 更新
/// </summary>
void PlayerReloadState::Update()
{
    // プレイヤー座標の修正
    UpdateOffsetValue();

    // アニメーションの更新
    UpdateAnimation();

    // アニメーションデータの更新
    UpdateAnimationData();
}

/// <summary>
/// リロードステートの座標修正
/// </summary>
void PlayerReloadState::UpdateOffsetValue()
{
    // アニメーションの適用率を上昇
    // 上昇させることで腕を下げる
    animationFactor += ReloadAnimationSpeed;
    if (animationFactor > 1.0f)
    {
        animationFactor = 1.0f;
    }

    // リロードステートのずらし量を決める
    stateOffsetValue = VScale(ReloadAnimationOffsetValue, animationFactor);
}
