#include "Player.h"
#include "Input.h"
#include "PlayerCamera.h"
#include "Stage.h"
#include "PlayerIdleState.h"
#include "PlayerOnHitEnemyState.h"
#include "PlayerRunState.h"
#include "PlayerWalkState.h"
#include "PlayerShotState.h"
#include "PlayerReloadState.h"
#include "ModelDataManager.h"
#include "AssaultRifle.h"
#include "BattleRifle.h"
#include "SubmachineGun.h"
#include "BulletObjectPools.h"


/// <summary>
/// コンストラクタ
/// </summary>
Player::Player()
    : position                      (VGet(0,0,0))
    , rotationMatrix                (MGetIdent())
    , pressMoveButton               (false)
    , isShooting                    (false)
    , isReload                      (false)
    , shootFireRateCount            (0)
    , isHitEnemyAttack              (false)
    , runAnimationCount             (0)
    , runAnimationFactor        (0.0f)
    , reloadAnimationCount          (0)
    , reloadAnimationFactor         (0.0f)
    , reloadTimer                   (0)
{
    modelDataManager        = ModelDataManager::GetInstance();
    Initialize();
    bulletObjectPools       = new BulletObjectPools();
    equippedGun             = new SubmachineGun();
    playerCamera            = new PlayerCamera();
    currentState            = new PlayerIdleState(modelHandle, animationData);
}

/// <summary>
/// デストラクタ
/// </summary>
Player::~Player()
{
    delete(bulletObjectPools);
    delete(equippedGun);
    delete(currentState);
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
    state = State::Idle;
    
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
    PlayAnimation(AnimationType::Idle);
    animationData.currentAnimationCount     = currentAnimationCount;
    animationData.currentAnimationCount     = currentAnimationCount;
    animationData.previousAnimationCount    = previousAnimationCount;
    animationData.previousPlayAnimation     = previousPlayAnimation;
    animationData.animationFactor           = 0.0f;
}

/// <summary>
/// 更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="stage">ステージ</param>
void Player::Update(const Input& input, Stage& stage)
{
    // 現在のステートの更新
    TransitionInputState(input);
    ChangeState(state);
    currentState->Update();

    // 移動更新
    UpdateMovement(input, stage);

    // 射撃更新
    UpdateShootingEquippedWeapon(input);

    // リロード更新
    UpdateReload(input);

    // 装備中の武器の更新
    VECTOR pos = playerCamera->GetCameraForwardVector();
    equippedGun->Update(position, playerCamera->GetCameraForwardVector(),
        playerCamera->GetTargetPosition(),playerCamera->GetCameraPosition(),
        playerCamera->GetCameraPitch(),state);

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

    // デバッグ //
    // 座標描画
    DrawFormatString(DebugPositionDrawX, DebugPositionDrawY,
        DebugFontColor,"X:%f Y:%f Z:%f",position.x,position.y,position.z);
    // 現在ステートの描画
    switch (state)
    {
    case Player::State::Idle:
        DrawString(100, 200, "Idle", DebugFontColor, true);
        break;
    case Player::State::Walk:
        DrawString(100, 200, "Walk", DebugFontColor, true);
        break;
    case Player::State::Run:
        DrawString(100, 200, "Run", DebugFontColor, true);
        break;
    case Player::State::Shot:
        DrawString(100, 200, "Shot", DebugFontColor, true);
        break;
    case Player::State::Reload:
        DrawString(100, 200, "Reload", DebugFontColor, true);
        break;
    case Player::State::Jump:
        DrawString(100, 200, "Jump", DebugFontColor, true);
        break;
    case Player::State::OnHitEnemy:
        DrawString(100, 200, "OnHitEnemy", DebugFontColor, true);
        break;
    default:
        break;
    }
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
            state = State::Idle;
        }

        // 着地時はアニメーションのブレンドは行わない
        animationBlendRate = 1.0f;
    }
}

