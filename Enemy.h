﻿#pragma once
#include "Common.h"

class ModelDataManager;
class Stage;

/// <summary>
/// エネミー(ゾンビ)
/// </summary>
class Enemy
{
public:
    /// <summary>
    /// エネミーの状態
    /// </summary>
    enum class State : int
    {
        None,       // 停止
        Walk,       // 歩き
        Run,        // 走り
        Attack,     // 攻撃
    };

    /// <summary>
    /// エネミーのアニメーションタイプ
    /// </summary>
    /// TODO:
    /// 不要なアニメーションデータの削除方法が分からないのでそのまま実装
    enum class AnimationType : int
    {
        CrawlingBiteAttack1,    // はいずり噛みつき攻撃(1,2は同じ)
        CrawlingBiteAttack2,    // はいずり噛みつき攻撃(1,2は同じ)
        Intimidation,           // 威嚇
        Crawling,               // はいずり移動
        RecoilDeath,            // のけぞり死亡
        NoAnimation1,           // アニメーションなし
        Attack,                 // 攻撃
        BiteAttack,             // 噛みつき攻撃
        Death,                  // 死亡
        Idle,                   // 何もしていない
        NoAnimation2,           // 走り
        Wolk,                   // 歩き
        Run,                    // 走り
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Enemy();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Enemy();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update(VECTOR targetPosition,Stage& stage);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

private:
    /// <summary>
    /// ルートフレームのZ軸方向の移動パラメータを無効にする
    /// </summary>
    void DisableRootFrameZMove();

    /// <summary>
    /// 移動ベクトルの更新
    /// </summary>
    /// <param name="targetPosition">向かうべき座標</param>
    void UpdateMoveVector(VECTOR targetPosition);

    /// <summary>
    /// 移動処理
    /// </summary>
    /// <param name="MoveVector">移動ベクトル</param>
    /// <param name="stage">ステージ</param>
    void Move(const VECTOR& MoveVector, Stage& stage);

    /// <summary>
    /// 回転制御
    /// </summary>
    void UpdateAngle();

    /// <summary>
    /// アニメーションを新しく再生する
    /// </summary>
    /// <param name="type">アニメーションの種類</param>
    void PlayAnimation(AnimationType type);

    /// <summary>
    /// アニメーション処理
    /// </summary>
    void UpdateAnimation();

    //---------------------------------------------------------------------------------//
    //                                      定数                                       //
    //---------------------------------------------------------------------------------//
    // ステータス
    static constexpr float  MoveSpeed           = 0.2f;                         // 移動速度
    static constexpr float  AngleSpeed          = 0.2f;                         // 角度変化速度
    static constexpr float  JumpPower           = 100.0f;                       // ジャンプ力
    static constexpr float  MoveLimitY          = 1.0f;                         // Y軸の移動制限
    static constexpr VECTOR InitializePosition  = { 0.0f,MoveLimitY,0.0f };     // 初期化座標
    static constexpr VECTOR ZeroVector          = { 0.0f,0.0f,0.0f };           // ゼロベクトル
    static constexpr VECTOR EnemyScale          = { 0.03f,0.03f,0.03f };        // プレイヤーのスケール
    // 重力関係
    static constexpr float  Gravity             = 3.0f;                         // 重力
    static constexpr float  FallUpPower         = 20.0f;                        // 足を踏み外した時のジャンプ力
    // アニメーション
    static constexpr float  PlayAnimationSpeed  = 0.5f;                         // アニメーション速度
    static constexpr float  AnimationBlendSpeed = 0.1f;                         // アニメーションのブレンド率変化速度

    //---------------------------------------------------------------------------------//
    //                                      変数                                       //
    //---------------------------------------------------------------------------------//
    // 管理クラス
    ModelDataManager*   modelDataManager;

    // ステータス
    VECTOR      position;                   // 座標
    VECTOR      targetMoveDirection;        // モデルが向くべき方向のベクトル
    float       angle;                      // モデルが向いている方向の角度
    float       currentJumpPower;           // Ｙ軸方向の速度
    int         modelHandle;                // モデルハンドル
    int         shadowHandle;               // 影画像ハンドル
    bool        currentFrameMove;           // そのフレームで動いたかどうか
    State       state;                      // 状態

    // アニメーション情報
    int         currentPlayAnimation;       // 再生しているアニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float       currentAnimationCount;      // 再生しているアニメーションの再生時間
    int         previousPlayAnimation;      // 前の再生アニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float       previousAnimationCount;     // 前の再生アニメーションの再生時間
    float       animationBlendRate;         // 現在と過去のアニメーションのブレンド率

};

