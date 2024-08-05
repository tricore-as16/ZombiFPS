﻿#include "PlayerRunState.h"

/// <summary>
/// コンストラクタ
/// </summary>
PlayerRunState::PlayerRunState()
{
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="playerModelHandle">プレイヤーのモデルハンドル</param>
/// <param name="previousStateData">前のステートの情報</param>
PlayerRunState::PlayerRunState(int playerModelHandle, AnimationData previousStateData)
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
PlayerRunState::~PlayerRunState()
{
}

/// <summary>
/// 更新
/// </summary>
void PlayerRunState::Update()
{
    // アニメーションの更新
    UpdateAnimation();

    // アニメーションデータの更新
    UpdateAnimationData();
}

/// <summary>
/// アニメーションデータの更新
/// </summary>
void PlayerRunState::UpdateAnimationData()
{
    nowStateData.currentAnimationCount  = currentAnimationCount;
    nowStateData.currentPlayAnimation   = currentPlayAnimation;
    nowStateData.previousAnimationCount = previousAnimationCount;
    nowStateData.previousPlayAnimation  = previousPlayAnimation;
}