/// <summary>
/// 移動更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="stage">ステージ</param>
void Player::UpdateMovement(const Input& input, Stage& stage)
{
    // ルートフレームのＺ軸方向の移動パラメータを無効にする
    DisableRootFrameZMove();

    // パッド入力によって移動パラメータを設定する
    VECTOR  upModveVector;          // 方向ボタン「↑」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR  leftMoveVector;         // 方向ボタン「←」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR  currentFrameMoveVector; // このフレームの移動ベクトル

    // 移動ベクトルの更新
    UpdateMoveVector(input, upModveVector, leftMoveVector, currentFrameMoveVector);

    // 移動ボタンが押されていたら
    if (pressMoveButton)
    {
        // 移動速度を設定
        float moveSpeed = SettingMoveSpeed(state);

        // 移動ベクトルを正規化したものをプレイヤーが向くべき方向として保存
        targetMoveDirection = VNorm(currentFrameMoveVector);

        // プレイヤーが向くべき方向ベクトルをプレイヤーのスピード倍したものを移動ベクトルとする
        currentFrameMoveVector = VScale(targetMoveDirection, moveSpeed);
    }

    // プレイヤーモデルとプレイヤーカメラの回転率を同期させる
    UpdateAngle();

    // 移動ベクトルを元にコリジョンを考慮しつつプレイヤーを移動
    Move(currentFrameMoveVector, stage);
}

/// <summary>
/// ステートに応じた移動速度を渡す
/// </summary>
/// <param name="state">現在のステート</param>
/// <returns>移動速度</returns>
float Player::SettingMoveSpeed(State state)
{
    // 移動速度
    float moveSpeed = 0.0f;

    switch (state)
    {
    case Player::State::Idle:
        moveSpeed = 0.0f;
        break;

    case Player::State::Walk:
        moveSpeed = WalkMoveSpeed;
        break;

    case Player::State::Run:
        moveSpeed = RunMoveSpeed;
        break;

    case Player::State::Shot:
        moveSpeed = WalkMoveSpeed;
        break;

    case Player::State::Reload:
        moveSpeed = WalkMoveSpeed;
        break;

    case Player::State::Jump:
        moveSpeed = 0.0f;
        break;

    case Player::State::OnHitEnemy:
        moveSpeed = OnHitEnemyMoveSpeed;
        break;
    default:
        break;
    }

    return moveSpeed;
}

/// <summary>
/// ルートフレームのZ軸方向の移動パラメータを無効にする
/// </summary>
/// MEMO:
/// アニメーションによっては座標移動を含めたものもあるため
/// その前方向の移動量(Z軸方向)の移動を無効化し、
/// 自分のみが移動量を設定できるようにする
void Player::DisableRootFrameZMove()
{
    // HACK:
    // ・DXライブラリのモデルファイル内には、複数のメッシュ（ポリゴンの集合）やカメラ、ライトを入れることができる
    // ・置いた複数のファイルは、親子関係をつけたり、Unityのヒエラルキーみたいに、階層構造が作れる
    // ・この階層それぞれには名前が付けられる Root-Meshes-Model1
    //                                                   |-Model2
    // ・この名前の付いた階層のことをDXライブラリではフレームという
    // ・一番親の階層を「ルートフレーム」と呼ぶ。ルートフレームは一つ

    MATRIX localMatrix;

    // ユーザー行列を解除する
    // MEMO:
    // コード記入者がmodelHandleにMATRIXで回転変換を行ったものを、デフォルトに戻す
    MV1ResetFrameUserLocalMatrix(modelHandle, 2);

    // 現在のルートフレームの行列を取得する
    // MEMO:
    // すでにMATRIXを使用して回転していた回転量を返す
    localMatrix = MV1GetFrameLocalMatrix(modelHandle, 2);

    // Ｚ軸方向の平行移動成分を無効にする
    // m[0][3] はX軸方向の平行移動成分
    // m[1][3] はY軸方向の平行移動成分
    // m[2][3] はZ軸方向の平行移動成分
    localMatrix.m[3][2] = 0.0f;

    // ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
    MV1SetFrameUserLocalMatrix(modelHandle, 2, localMatrix);
}

