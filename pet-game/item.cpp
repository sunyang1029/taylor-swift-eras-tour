#include "item.h"

// ==================== Item 基类 ====================

Item::Item(const QString &name, const QString &desc, int price, int effect)
    : m_name(name), m_desc(desc), m_price(price), m_effect(effect)
{}

Item::~Item() {}

// ==================== Food 实现 ====================

void Food::use(Pet *pet)
{
    pet->feed(m_effect);
}

QString Food::icon() const
{
    // 食物 emoji 映射
    if (m_name.contains(QString::fromUtf8("鱼")))   return QString::fromUtf8("🐟");
    if (m_name.contains(QString::fromUtf8("骨头"))) return QString::fromUtf8("🦴");
    if (m_name.contains(QString::fromUtf8("萝卜"))) return QString::fromUtf8("🥕");
    if (m_name.contains(QString::fromUtf8("粮")))   return QString::fromUtf8("🍚");
    if (m_name.contains(QString::fromUtf8("罐头"))) return QString::fromUtf8("🥫");
    if (m_name.contains(QString::fromUtf8("蛋糕"))) return QString::fromUtf8("🍰");
    if (m_name.contains(QString::fromUtf8("奶")))   return QString::fromUtf8("🥛");
    return QString::fromUtf8("🍖");
}

// ==================== Toy 实现 ====================

void Toy::use(Pet *pet)
{
    pet->play(m_effect);
}

QString Toy::icon() const
{
    if (m_name.contains(QString::fromUtf8("球")))     return QString::fromUtf8("⚽");
    if (m_name.contains(QString::fromUtf8("毛线")))   return QString::fromUtf8("🧶");
    if (m_name.contains(QString::fromUtf8("飞盘")))   return QString::fromUtf8("🥏");
    if (m_name.contains(QString::fromUtf8("老鼠")))   return QString::fromUtf8("🐭");
    if (m_name.contains(QString::fromUtf8("铃铛")))   return QString::fromUtf8("🔔");
    if (m_name.contains(QString::fromUtf8("磨牙")))   return QString::fromUtf8("🪵");
    return QString::fromUtf8("🎾");
}

// ==================== Medicine 实现 ====================

void Medicine::use(Pet *pet)
{
    pet->heal(m_effect);
}

QString Medicine::icon() const
{
    if (m_name.contains(QString::fromUtf8("药膏"))) return QString::fromUtf8("💊");
    if (m_name.contains(QString::fromUtf8("疫苗"))) return QString::fromUtf8("💉");
    if (m_name.contains(QString::fromUtf8("绷带"))) return QString::fromUtf8("🩹");
    return QString::fromUtf8("💊");
}
