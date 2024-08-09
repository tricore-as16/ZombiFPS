#include "PlayerIdleState.h"

/// <summary>
/// デフォルトコンストラクタ
/// </summary>
PlayerIdleState::PlayerIdleState()
{
    // 処理なし
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="playerModelHandle">プレイヤーのモデルハンドル</param>
/// <param name="previousStateData">前のステートの情報</param>
PlayerIdleState::PlayerIdleState(int playerModelHandle, AnimationData previousStateData)
    : PlayerStateBase(playerModelHandle, previousStateData)
{
    playAnimationSpeed  = PlayAnimationSpeed;
    animationBlendSpeed = AnimationBlendSpeed;

    // 新しいアニメーションを再生する
    PlayNewAnimation(AnimationStateType::Idle);
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerIdleState::~PlayerIdleState()
{
    // 処理なし
}

/// <summary>
/// 更新
/// </summary>
void PlayerIdleState::Update()
{
    // プレイヤーの座標修正
    UpdateOffsetValue();

    // アニメーションの更新
    UpdateAnimation();

    // アニメーションデータの更新
    UpdateAnimationData();
}


