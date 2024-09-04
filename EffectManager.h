﻿#pragma once
#include "Common.h"

/// <summary>
/// エフェクト再生管理クラス
/// </summary>
class EffectManager     final
{
public:
    /// <summary>
    /// 再生状態
    /// </summary>
    enum EffectPlayState
    {
        NoPlay = -1,    // 再生されていない
        Play,           // 再生中
    };

    /// <summary>
    /// エフェクトの種類
    /// </summary>
    enum EffectType
    {
        // 銃
        MuzzleFlashEffect,      // マズルフラッシュ
        BloodSplatter,          // 血しぶき
        Explosion,              // 爆発
        BulletRay,              // 弾道
    };

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~EffectManager();

    /// <summary>
    /// インスタス作成
    /// </summary>
    static void CreateInstance();

    /// <summary>
    /// エフェクトマネージャーのインスタンスのポインタを渡す
    /// </summary>
    /// <returns>EffectManagerのポインタ</returns>
    static EffectManager* GetInstance();

    /// <summary>
    /// インスタンスの削除
    /// </summary>
    static void DeleteInstance();

    /// <summary>
    /// エフェクトの読み込み
    /// </summary>
    void LoadEffect();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// シーンが切り替わった時の処理
    /// </summary>
    void OnChangedScene();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 現在再生中のエフェクトがあるかどうか
    /// </summary>
    /// <returns></returns>
    bool HasPlayingEffect();

    /// <summary>
    /// 読み込んだエフェクトリストから再生
    /// </summary>
    /// <param name="effectType">再生するエフェクトの種類</param>
    /// <param name="playPosition">再生する座標</param>
    /// <param name="scale">※エフェクトの描画サイズ</param>
    /// NOTE:第3引数の「scale」はデフォルト引数　デフォルト値{1.0f,1.0f,1.0f}
    ///      1.0fが読み込み時サイズ
    void PlayEffectByEffectList(EffectType effectType, VECTOR playPosition, VECTOR scale = { 1.0f,1.0f,1.0f });

    /// <summary>
    /// マズルフラッシュエフェクト再生
    /// </summary>
    /// <param name="playPosition">再生座標</param>
    void PlayMuzzleFlashEffect(VECTOR playPosition);


    void UpdateMuzzleFlashEffect(VECTOR newPosition);

    /// <summary>
    /// 血しぶきのエフェクト再生
    /// </summary>
    /// <param name="playPosition">再生座標</param>
    void PlayBloodSplatterEffect(VECTOR playPosition);

    /// <summary>
    /// 強化済みの銃のマズルフラッシュエフェクトを再生する
    /// </summary>
    /// <param name="playPosition">再生座標</param>
    void PlayGunPowerUpMuzzleFlashEffect(VECTOR playPosition);

    /// <summary>
    /// 銃が強化された時のエフェクトを再生する
    /// </summary>
    /// <param name="playPosition">再生座標</param>
    void PlayGunPowerUpEffect(VECTOR playPosition);

private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    EffectManager();

    // 管理用
    static EffectManager*   effectManager;          // エフェクトマネージャーのインスタンス
    int                     playingEffectHandle;    // 現在再生中のエフェクトのハンドル
    vector<int>             playingList;            // 現在再生中のリスト
    map< EffectType, int>   effectList;             // 再生するエフェクトのリスト

    // ハンドル
    int muzzleFlashEffectHandle;        // マズルフラシュ
    int powerUpMuzzleFlashEffectHandle; // 強化済みの銃のマズルフラッシュ
    int gunPowerUpEffectHandle;         // 銃を強化した時のエフェクト
    int bloodSplatterEffectHandle;      // 血しぶき
    int explosionEffectHandle;          // 爆発
    int bulletRayEffectHandle;          // 弾道
};

