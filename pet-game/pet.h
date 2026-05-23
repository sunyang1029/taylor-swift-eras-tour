#ifndef PET_H
#define PET_H

#include <QString>
#include <QPainter>
#include <QPointF>
#include <QRectF>

// ============ 基类：Pet (抽象) — 封装所有宠物状态 ============
class Pet
{
public:
    Pet(const QString &name);
    virtual ~Pet();

    // ---- 纯虚函数（多态） ----
    virtual QString species() const = 0;
    virtual void draw(QPainter &p, const QRectF &bounds, int frame) const = 0;
    virtual QString speak() const = 0;

    // ---- 公共接口 ----
    void feed(int amount);
    void play(int amount);
    void heal(int amount);
    void rest(int amount);

    // ---- 状态查询 ----
    QString name() const { return m_name; }
    int hunger() const { return m_hunger; }
    int mood() const { return m_mood; }
    int health() const { return m_health; }
    int energy() const { return m_energy; }

    void setHunger(int v) { m_hunger = qBound(0, v, 100); }
    void setMood(int v)   { m_mood   = qBound(0, v, 100); }
    void setHealth(int v) { m_health = qBound(0, v, 100); }
    void setEnergy(int v) { m_energy = qBound(0, v, 100); }

    // 时间流逝 — 所有属性自然衰减
    virtual void tick();

    // 判断宠物状态
    bool isHappy() const { return m_mood > 60; }
    bool isHungry() const { return m_hunger < 30; }
    bool isSick() const { return m_health < 25; }

protected:
    QString m_name;
    int m_hunger;  // 0-100 (100=饱)
    int m_mood;    // 0-100 (100=开心)
    int m_health;  // 0-100 (100=健康)
    int m_energy;  // 0-100 (100=精力充沛)
};

// ============ 派生类：猫咪 ============
class Cat : public Pet
{
public:
    Cat() : Pet(QString::fromUtf8("小橘")) {}
    QString species() const override { return QString::fromUtf8("猫"); }
    void draw(QPainter &p, const QRectF &bounds, int frame) const override;
    QString speak() const override;
};

// ============ 派生类：狗狗 ============
class Dog : public Pet
{
public:
    Dog() : Pet(QString::fromUtf8("旺财")) {}
    QString species() const override { return QString::fromUtf8("狗"); }
    void draw(QPainter &p, const QRectF &bounds, int frame) const override;
    QString speak() const override;
};

// ============ 派生类：兔兔 ============
class Bunny : public Pet
{
public:
    Bunny() : Pet(QString::fromUtf8("团子")) {}
    QString species() const override { return QString::fromUtf8("兔"); }
    void draw(QPainter &p, const QRectF &bounds, int frame) const override;
    QString speak() const override;
};

#endif // PET_H
