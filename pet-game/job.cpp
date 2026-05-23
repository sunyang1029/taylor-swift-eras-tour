#include "job.h"
#include "pet.h"
#include <QRandomGenerator>

// ==================== Job 基类 ====================

Job::Job(const QString &name, int durationSec, int reward, int energyCost, int risk)
    : QObject(nullptr), m_name(name), m_duration(durationSec),
      m_reward(reward), m_energyCost(energyCost), m_risk(risk)
{}

Job::~Job() {}

QString Job::execute(Pet *pet, int &earnedOut)
{
    pet->setEnergy(pet->energy() - m_energyCost);

    // 风险判定
    int roll = QRandomGenerator::global()->bounded(100);
    if (roll < m_risk * 8) {
        pet->setHealth(pet->health() - m_risk * 5);
        pet->setMood(pet->mood() - 10);
        earnedOut = m_reward / 2;
        return randomEvent();
    }

    // 正常收益
    int bonus = QRandomGenerator::global()->bounded(m_reward / 3);
    earnedOut = m_reward + bonus;
    return QString::fromUtf8("工作完成！获得了 %1 金币！").arg(earnedOut);
}

QString Job::randomEvent() const
{
    return QString::fromUtf8("工作中出了点小意外...");
}

// ==================== WaiterJob ====================

WaiterJob::WaiterJob()
    : Job(QString::fromUtf8("餐厅服务员"), 5, 30, 15, 1)
{}

QString WaiterJob::description() const
{
    return QString::fromUtf8("在高级餐厅端盘子 | 安全稳定");
}

QString WaiterJob::emoji() const { return QString::fromUtf8("🍽️"); }

QString WaiterJob::randomEvent() const
{
    QStringList events = {
        QString::fromUtf8("不小心打翻了一杯水，被扣了点工资..."),
        QString::fromUtf8("客人给了小费！额外收入！"),
    };
    return events[QRandomGenerator::global()->bounded(events.size())];
}

// ==================== DeliveryJob ====================

DeliveryJob::DeliveryJob()
    : Job(QString::fromUtf8("快递配送员"), 8, 50, 25, 3)
{}

QString DeliveryJob::description() const
{
    return QString::fromUtf8("骑着小电驴送快递 | 中等风险");
}

QString DeliveryJob::emoji() const { return QString::fromUtf8("📦"); }

QString DeliveryJob::randomEvent() const
{
    QStringList events = {
        QString::fromUtf8("路上堵车了，迟到了几分钟..."),
        QString::fromUtf8("遇到了一只流浪猫，耽误了一会儿 🐱"),
        QString::fromUtf8("快递被雨淋了一点，赔了钱 😢"),
    };
    return events[QRandomGenerator::global()->bounded(events.size())];
}

// ==================== GuardJob ====================

GuardJob::GuardJob()
    : Job(QString::fromUtf8("夜班保安"), 10, 80, 35, 5)
{}

QString GuardJob::description() const
{
    return QString::fromUtf8("深夜守护大楼 | 高风险高回报");
}

QString GuardJob::emoji() const { return QString::fromUtf8("🛡️"); }

QString GuardJob::randomEvent() const
{
    QStringList events = {
        QString::fromUtf8("半夜被奇怪的声音吓了一跳..."),
        QString::fromUtf8("抓到一只偷吃零食的小老鼠！"),
        QString::fromUtf8("一不小心在值班室睡着了 😴"),
    };
    return events[QRandomGenerator::global()->bounded(events.size())];
}
