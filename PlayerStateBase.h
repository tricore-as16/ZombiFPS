#pragma once
#include "Common.h"

/// <summary>
/// プレイヤーステート基礎クラス
/// </summary>
class PlayerStateBase
{
public:
    /// <summary>
    /// アニメーションタイプ
    /// </summary>
    /// TODO：アニメーションの消し方が分からないため使用する箇所のみ名前付け
    enum class AnimationStateType : int
    {
        NoAnimation0,           // アニメーションなし
        NoAnimation1,           // アニメーションなし
        NoAnimation2,           // アニメーションなし
        NoAnimation3,           // アニメーションなし
        NoAnimation4,           // アニメーションなし
        NoAnimation5,           // アニメーションなし
        NoAnimation6,           // アニメーションなし
        NoAnimation7,           // アニメーションなし
        Idle,                   // 何もしていない
        Reloading,              // リロード
        ReloadingCocking,       // リロード＋コッキング
        Shot,                   // 発砲
        NoAnimation8,           // アニメーションなし
    };

    /// <summary>
    /// アニメーション変更時に受け渡す用のデータ
    /// </summary>
    struct AnimationData
    {
        int         currentPlayAnimation;       // 再生しているアニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
        float       currentAnimationCount;      // 再生しているアニメーションの再生時間
        int         previousPlayAnimation;      // 前の再生アニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
        float       previousAnimationCount;     // 前の再生アニメーションの再生時間
        float       animationFactor;            // アニメーションの適用率
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    PlayerStateBase();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="playerModelHandle">プレイヤーモデルハンドル</param>
    /// <param name="previousStateData">前のステートの情報</param>
    PlayerStateBase(int playerModelHandle, AnimationData previousStateData);

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~PlayerStateBase();

    /// <summary>
    /// 更新
    /// </summary>
    virtual void Update() abstract;

    // Getter
    const AnimationData GetStateAnimationData()const { return nowStateData; }
    const VECTOR GetStateOffsetValue()const { return stateOffsetValue; }

protected:
    /// <summary>
    /// 新しくアニメーションを再生する
    /// </summary>
    /// <param name="type">再生したいアニメーション番号</param>
    void PlayNewAnimation(AnimationStateType type);

    /// <summary>
    /// アニメーション更新処理
    /// </summary>
    void UpdateAnimation();

    /// <summary>
    /// プレイヤーのステートごとの座標修正
    /// </summary>
    /// TODO :オーバーライドさせている分のifをベース側の関数でまとめて
    ///       加算する符号、オフセット量、再生速度、引数で受け取れるようにする
    virtual void UpdateOffsetValue();

    /// <summary>
    /// アニメーションデータの更新
    /// </summary>
    void UpdateAnimationData();

    //---------------------------------------------------------------------------------//
    //                                      定数                                       //
    //---------------------------------------------------------------------------------//
    static constexpr float  MaxAnimationBlendRate   = 1.0f;             // アニメーションのブレンド率の最大値
    static constexpr int    NoAnimationAttached     = -1;               // アニメーションアタッチがされていない
    static constexpr float  AnimationFactorSpeed    = 0.05f;         // プログラムで作成したアニメーションの適用率
    static constexpr VECTOR AnimationOffsetValue    = { 0,-0.5f,0 };    // 基本のずらし量(ステートごとにずらす量を定義する)

    //---------------------------------------------------------------------------------//
    //                                      変数                                       //
    //---------------------------------------------------------------------------------//

    // アニメーション関係
    int             modelHandle;                // モデルハンドル
    int             currentPlayAnimation;       // 再生しているアニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float           currentAnimationCount;      // 再生しているアニメーションの再生時間
    int             previousPlayAnimation;      // 前の再生アニメーションのアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float           previousAnimationCount;     // 前の再生アニメーションの再生時間
    float           animationBlendRate;         // 現在と過去のアニメーションのブレンド率
    bool            currentFrameMove;           // そのフレームで動いたかどうか
    bool            pressMoveButton;            // 移動用のボタンが入力されているか
    float           animationBlendSpeed;        // アニメーションのブレンド速度
    float           playAnimationSpeed;         // アニメーション再生速度
    AnimationData   nowStateData;               // 次のステートへ渡す情報
    float           animationFactor;            // アニメーションの適用率
    VECTOR          stateOffsetValue;           // ステートごとのプレイヤーずらし量
};

