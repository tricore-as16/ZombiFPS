﻿#include "Player.h"
#include "Input.h"
#include "PlayerCamera.h"
#include "Stage.h"
#include "PlayerIdleState.h"
#include "PlayerOnHitEnemyState.h"
#include "PlayerRunState.h"
#include "PlayerWalkState.h"
#include "ModelDataManager.h"
#include "AssaultRifle.h"
#include "BattleRifle.h"
#include "SubmachineGun.h"


/// <summary>
/// コンストラクタ
/// </summary>
Player::Player()
    : position              (VGet(0,0,0))
    , pressMoveButton       (false)
{
    equippedGun             = new SubmachineGun();
    modelDataManager        = ModelDataManager::GetInstance();
    playerCamera            = new PlayerCamera();
    playerState             = new PlayerIdleState();
    Initialize();
}

/// <summary>
/// デストラクタ
/// </summary>
Player::~Player()
{
    delete(playerState);
    delete(playerCamera);
    MV1DeleteModel(modelHandle);
}

/// <summary>
/// 初期化
/// </summary>
void Player::Initialize()
{
    // 座標設定
    position = VGet(0.0f, MoveLimitY, 0.0f);
    
    // モデルハンドルを取得
    modelHandle = modelDataManager->GetOriginalModelHandle(ModelDataManager::ModelDataType::PlayerModelData);

    // モデルサイズを再設定
    MV1SetScale(modelHandle, PlayerScale);

    // 状態を初期化
    state = State::None;
    
    // ジャンプ力は初期状態では０
    currentJumpPower = 0.0f;
    
    // 初期状態でプレイヤーが向くべき方向はＸ軸方向
    targetMoveDirection = VGet(1.0f, 0.0f, 0.0f);
    
    // アニメーションのブレンド率を初期化
    animationBlendRate = 1.0f;
    
    // 初期状態ではアニメーションはアタッチされていないにする
    currentPlayAnimation = -1;
    previousPlayAnimation = -1;
    
    // アニメーション設定
    PlayAnimation(AnimationType::None);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="stage">ステージ</param>
void Player::Update(const Input& input, Stage& stage)
{
    // ルートフレームのＺ軸方向の移動パラメータを無効にする
    DisableRootFrameZMove();

    // パッド入力によって移動パラメータを設定する
    VECTOR  upModveVector;          // 方向ボタン「↑」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR  leftMoveVector;         // 方向ボタン「←」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR  currentFrameMoveVector; // このフレームの移動ベクトル

    // 移動ベクトルの更新
    UpdateMoveVector(input, upModveVector, leftMoveVector, currentFrameMoveVector);

    // 移動ボタンが押されたかどうかで処理を分岐
    if (pressMoveButton)
    {
        // 移動ベクトルを正規化したものをプレイヤーが向くべき方向として保存
        targetMoveDirection = VNorm(currentFrameMoveVector);

        // プレイヤーが向くべき方向ベクトルをプレイヤーのスピード倍したものを移動ベクトルとする
        currentFrameMoveVector = VScale(targetMoveDirection, MoveSpeed);

        // もし今まで「立ち止まり」状態だったら
        if (state == State::None)
        {
            // 走りアニメーションを再生する
            PlayAnimation(AnimationType::Run);

            // 状態を「走り」にする
            state = State::Run;
        }
    }
    else
    {
        // このフレームで移動していなくて、且つ状態が「走り」だったら
        if (state == State::Run)
        {
            // 立ち止りアニメーションを再生する
            PlayAnimation(AnimationType::Stop);

            // 状態を「立ち止り」にする
            state = State::None;
        }
    }

    // 状態が「ジャンプ」の場合は
    if (state == State::Jump)
    {
        // Ｙ軸方向の速度を重力分減算する
        currentJumpPower -= Gravity;

        // もし落下していて且つ再生されているアニメーションが上昇中用のものだった場合は
        if (currentJumpPower < 0.0f && MV1GetAttachAnim(modelHandle, currentPlayAnimation) == 2)
        {
            // 落下中ようのアニメーションを再生する
            PlayAnimation(AnimationType::Jump);
        }

        // 移動ベクトルのＹ成分をＹ軸方向の速度にする
        currentFrameMoveVector.y = currentJumpPower;
    }

    // プレイヤーモデルとプレイヤーカメラの回転率を同期させる
    UpdateAngle();

    // 移動ベクトルを元にコリジョンを考慮しつつプレイヤーを移動
    Move(currentFrameMoveVector, stage);

    // アニメーション処理
    UpdateAnimation();

    // 装備中の武器の更新
    equippedGun->Update(position, playerCamera->GetCameraForwardVector(), playerCamera->GetCameraPitch());

    // プレイヤーカメラの更新
    UpdatePlayerCamera(input, stage);
}

/// <summary>
/// 描画
/// </summary>
void Player::Draw(const Stage& stage)
{
    // モデルの描画
    MV1DrawModel(modelHandle);

    // 武器の描画
    equippedGun->Draw();

    // 座標描画
    DrawFormatString(100,0,GetColor(255,255,255),"X:%f Y:%f Z:%f",position.x,position.y,position.z);
}

/// <summary>
/// プレイヤーカメラの更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="stage">ステージ</param>
void Player::UpdatePlayerCamera(const Input& input, Stage& stage)
{
    playerCamera->Update(input, position,stage);
}

/// <summary>
/// 天井に接触した時の処理
/// </summary>
void Player::OnHitRoof()
{
    // Ｙ軸方向の速度は反転
    currentJumpPower = -currentJumpPower;
}

/// <summary>
/// 床に当たった時の処理
/// </summary>
void Player::OnHitFloor()
{
    // Ｙ軸方向の移動速度は０に
    currentJumpPower = 0.0f;

    // もしジャンプ中だった場合は着地状態にする
    if (state == State::Jump)
    {
        // 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
        if (currentFrameMove)
        {
            // 移動している場合は走り状態に
            PlayAnimation(AnimationType::Run);
            state = State::Run;
        }
        else
        {
            // 移動していない場合は立ち止り状態に
            PlayAnimation(AnimationType::Stop);
            state = State::None;
        }

        // 着地時はアニメーションのブレンドは行わない
        animationBlendRate = 1.0f;
    }
}

/// <summary>
/// ルートフレームのZ軸方向の移動パラメータを無効にする
/// </summary>
void Player::DisableRootFrameZMove()
{
    // HACK:
    // ・DXライブラリのモデルファイル内には、複数のメッシュ（ポリゴンの集合）やカメラ、ライトを入れることができる
    // ・置いた複数のファイルは、親子関係をつけたり、Unityのヒエラルキーみたいに、階層構造が作れる
    // ・この階層それぞれには名前が付けられる Root-Meshes-Model1
    //                                                   |-Model2
    // ・この名前の付いた階層のことをDXライブラリではフレームという
    // ・一番親の階層を「ルートフレーム」と呼ぶ。ルートフレームは一つ
    // 
    // HACK: 何のために？モデルの一番親フレーム（親階層）のZ軸方向の移動パラメータをゼロにしている

    MATRIX localMatrix;

    // ユーザー行列を解除する
    MV1ResetFrameUserLocalMatrix(modelHandle, 2);

    // 現在のルートフレームの行列を取得する
    localMatrix = MV1GetFrameLocalMatrix(modelHandle, 2);

    // Ｚ軸方向の平行移動成分を無効にする
    localMatrix.m[3][2] = 0.0f;

    // ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
    MV1SetFrameUserLocalMatrix(modelHandle, 2, localMatrix);
}

/// <summary>
/// 移動ベクトルの更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="upModveVector">上方向ベクトル</param>
/// <param name="leftMoveVector">左方向ベクトル</param>
/// <param name="currentFrameMoveVector">移動ベクトル</param>
void Player::UpdateMoveVector(const Input& input, VECTOR& upModveVector,
    VECTOR& leftMoveVector, VECTOR& currentFrameMoveVector)
{
    // プレイヤーの移動方向のベクトルを算出
// 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
    upModveVector = VSub(playerCamera->GetTargetPosition(), playerCamera->GetCameraPosition());
    upModveVector.y = 0.0f;

    // 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
    leftMoveVector = VCross(upModveVector, VGet(0.0f, 1.0f, 0.0f));

    // ベクトルの正規化
    upModveVector = VNorm(upModveVector);
    leftMoveVector = VNorm(leftMoveVector);

    // このフレームでの移動ベクトルを初期化
    currentFrameMoveVector = ZeroVector;

    // 移動用のボタンが入力されたかどうか
    pressMoveButton = false;

    // パッドの３ボタンと左シフトがどちらも押されていなかったらプレイヤーの移動処理
    if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (input.GetCurrentFrameInput() & PAD_INPUT_C) == 0)
    {
        // 方向ボタン「←」が入力されたらカメラの見ている方向から見て左方向に移動する
        if (input.GetCurrentFrameInput() & PAD_INPUT_LEFT || CheckHitKey(KEY_INPUT_A))
        {
            // 移動ベクトルに「←」が入力された時の移動ベクトルを加算する
            currentFrameMoveVector = VAdd(currentFrameMoveVector, leftMoveVector);

            // 移動用ボタンが押された
            pressMoveButton = true;
        }
        else
        {
            // 方向ボタン「→」が入力されたらカメラの見ている方向から見て右方向に移動する
            if (input.GetCurrentFrameInput() & PAD_INPUT_RIGHT || CheckHitKey(KEY_INPUT_D))
            {
                // 移動ベクトルに「←」が入力された時の移動ベクトルを反転したものを加算する
                currentFrameMoveVector = VAdd(currentFrameMoveVector, VScale(leftMoveVector, -1.0f));

                // 移動用ボタンが押された
                pressMoveButton = true;
            }
        }

        // 方向ボタン「↑」が入力されたらカメラの見ている方向に移動する
        if (input.GetCurrentFrameInput() & PAD_INPUT_UP || CheckHitKey(KEY_INPUT_W))
        {
            // 移動ベクトルに「↑」が入力された時の移動ベクトルを加算する
            currentFrameMoveVector = VAdd(currentFrameMoveVector, upModveVector);

            // 移動用ボタンが押された
            pressMoveButton = true;
        }
        else
        {
            // 方向ボタン「↓」が入力されたらカメラの方向に移動する
            if (input.GetCurrentFrameInput() & PAD_INPUT_DOWN || CheckHitKey(KEY_INPUT_S))
            {
                // 移動ベクトルに「↑」が入力された時の移動ベクトルを反転したものを加算する
                currentFrameMoveVector = VAdd(currentFrameMoveVector, VScale(upModveVector, -1.0f));

                // 移動用ボタンが押された
                pressMoveButton = true;
            }
        }

        // MEMO:
        // のちにジャンプを追加する可能性があるためコメントアウトしています
        // プレイヤーの状態が「ジャンプ」ではなく、且つボタン１が押されていたらジャンプする
        //if (state != State::Jump && (input.GetNowNewFrameInput() & PAD_INPUT_A))
        //{
        //    // 状態を「ジャンプ」にする
        //    state = State::Jump;

        //    // Ｙ軸方向の速度をセット
        //    currentJumpPower = JumpPower;

        //    // ジャンプアニメーションの再生
        //    PlayAnimation(AnimationType::Jump);
        //}
    }
}

