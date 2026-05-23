#ifndef JOB_H
#define JOB_H

#include <QString>
#include <QTimer>
#include <QObject>

class Pet;

// ============ 基类：Job (抽象) — 打工系统 ============
class Job : public QObject
{
    Q_OBJECT

public:
    Job(const QString &name, int durationSec, int reward, int energyCost, int risk);
    virtual ~Job();

    // ---- 纯虚函数（多态） ----
    virtual QString description() const = 0;
    virtual QString emoji() const = 0;

    // ---- 公共接口 ----
    QString name() const { return m_name; }
    int durationSec() const { return m_duration; }
    int reward() const { return m_reward; }
    int energyCost() const { return m_energyCost; }
    int risk() const { return m_risk; }

    // 执行工作 — 返回结果描述 + 实际获得金币
    QString execute(Pet *pet, int &earnedOut);

    // 打工过程中触发的事件
    virtual QString randomEvent() const;

signals:
    void finished(int earned);

protected:
    QString m_name;
    int m_duration;
    int m_reward;
    int m_energyCost;
    int m_risk;
};

// ============ 派生类：服务员 ============
class WaiterJob : public Job
{
public:
    WaiterJob();
    QString description() const override;
    QString emoji() const override;
    QString randomEvent() const override;
};

// ============ 派生类：快递员 ============
class DeliveryJob : public Job
{
public:
    DeliveryJob();
    QString description() const override;
    QString emoji() const override;
    QString randomEvent() const override;
};

// ============ 派生类：保安 ============
class GuardJob : public Job
{
public:
    GuardJob();
    QString description() const override;
    QString emoji() const override;
    QString randomEvent() const override;
};

#endif // JOB_H