/// <summary>
/// 移動ベクトルの更新
/// </summary>
/// <param name="input">入力情報</param>
/// <param name="upMoveVector">上方向ベクトル</param>
/// <param name="leftMoveVector">左方向ベクトル</param>
/// <param name="currentFrameMoveVector">移動ベクトル</param>
void Player::UpdateMoveVector(const Input& input, VECTOR& upModveVector,
    VECTOR& leftMoveVector, VECTOR& currentFrameMoveVector)
{
    // プレイヤーの移動方向のベクトルを算出
    // 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
    upModveVector   = VSub(playerCamera->GetTargetPosition(),
                           playerCamera->GetCameraPosition());
    upModveVector.y = 0.0f;

    // 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
    leftMoveVector = VCross(upModveVector, VGet(0.0f, 1.0f, 0.0f));

    // ベクトルの正規化
    upModveVector   = VNorm(upModveVector);
    leftMoveVector  = VNorm(leftMoveVector);

    // このフレームでの移動ベクトルを初期化
    currentFrameMoveVector = ZeroVector;

    // 移動用のボタンが入力されたかどうか
    pressMoveButton = false;

    // パッドの３ボタンと左シフトがどちらも押されていなかったらプレイヤーの移動処理
    if ((input.GetCurrentFrameInput() & PAD_INPUT_C) == 0)
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
    VECTOR oldPosition  = position;                     // 移動前の座標
    VECTOR nextPosition = VAdd(position, MoveVector);   // 移動後の座標

    // ステージとの当たり判定処理
    position = stage.IsHitCollision(*this, nextPosition, MoveVector);

    // 床より少し高くする
    if (position.y <= MoveLimitY)
    {
        position.y = MoveLimitY;
    }

    // 移動用の座標
    // MEMO:アニメーション再生時にY座標のみ下にしたいため別のVECTORを用意
    VECTOR movePosition = position;

    // 現在の適用率に基づいてオフセットを計算
    VECTOR offset   = currentState->GetStateOffsetValue();
    movePosition    = VAdd(position, offset);

    // プレイヤーのモデルの座標を更新する
    MV1SetPosition(modelHandle, movePosition);
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
    playerAngleY += HipUpPositionAngleY;

    // 腰だめ角度に調整
    cameraPitch += HipUpPositionANglePitch;

    // 回転を行列に変換
    MATRIX matrixX = MGetRotX(cameraPitch);             // X軸回転
    MATRIX matrixY = MGetRotY(playerAngleY);            // Y軸回転
    MATRIX matrixZ = MGetRotZ(0.0f);                    // Z軸回転

    // 回転行列を合成
    rotationMatrix = MMult(matrixX, matrixY);           // X軸回転した後Y軸回転
    rotationMatrix = MMult(rotationMatrix, matrixZ);    // Z軸回転


    // 走るアニメーションを再生
    if (state == State::Run)
    {
        // アニメーションカウントを進める
        runAnimationCount++;

        // 回転角度を設定
        // sin関数で[1]～[-1]を出してもらう
        // (アニメーションカウントを再生周期で割ることで現在どのくらい進んでいるかが分かる)
        float animationProgress = sin(DX_TWO_PI_F * runAnimationCount / RunAnimationFrameCycle);
        // 最大アングル * [１～ -１] = 角度
        // sinを使うことでマイナスの条件式を省く
        float angle = RunAnimationLimitAngle * animationProgress;

        // 回転行列を取得
        MATRIX runMatrix = MGetRotY(angle);

        // 回転行列を合成
        rotationMatrix = MMult(rotationMatrix, runMatrix);
    }

    // モデルに回転行列を適用
    MV1SetRotationMatrix(modelHandle, rotationMatrix);

}

/// <summary>
/// プレイヤーのアニメーションを新しく追加する
/// </summary>
/// <param name="PlayAnimation">再生したいアニメーション番号</param>
void Player::PlayAnimation(AnimationType type)
{
    // HACK: 指定した番号のアニメーションをアタッチし、直前に再生していたアニメーションの情報をpreviousに移行している
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
    currentPlayAnimation = MV1AttachAnim(modelHandle, static_cast<int>(type));
    currentAnimationCount = 0.0f;

    // ブレンド率はPrevが有効ではない場合は１．０ｆ( 現在モーションが１００％の状態 )にする
    animationBlendRate = previousPlayAnimation == -1 ? 1.0f : 0.0f;
}

/// <summary>
/// 銃を撃つ
/// </summary>
/// <param name="input">入力情報</param>
void Player::UpdateShootingEquippedWeapon(const Input& input)
{
    // 左クリックされたら射撃する
    if (input.GetMouseCurrentFrameInput() & MOUSE_INPUT_LEFT)
    {
        // 連射力カウントを進める
        shootFireRateCount++;

        // 連射間隔に達した場合、発射
        if (shootFireRateCount >= equippedGun->GetFireRate())
        {
            // 発砲している
            isShooting = true;

            // 弾丸の初期化用データを取得
            Bullet::BulletInitializeData initData = equippedGun->GetBulletInitializeData();

            // 未使用の弾丸をオブジェクトプールから取得
            Bullet* bullet = bulletObjectPools->GetInactiveBullet();

            // 取得した弾丸があるなら使用中に追加
            if (bullet != nullptr)
            {
                bullet->Initialize(initData);                       // 弾丸の初期化
                equippedGun->GetActiveBullet().push_back(bullet);   // 弾丸の追加
            }

            // 連射力カウントをリセット
            shootFireRateCount = 0;
        }
    }
    else
    {
        // 発砲していない
        isShooting = false;

        // 連射力カウントをリセット
        shootFireRateCount = 0;
    }

    // 使い終わった弾丸があれば返却する
    DeactivateBulletReturn();
}

