﻿#pragma once
#include "Common.h"

// 当たり判定の情報を持った構造体を宣言するヘッダー

struct LineCollisionData;
struct SphereCollisionData;
struct CapsuleCollisionData;

/// <summary>
/// オブジェクトの種別
/// </summary>
enum class ObjectTag : int
{
    Player,     // プレイヤー
    Enemy,      // エネミー
    EnemyHead,  // エネミーの頭
    EnemyBoby,  // エネミーの胴体
    Bullet,     // 弾丸
};

/// <summary>
/// 当たり判定用情報
/// </summary>
struct CollisionData
{
    ObjectTag   tag;                    // このオブジェクトが何なのかのタグ(Player、Enemy、Bullet）
    // 球用
    VECTOR      centerPosition;         // 球の中心座標

    // カプセル用
    VECTOR      startPosition;          // カプセルを形成し始める座標
    VECTOR      endPosition;            // カプセルの形成を終わる座標
    float       radius;                 // カプセルの半径

    // 線形用
    VECTOR      lineStartPosition;      // 線の始点
    VECTOR      lineEndPosition;        // 線の終点
    int         bulletPower;            // 弾丸の威力


    std::function<void(CollisionData)> onHit;   // 当たった時に行われる処理
    bool        isCollisionActive;              // 当たり判定を適用させたいかどうか
};