/// <summary>
/// 移動処理
/// </summary>
/// <param name="MoveVector">移動ベクトル</param>
/// <param name="stage">ステージ</param>
void Player::Move(const VECTOR& MoveVector, Stage& stage)
{
    // HACK: 移動距離が0.01未満で微妙に移動していた場合はじんわり移動してバグる
    // x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
    if (fabs(MoveVector.x) > 0.01f || fabs(MoveVector.z) > 0.01f)
    {
        currentFrameMove = true;
    }
    else
    {
        currentFrameMove = false;
    }

    // 当たり判定をして、新しい座標を保存する
    VECTOR oldPosition = position;                      // 移動前の座標
    VECTOR nextPosition = VAdd(position, MoveVector);   // 移動後の座標

    // ステージとの当たり判定処理
    position = stage.IsHitCollision(*this, nextPosition, MoveVector);

    // 床より少し高くする
    if (position.y <= MoveLimitY)
    {
        position.y = MoveLimitY;
    }

    // プレイヤーのモデルの座標を更新する
    MV1SetPosition(modelHandle, position);
}

/// <summary>
/// 回転制御
/// </summary>
void Player::UpdateAngle()
{
    // プレイヤーモデルをプレイヤーカメラの回転率と同様に回転させる

    // プレイヤー専用カメラの方向を取得
    VECTOR cameraForward = playerCamera->GetCameraForwardVector();
    float cameraPitch = playerCamera->GetCameraPitch();

    // モデルの水平方向回転値を計算
    float playerAngleY = atan2f(cameraForward.x, cameraForward.z);

    // プレイヤーモデルを追加で180度回転
    // FIXME:
    // ブレンダーでアニメーションを残したままモデルを回転し、
    // 保存する方法が分からないためプログラム上で実装
    playerAngleY += 150.0f * DX_PI_F / 180.0f;

    // 腰だめ角度に調整
    cameraPitch += 30.0f * DX_PI_F / 180.0f;

    // モデルの回転
    MV1SetRotationXYZ(modelHandle, VGet(cameraPitch, playerAngleY, 0.0f));
}

