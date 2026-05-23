#ifndef PETMANAGER_H
#define PETMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "pet.h"
#include "item.h"

// ============ 单例：PetManager — 封装全局游戏状态 ============
class PetManager : public QObject
{
    Q_OBJECT

public:
    static PetManager *instance();

    // ---- 宠物管理 ----
    Pet *currentPet() { return m_pet; }
    void switchPet(int type); // 0=Cat, 1=Dog, 2=Bunny

    // ---- 金币系统 ----
    int coins() const { return m_coins; }
    void addCoins(int amount);
    bool spendCoins(int amount);

    // ---- 背包系统 ----
    const QVector<Item *> &inventory() const { return m_inventory; }
    void addItem(Item *item);
    void useItem(int index);

    // ---- 时间系统 ----
    void startTick();
    void pauseTick();

signals:
    void statsChanged();
    void coinsChanged(int coins);
    void inventoryChanged();
    void petChanged();

private:
    explicit PetManager(QObject *parent = nullptr);
    ~PetManager();
    PetManager(const PetManager &) = delete;
    PetManager &operator=(const PetManager &) = delete;

    Pet *m_pet = nullptr;
    int m_coins = 200;  // 初始金币
    QVector<Item *> m_inventory;
    QTimer *m_tickTimer = nullptr;
};

#endif // PETMANAGER_H