/// <summary>
/// リロードの更新
/// </summary>
/// <param name="input">入力情報</param>
void Player::UpdateReload(const Input& input)
{
    // 「R」が押されたらリロード
    if (CheckHitKey(KEY_INPUT_R))
    {
        // リロードしている
        isReload = true;
    }

    // 一定フレーム期間リロードさせる
    if (isReload)
    {
        // タイマーを進める
        reloadTimer++;

        // リロードに必要なフレーム数になったら終了
        // FIXME:銃からリロードに必要な時間をもらうように変更する
        if (reloadTimer >= ReloadTimeFrame)
        {
            isReload = false;   // リロードをやめる
            reloadTimer = 0;    // タイマーをリセット
        }
    }
}

/// <summary>
/// 使い終わった弾丸をオブジェクトプールに返す
/// </summary>
void Player::DeactivateBulletReturn()
{
    // 使い終わったものを返す
    bulletObjectPools->ReturnActiveBulletInstance(equippedGun->GetActiveBullet());
}

/// <summary>
/// 入力に応じたステートの切り替えを行う
/// </summary>
/// <param name="input">入力情報</param>
void Player::TransitionInputState(const Input& input)
{
    // アイドル、歩き、走り、射撃、いずれかの状態か
    bool isIdleWalkRun = (state == State::Idle || state == State::Walk || state == State::Run || state == State::Shot || state == State::Reload);

    // リロード中はステート移動できない
    if (isReload)
    {
        // リロード
        ChangeState(Player::State::Reload);
        return;
    }
    else if(!pressMoveButton && !isShooting)   // 移動キーとショットボタンが押されていなければ
    {
        // アイドル
        ChangeState(State::Idle);
    }
    else if (isShooting)
    {
        // 発砲中
        ChangeState(State::Shot);
    }
    else if(isIdleWalkRun && CheckHitKey(KEY_INPUT_LSHIFT))
    {
        // 走り
        ChangeState(State::Run);
    }
    else if (isIdleWalkRun)
    {
        // 歩き
        ChangeState(State::Walk);
    }

    // 攻撃を受けた場合
    if (isIdleWalkRun)
    {
        if (isHitEnemyAttack)
        {
            ChangeState(State::OnHitEnemy);
        }
    }
}

/// <summary>
/// ステートの切り替え
/// </summary>
/// <param name="newState">新しいステート</param>
void Player::ChangeState(State newState)
{
    // 同じステートの場合は変更しない
    if (state == newState)return;

    // 現在のステートの情報を次に渡す用の変数
    PlayerStateBase::AnimationData previousData = currentState->GetStateAnimationData();

    // 前のステートを削除する
    delete(currentState);
    currentState = nullptr;

    // ステートの切り替え
    switch (newState)
    {
    case Player::State::Idle:
        // 何もしていない状態へ推移
        state = State::Idle;
        currentState = new PlayerIdleState(modelHandle,previousData);

        break;
    case Player::State::Walk:
        // 歩き状態に推移
        state = State::Walk;
        currentState = new PlayerWalkState(modelHandle, previousData);

        break;
    case Player::State::Run:
        // 走り状態に推移
        state = State::Run;
        currentState = new PlayerRunState(modelHandle, previousData);

        break;
    case Player::State::Shot:
        // 発砲状態に推移
        state = State::Shot;
        currentState = new PlayerShotState(modelHandle, previousData);

        break;
    case Player::State::Reload:
        // リロード状態に推移
        state = State::Reload;
        currentState = new PlayerReloadState(modelHandle, previousData);
        break;

    case Player::State::OnHitEnemy:
        // エネミーに攻撃されている状態へ推移
        state = State::OnHitEnemy;
        currentState = new PlayerOnHitEnemyState();

        break;
    default:
        break;
    }
}
