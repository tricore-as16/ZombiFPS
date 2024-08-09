#pragma once
#include "GunBase.h"

class GunStateBase;
class Bullet;
class ModelDataManager;
class BulletObjectPools;

/// <summary>
/// サブマシンガン
/// </summary>
class SubmachineGun : public GunBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SubmachineGun();
    
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~SubmachineGun();

    /// <summary>
    /// 初期化 
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 弾丸情報の初期化
    /// </summary>
    void InitializeBulletData(VECTOR cameraPosition, VECTOR targetPosition) override;

    /// <summary>
    /// 銃の更新
    /// </summary>
    /// <param name="setPosition">設定座標</param>
    /// <param name="cameraVector">カメラの前ベクトル</param>
    /// <param name="cameraPitch">カメラの水平からの角度</param>
    void Update(VECTOR setPosition,VECTOR cameraVector, VECTOR cameraTargetVector,
        VECTOR cameraPosition,float cameraPitch, Player::State);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;

private:
    /// <summary>
    /// 移動の更新
    /// </summary>
    /// <param name="setPosition">設定したい座標</param>
    /// <param name="playerState">プレイヤーの状態</param>
    void UpdateMove(VECTOR setPosition, Player::State playerState) override;

    /// <summary>
    /// 銃を発砲する
    /// </summary>
    void UpdateShooting(VECTOR cameraPosition,VECTOR targetPosition);

    /// <summary>
    /// 走り時の座標調整
    /// </summary>
    /// <param name="playerState">プレイヤーの状態</param>
    /// <returns>調整された自身のポジション</returns>
    VECTOR FixedRunPosition(Player::State playerState);

    /// <summary>
    /// リロード時の座標調整
    /// </summary>
    /// <param name="playerState">プレイヤーの状態</param>
    /// <returns>調整された自身のポジション</returns>
    VECTOR FixedReloadPosition(Player::State playerState);

    //---------------------------------------------------------------------------------//
    //                                      定数                                       //
    //---------------------------------------------------------------------------------//
    // ステータス
    // 弾丸
    static constexpr float  BulletDamagePower       = 1.0f;         // 弾丸の威力
    static constexpr float  BulletPenetrationPower  = 1.0f;         // 弾丸の貫通力
    static constexpr float  BulletSpeed             = 100.0f;       // 弾丸の速度
    static constexpr VECTOR BulletDirection         = { 0,0,1 };    // 弾丸の移動方向
    // 銃
    static constexpr int    GunFireRate             = 5;            // 銃の連射力(このフレームに１回発射する)
    static constexpr float  GunRecoil               = 1.0f;         // 銃の反動
    static constexpr float  GunAccuracy             = 1.0f;         // 銃の精度(拡散度合い)
    static constexpr VECTOR InitializeScale         = { 0.07f,0.07f,0.07f };    // 初期化時のスケール
    // ずらし量
    static constexpr VECTOR GunOffset = { 0.0f,0.5f,0.0f };   // 銃のプレイヤーの腕からのずらし量

    //---------------------------------------------------------------------------------//
    //                                      変数                                       //
    //---------------------------------------------------------------------------------//
    ModelDataManager*       modelDataManager;       // モデルデータ読み込み用クラスのアドレス
    float                   runAnimationFactor;     // 走りアニメーション再生時の適応率
    float                   reloadAnimationFactor;  // リロードアニメーションの再生時の適応率
};