/// <summary>
/// プレイヤーのアニメーションを新しく追加する
/// </summary>
/// <param name="PlayAnimation">再生したいアニメーション番号</param>
void Player::PlayAnimation(AnimationType PlayAnimation)
{
    // HACK: 指定した番号のアニメーションをアタッチし、直前に再生していたアニメーションの情報をprevに移行している
    // 入れ替えを行うので、１つ前のモーションがが有効だったらデタッチする
    if (previousPlayAnimation != -1)
    {
        MV1DetachAnim(modelHandle, previousPlayAnimation);
        previousPlayAnimation = -1;
    }

    // 今まで再生中のモーションだったものの情報をPrevに移動する
    previousPlayAnimation = currentPlayAnimation;
    previousAnimationCount = currentAnimationCount;

    // 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
    currentPlayAnimation = MV1AttachAnim(modelHandle, static_cast<int>(PlayAnimation));
    currentAnimationCount = 0.0f;

    // ブレンド率はPrevが有効ではない場合は１．０ｆ( 現在モーションが１００％の状態 )にする
    animationBlendRate = previousPlayAnimation == -1 ? 1.0f : 0.0f;
}


/// <summary>
/// アニメーション更新
/// </summary>
void Player::UpdateAnimation()
{
    float animationTotalTime;       // 再生しているアニメーションの総時間

    // ブレンド率が１以下の場合は１に近づける
    if (animationBlendRate < 1.0f)
    {
        animationBlendRate += AnimationBlendSpeed;
        if (animationBlendRate > 1.0f)
        {
            animationBlendRate = 1.0f;
        }
    }

    // 再生しているアニメーション１の処理
    if (currentPlayAnimation != -1)
    {
        // アニメーションの総時間を取得
        animationTotalTime = MV1GetAttachAnimTotalTime(modelHandle, currentPlayAnimation);

        // 再生時間を進める
        currentAnimationCount += PlayAnimationSpeed;

        // 再生時間が総時間に到達していたら再生時間をループさせる
        if (currentAnimationCount >= animationTotalTime)
        {
            currentAnimationCount = static_cast<float>(fmod(currentAnimationCount, animationTotalTime));
        }

        // 変更した再生時間をモデルに反映させる
        MV1SetAttachAnimTime(modelHandle, currentPlayAnimation, currentAnimationCount);

        // アニメーション１のモデルに対する反映率をセット
        MV1SetAttachAnimBlendRate(modelHandle, currentPlayAnimation, animationBlendRate);
    }

    // 再生しているアニメーション２の処理
    if (previousPlayAnimation != -1)
    {
        // アニメーションの総時間を取得
        animationTotalTime = MV1GetAttachAnimTotalTime(modelHandle, previousPlayAnimation);

        // 再生時間を進める
        previousAnimationCount += PlayAnimationSpeed;

        // 再生時間が総時間に到達していたら再生時間をループさせる
        if (previousAnimationCount > animationTotalTime)
        {
            previousAnimationCount = static_cast<float>(fmod(previousAnimationCount, animationTotalTime));
        }

        // 変更した再生時間をモデルに反映させる
        MV1SetAttachAnimTime(modelHandle, previousPlayAnimation, previousAnimationCount);

        // アニメーション２のモデルに対する反映率をセット
        MV1SetAttachAnimBlendRate(modelHandle, previousPlayAnimation, 1.0f - animationBlendRate);
    }
}
