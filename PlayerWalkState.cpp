﻿#include "PlayerWalkState.h"
#include "Player.h"

/// <summary>
/// コンストラクタ
/// </summary>
PlayerWalkState::PlayerWalkState()
{
    // 処理なし
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="playerModelHandle">プレイヤーのモデルハンドル</param>
/// <param name="previousStateData">前のステートの情報</param>
PlayerWalkState::PlayerWalkState(int& playerModelHandle, AnimationData previousStateData)
    : PlayerStateBase(playerModelHandle, previousStateData)
{
    playAnimationSpeed      = PlayAnimationSpeed;
    animationBlendSpeed     = AnimationBlendSpeed;

    // 新しいアニメーションを再生する
    PlayNewAnimation(AnimationStateType::Idle);
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerWalkState::~PlayerWalkState()
{
    // 処理なし
}

/// <summary>
/// 更新
/// </summary>
void PlayerWalkState::Update()
{
    // アニメーションの更新
    UpdateAnimation();

    // アニメーションデータの更新
    UpdateAnimationData();
}

/// <summary>
/// アニメーションデータの更新
/// </summary>
void PlayerWalkState::UpdateAnimationData()
{
    nowStateData.currentAnimationCount  = currentAnimationCount;
    nowStateData.currentPlayAnimation   = currentPlayAnimation;
    nowStateData.previousAnimationCount = previousAnimationCount;
    nowStateData.previousPlayAnimation  = previousPlayAnimation;
}

