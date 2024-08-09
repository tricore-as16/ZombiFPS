#include "GunBase.h"

/// <summary>
/// コンストラクタ
/// </summary>
GunBase::GunBase()
    : runAnimationCount     (0)
    , shotAnimationCount    (0)
    , reloadAnimationCount  (0)
    , rotationMatrix        (MGetIdent())
{
}

/// <summary>
/// デストラクタ
/// </summary>
GunBase::~GunBase()
{
    // モデルの削除
    MV1DeleteModel(modelHandle);
}

/// <summary>
/// 移動の更新
/// </summary>
/// <param name="setPosition">設定したい座標</param>
/// <param name="playerState">プレイヤーの状態</param>
void GunBase::UpdateMove(VECTOR setPosition, Player::State playerState)
{
}

/// 銃の座標更新
/// </summary>
/// <param name="setPosition">設定する座標</param>
/// <param name="cameraForwardVector">カメラの前方向ベクトル</param>
/// <param name="setPitch">設定する上下角度</param>
void GunBase::UpdateGunPosition(VECTOR setPosition, VECTOR cameraForwardVector, float cameraPitch ,Player::State playerState)
{
    // 座標をプレイヤーの腕に丁度良くずらす
    //FixedGunPosition(setPosition, cameraForwardVector);

    // 回転の更新
    UpdateAngle(cameraForwardVector, cameraPitch,playerState);
}

/// <summary>
/// 回転の更新
/// </summary>
/// <param name="cameraForwardVector">カメラの前ベクトル</param>
/// <param name="pitch">上下角度</param>
void GunBase::UpdateAngle(VECTOR cameraForwardVector, float pitch,Player::State playerState)
{
    // 銃モデルをプレイヤーカメラの回転率と同様に回転させる
    // プレイヤー専用カメラの方向を取得
    VECTOR cameraForward = cameraForwardVector;
    float cameraPitch = pitch;

    // モデルの水平方向回転値を計算
    float gunAngleY = atan2f(cameraForward.x, cameraForward.z);

    // 腰だめの角度に修正
    gunAngleY   -= HipUpPositionAngleY;         // 水平方向回転度
    cameraPitch += HipUpPositionANglePitch;     // 垂直方向回転度

    // 回転行列に変更
    MATRIX rotationX = MGetRotX(-cameraPitch);
    MATRIX rotationY = MGetRotY(gunAngleY);
    MATRIX rotationZ = MGetRotZ(0.0f);

    // 回転行列を合成
    rotationMatrix = MMult(rotationX, rotationY);

    // ステートに応じてアニメーションを再生
    // MEMO:
    // アニメーション中にモデルの座標、回転値が変わるため
    // 回転の設定関数はこれよりも後に行う
    switch (playerState)
    {
    case Player::State::Run:
        PlayRunAnimation();
        break;

    case Player::State::Shot:
        PlayShotAnimation(cameraForwardVector);
        break;

    case Player::State::Reload:
        break;

    case Player::State::OnHitEnemy:
        break;

    default:
        break;
    }

    // モデルに回転行列を適用
    MV1SetRotationMatrix(modelHandle, rotationMatrix);
}

/// <summary>
/// 銃の位置調整
/// </summary>
/// <param name="cameraForwardVector">カメラの前方向ベクトル</param>
void GunBase::FixedGunPosition(VECTOR setPosition, VECTOR cameraForwardVector)
{
    // カメラから平行なベクトル
    VECTOR horizonDirection = VGet(cameraForwardVector.x, 0.0f, cameraForwardVector.z);

    // 正規化
    VECTOR velocity = VNorm(horizonDirection);

    // ずらし量を計算
    VECTOR offset = VScale(velocity, HipUpPositionOffsetScale);
    position = VAdd(position, offset);

    // 座標を設定
    MV1SetPosition(modelHandle, position);
}

/// <summary>
/// 走るアニメーションの再生
/// </summary>
void GunBase::PlayRunAnimation()
{
    // アニメーションカウントを進める
    runAnimationCount++;

    // 回転角度を設定
    // sin関数で[1]～[-1]を出してもらう
    // (アニメーションカウントを再生周期で割ることで現在どのくらい進んでいるかが分かる)
    float animationProgress = sin(DX_TWO_PI_F * runAnimationCount / Player::RunAnimationFrameCycle);
    // 最大アングル * [１～ -１] = 角度
    // sinを使うことでマイナスの条件式を省く
    float angle = Player::RunAnimationLimitAngle * animationProgress;

    // 回転行列を取得
    MATRIX runMatrixY       = MGetRotY(angle);                  // 銃口を下に向ける
    MATRIX runMatrixX       = MGetRotX(RunAnimationAngle);      // 左右に周期的に回転させる
    MATRIX runFinalMatrix   = MMult(runMatrixX, runMatrixY);    // 最終的な回転率

    // 回転行列を合成
    rotationMatrix = MMult(runFinalMatrix, rotationMatrix);
}

/// <summary>
/// 撃つアニメーションの再生
/// </summary>
void GunBase::PlayShotAnimation(VECTOR cameraForwardVector)
{
    // アニメーションカウントを進める
    shotAnimationCount++;

    // 進行度合を計算
    float shotAnimationFactor = shotAnimationCount / RecoilCycle;

    // 銃を後ろに後退させる
    float recoiOffset = 0.0f;
    if (shotAnimationFactor < RecoilDistanceLimit)
    {
        recoiOffset = RecoilDistance * (fabs(RecoilDistanceLimit - shotAnimationFactor));
    }

    // カメラの前方向ベクトルを正規化
    VECTOR cameraForwardVectorNormalized = VNorm(cameraForwardVector);

    // カメラの前方向から見て後ろへ反動を加える
    VECTOR recoilVector = VScale(cameraForwardVectorNormalized, recoiOffset);

    // 座標を更新
    position = VAdd(position, recoilVector);

    // 座標を設定
    MV1SetPosition(modelHandle, position);

    // カウントが上限ではないか
    if (shotAnimationCount >= RecoilCycle)
    {
        // アニメーションカウントをリセットする
        shotAnimationCount = 0;
    }
}

/// <summary>
/// リロードアニメーションの再生
/// </summary>
void GunBase::PlayReloadAnimation()
{

}
