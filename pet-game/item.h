#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include "pet.h"

// ============ 基类：Item (抽象) — 封装道具信息 ============
class Item
{
public:
    Item(const QString &name, const QString &desc, int price, int effect);
    virtual ~Item();

    // ---- 纯虚函数（多态） ----
    virtual void use(Pet *pet) = 0;
    virtual QString icon() const = 0;
    virtual QString category() const = 0;

    // ---- 公共接口 ----
    QString name() const { return m_name; }
    QString description() const { return m_desc; }
    int price() const { return m_price; }
    int effect() const { return m_effect; }

protected:
    QString m_name;
    QString m_desc;
    int m_price;
    int m_effect;
};

// ============ 派生类：食物 ============
class Food : public Item
{
public:
    Food(const QString &name, const QString &desc, int price, int effect)
        : Item(name, desc, price, effect) {}
    void use(Pet *pet) override;
    QString icon() const override;
    QString category() const override { return QString::fromUtf8("食物"); }
};

// ============ 派生类：玩具 ============
class Toy : public Item
{
public:
    Toy(const QString &name, const QString &desc, int price, int effect)
        : Item(name, desc, price, effect) {}
    void use(Pet *pet) override;
    QString icon() const override;
    QString category() const override { return QString::fromUtf8("玩具"); }
};

// ============ 派生类：药品 ============
class Medicine : public Item
{
public:
    Medicine(const QString &name, const QString &desc, int price, int effect)
        : Item(name, desc, price, effect) {}
    void use(Pet *pet) override;
    QString icon() const override;
    QString category() const override { return QString::fromUtf8("药品"); }
};

#endif // ITEM_H
