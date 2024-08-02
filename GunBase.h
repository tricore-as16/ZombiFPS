﻿#pragma once
#include "Common.h"
#include "Bullet.h"
#include "Player.h"

class   GunStateBase;
enum    GunState;
class   Bullet;

/// <summary>
/// 銃の基礎クラス
/// </summary>
class GunBase
{
public:
    /// <summary>
    /// 銃の状態
    /// </summary>
    enum class GunState : int
    {
        IdleState,      // 何もしていない状態
        RunState,       // 走り状態
        ShotState,      // 発砲状態
        ReloadState,    // リロード状態
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    GunBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~GunBase();

    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize() abstract;

    /// <summary>
    /// 弾丸情報の初期化
    /// </summary>
    virtual void InitializeBulletData(VECTOR cameraPosition, VECTOR targetPosition) abstract;

    /// <summary>
    /// 更新
    /// </summary>
    virtual void Update(VECTOR setPosition, VECTOR cameraVector,VECTOR cameraTargetVector,
        VECTOR cameraPosition, float cameraPitch,Player::State) abstract;

    /// <summary>
    /// 描画
    /// </summary>
    virtual void Draw() abstract;

    // ゲッター
    const int GetFireRate()const { return fireRate; }
    list<Bullet*>& GetActiveBullet() { return activeBullet; }
    const  int GetActiveBulletSize() const { return activeBullet.size(); }
    const Bullet::BulletInitializeData GetBulletInitializeData()const { return bulletData; }

protected:
    /// <summary>
    /// 移動の更新
    /// </summary>
    /// <param name="setPosition">設定したい座標</param>
    /// <param name="playerState">プレイヤーの状態</param>
    virtual void UpdateMove(VECTOR setPosition, Player::State playerState);

    /// <summary>
    /// 回転の更新
    /// </summary>
    /// <param name="cameraForwardVector">カメラの前ベクトル</param>
    /// <param name="pitch">上下角度</param>
    void UpdateAngle(VECTOR cameraForwardVector, float pitch, Player::State playerState);

    // 銃の位置調整
    void FixedGunPosition(VECTOR setPosition,VECTOR cameraForwardVector);


    //---------------------------------------------------------------------------------//
    //                                      変数                                       //
    //---------------------------------------------------------------------------------//
    // 腰だめ
    static constexpr float  HipUpPositionAngleY     = 3.0f * DX_PI_F / 180.0f;      // 水平回転用：腰だめの位置に調整するために回転させるY軸回転度(ラジアン)
    static constexpr float  HipUpPositionANglePitch = 30.0f * DX_PI_F / 180.0f;     // 垂直回転用：腰だめの位置に調整するために回転させる水平方向からの角度(ラジアン)
    // アニメーション


    //---------------------------------------------------------------------------------//
    //                                      変数                                       //
    //---------------------------------------------------------------------------------//
    // ステータス
    int             modelHandle;                // モデルハンドル
    VECTOR          position;                   // 座標
    VECTOR          positionOffsetAmount;       // 座標をずらす量
    MATRIX          rotationMatrix;             // モデルの回転行列
    GunState        state;                      // 銃の状態
    GunStateBase*   currentState;               // 現在の銃の更新を行う状態
    int             fireRate;                   // 銃の連射力
    float           recoil;                     // 銃の反動
    float           accuracy;                   // 銃の精度(拡散度合い)

    // 弾丸
    Bullet::BulletInitializeData bulletData;    // 弾丸のデータ
    list<Bullet*>   activeBullet;               // 使用中の弾丸
    float           bulletDamagePower;          // 弾丸の威力
    float           bulletPenetrationPower;     // 弾丸の貫通力

    // アニメーション
    int             runAnimationCount;          // 銃のアニメーションカウント
};


