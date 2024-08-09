#include "SubmachineGun.h"
#include "Bullet.h"
#include "GunStateBase.h"
#include "ModelDataManager.h"
#include "GunIdleState.h"
#include "GunReloadState.h"
#include "GunRunState.h"
#include "GunShotState.h"

/// <summary>　　　　　　
/// コンストラクタ
/// </summary>
SubmachineGun::SubmachineGun()
    : runAnimationFactor        (0.0f)
    , reloadAnimationFactor     (0.0f)
{
    modelDataManager = ModelDataManager::GetInstance();
    Initialize();
}

/// <summary>
/// デストラクタ
/// </summary>
SubmachineGun::~SubmachineGun()
{
    // 要素の削除
    activeBullet.clear();

    // メモリ解放
    for (Bullet* bullet : activeBullet)
    {
        delete(bullet);
    }
}

/// <summary>
/// 初期化
/// </summary>
void SubmachineGun::Initialize()
{
    // モデルハンドルの読み込み
    modelHandle = modelDataManager->GetOriginalModelHandle(
        ModelDataManager::ModelDataType::SubmachineGunModelData);

    // ステータスの初期化
    state                   = GunBase::GunState::IdleState;

    // TODO:
    // 状態クラスを未作成のため未実装
    //currentState            = new GunIdleState();

    // 弾丸情報の初期化
    bulletDamagePower       = BulletDamagePower;
    bulletPenetrationPower  = BulletPenetrationPower;
    fireRate                = GunFireRate;
    recoil                  = GunRecoil;
    accuracy                = GunAccuracy;
    positionOffsetAmount = VGet(0, 0.5, -1);

    // スケールを調整
    MV1SetScale(modelHandle, InitializeScale);
}

/// <summary>
/// 更新
/// </summary>
void SubmachineGun::Update(VECTOR setPosition, VECTOR cameraVector, VECTOR cameraTargetVector,
     VECTOR cameraPosition, float cameraPitch, Player::State playerState)
{
    // 座標を更新
    position = VAdd(setPosition, GunOffset);

    // 銃の座標更新
    UpdateGunPosition(setPosition, cameraVector, cameraPitch,playerState);

    // 弾丸の更新
    UpdateShooting(cameraPosition,cameraTargetVector);

    // 移動更新
    UpdateMove(setPosition, playerState);
}

/// <summary>
/// 移動の更新
/// </summary>
/// <param name="setPosition">設定したい座標</param>
/// <param name="playerState">プレイヤーの状態</param>
void SubmachineGun::UpdateMove(VECTOR setPosition, Player::State playerState)
{
    // Runステート用の座標
    VECTOR movePosition = position;

    // 現在の適用率に基づいてオフセットを計算
    VECTOR runOffset    = FixedRunPosition(playerState);
    VECTOR reloadOffset = FixedReloadPosition(playerState);

    // 各種ステート時の座標修正
    movePosition = VAdd(movePosition, runOffset);
    movePosition = VAdd(movePosition, reloadOffset);

    // 座標の設定
    MV1SetPosition(modelHandle, movePosition);
}

/// <summary>
/// 走りのアンメーション再生
/// </summary>
/// <param name="playerState">プレイヤーのステート</param>
VECTOR SubmachineGun::FixedRunPosition(Player::State playerState)
{
    // 走りアニメーション時の処理
    if (playerState == Player::State::Run)
    {
        // アニメーションの適用率を増加させる
        runAnimationFactor += Player::RunAnimationFactorSpeed;
        if (runAnimationFactor > 1.0f)
        {
            runAnimationFactor = 1.0f;
        }
    }
    else
    {
        // 他の状態に移行した場合、適用率を減少させる
        runAnimationFactor -= Player::RunAnimationFactorSpeed;
        if (runAnimationFactor < 0.0f)
        {
            runAnimationFactor = 0.0f;
        }
    }

    // 現在の適用率を返す
    VECTOR runOffset = VScale(Player::RunAnimationOffset, runAnimationFactor);
    return runOffset;
}

/// <summary>
/// リロード時の座標調整
/// </summary>
/// <param name="playerState">プレイヤーの状態</param>
/// <returns>調整された自身のポジション</returns>
VECTOR SubmachineGun::FixedReloadPosition(Player::State playerState)
{
    // 走りアニメーション時の処理
    if (playerState == Player::State::Reload)
    {
        // アニメーションの適用率を増加させる
        reloadAnimationFactor += Player::ReloadAnimationFactorSpeed;
        if (reloadAnimationFactor > 1.0f)
        {
            reloadAnimationFactor = 1.0f;
        }
    }
    else
    {
        // 他の状態に移行した場合、適用率を減少させる
        reloadAnimationFactor -= Player::ReloadAnimationFactorSpeed;
        if (reloadAnimationFactor < 0.0f)
        {
            reloadAnimationFactor = 0.0f;
        }
    }

    // 現在の適用率を返す
    VECTOR reloadOffset = VScale(Player::ReloadAnimationOffset, reloadAnimationFactor);
    return reloadOffset;
}

/// <summary>
/// 弾丸情報の初期化
/// </summary>
void SubmachineGun::InitializeBulletData(VECTOR cameraPosition, VECTOR targetPosition)
{
    bulletData.lineStartPosition    = cameraPosition;           // カメラの座標
    bulletData.lineEndPosiion       = targetPosition;           // カメラの向いている座標
    bulletData.direction            = VNorm(VSub(targetPosition, cameraPosition));  // 弾丸の移動方向
    bulletData.position             = position;                 // 座標
    bulletData.power                = BulletDamagePower;        // 威力
    bulletData.speed                = BulletSpeed;              // 速度
    bulletData.penetratingPower     = BulletPenetrationPower;   // 貫通力
}

/// <summary>
/// 描画
/// </summary>
void SubmachineGun::Draw()
{
    // モデルの描画
    MV1DrawModel(modelHandle);

    // 現在使用中の弾丸を描画
    for (auto it = activeBullet.begin(); it != activeBullet.end(); ++it)
    {
        (*it)->Draw();
    }
}

/// <summary>
/// 銃を発砲する
/// </summary>
void SubmachineGun::UpdateShooting(VECTOR cameraPosition, VECTOR targetPosition)
{
    // 弾丸の初期化用データの更新
    InitializeBulletData(cameraPosition,targetPosition);

    // 現在使用中の弾丸の更新
    for (auto it = activeBullet.begin(); it != activeBullet.end(); ++it)
    {
        (*it)->Update();
    }
}


