#include "pet.h"
#include <QtMath>
#include <QColor>
#include <QPainterPath>
#include <QRandomGenerator>

// ==================== Pet 基类实现 ====================

Pet::Pet(const QString &name)
    : m_name(name), m_hunger(80), m_mood(70), m_health(100), m_energy(90)
{}

Pet::~Pet() {}

void Pet::feed(int amount)
{
    m_hunger = qMin(100, m_hunger + amount);
    m_health = qMin(100, m_health + amount / 4);
}

void Pet::play(int amount)
{
    m_mood = qMin(100, m_mood + amount);
    m_energy = qMax(0, m_energy - amount / 3);
}

void Pet::heal(int amount)
{
    m_health = qMin(100, m_health + amount);
}

void Pet::rest(int amount)
{
    m_energy = qMin(100, m_energy + amount);
}

void Pet::tick()
{
    m_hunger = qMax(0, m_hunger - 2);
    m_mood   = qMax(0, m_mood   - 1);
    m_energy = qMax(0, m_energy - 1);
    if (m_hunger < 20 || m_energy < 10)
        m_health = qMax(0, m_health - 1);
}

// ==================== Cat 绘制 + 对话 ====================

QString Cat::speak() const
{
    auto rng = QRandomGenerator::global();
    if (isHungry()) {
        QStringList lines = {
            QString::fromUtf8("喵呜～肚子咕咕叫了！🐟"),
            QString::fromUtf8("快给我小鱼干！不然我要偷吃你的零食了！"),
            QString::fromUtf8("喵...饿得我胡子都耷拉下来了..."),
            QString::fromUtf8("开饭时间到了吗？我已经准备好了！🍚"),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (isSick()) {
        QStringList lines = {
            QString::fromUtf8("喵...我不太舒服...要抱抱...🤒"),
            QString::fromUtf8("头好晕...主人你在哪里..."),
            QString::fromUtf8("喵呜...需要去看兽医吗？不要啊..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (!isHappy()) {
        QStringList lines = {
            QString::fromUtf8("喵...好无聊啊，陪我玩嘛～"),
            QString::fromUtf8("主人你是不是不爱我了...(´;ω;`)"),
            QString::fromUtf8("喵～逗猫棒在哪里？激光笔也行！"),
            QString::fromUtf8("看我在地上打滚...没人理我..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (m_energy < 30) {
        QStringList lines = {
            QString::fromUtf8("呼噜呼噜...zzZ...好困..."),
            QString::fromUtf8("我要睡午觉了，不要吵我哦～😴"),
            QString::fromUtf8("哈欠～今天抓老鼠太累了..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    QStringList happy = {
        QString::fromUtf8("喵喵！最喜欢主人啦！"),
        QString::fromUtf8("呼噜呼噜～被摸得好舒服～"),
        QString::fromUtf8("喵！今天天气真好，我们去冒险吧！"),
        QString::fromUtf8("(=^･ω･^=) 喵～"),
        QString::fromUtf8("我发现了一个超好玩的纸箱子！📦"),
        QString::fromUtf8("蹭蹭主人的腿～喵呜～"),
        QString::fromUtf8("今天抓到一只蟑螂！我厉害吧！"),
        QString::fromUtf8("喵生最大的幸福就是晒太阳＋吃小鱼干～"),
        QString::fromUtf8("窗外面有一只鸟！我要...我看看而已..."),
        QString::fromUtf8("主人的键盘好暖和，我要趴在上面～"),
    };
    return happy[rng->bounded(happy.size())];
}

void Cat::draw(QPainter &p, const QRectF &bounds, int frame) const
{
    p.setRenderHint(QPainter::Antialiasing);
    qreal s = qMin(bounds.width(), bounds.height()) / 200.0;
    qreal cx = bounds.center().x();
    qreal cy = bounds.center().y() + 10 * s;

    // 尾巴 — 根据 frame 摆动
    qreal tailWag = qSin(frame * 0.15) * 12 * s;
    QPainterPath tail;
    tail.moveTo(cx + 40 * s, cy + 10 * s);
    tail.cubicTo(cx + 65 * s, cy + 15 * s + tailWag,
                 cx + 70 * s, cy - 5 * s + tailWag,
                 cx + 55 * s, cy - 25 * s + tailWag);
    p.setPen(QPen(QColor("#E87A36"), 6 * s, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    p.drawPath(tail);

    // 身体
    QRadialGradient bodyGrad(cx, cy + 15 * s, 48 * s);
    bodyGrad.setColorAt(0, QColor("#FFB347"));
    bodyGrad.setColorAt(1, QColor("#E87A36"));
    p.setPen(Qt::NoPen);
    p.setBrush(bodyGrad);
    p.drawEllipse(QPointF(cx, cy + 15 * s), 38 * s, 42 * s);

    // 耳朵 (左)
    QPolygonF earL;
    earL << QPointF(cx - 22 * s, cy - 20 * s)
         << QPointF(cx - 32 * s, cy - 60 * s)
         << QPointF(cx - 6 * s,  cy - 25 * s);
    p.setBrush(QColor("#E87A36"));
    p.drawPolygon(earL);
    // 内耳
    QPolygonF earLIn;
    earLIn << QPointF(cx - 20 * s, cy - 22 * s)
           << QPointF(cx - 27 * s, cy - 48 * s)
           << QPointF(cx - 10 * s, cy - 26 * s);
    p.setBrush(QColor("#F5C6B3"));
    p.drawPolygon(earLIn);

    // 耳朵 (右)
    QPolygonF earR;
    earR << QPointF(cx + 22 * s, cy - 20 * s)
         << QPointF(cx + 32 * s, cy - 60 * s)
         << QPointF(cx + 6 * s,  cy - 25 * s);
    p.setBrush(QColor("#E87A36"));
    p.drawPolygon(earR);
    QPolygonF earRIn;
    earRIn << QPointF(cx + 20 * s, cy - 22 * s)
           << QPointF(cx + 27 * s, cy - 48 * s)
           << QPointF(cx + 10 * s, cy - 26 * s);
    p.setBrush(QColor("#F5C6B3"));
    p.drawPolygon(earRIn);

    // 脸部浅色区域
    p.setBrush(QColor("#FFD59A"));
    p.drawEllipse(QPointF(cx, cy + 2 * s), 26 * s, 24 * s);

    // 眼睛 (大眼 = 可爱)
    auto drawEye = [&](qreal ex, qreal ey) {
        // 眼白
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex, ey), 9 * s, 10 * s);
        // 瞳孔
        p.setBrush(QColor("#2D1B00"));
        p.drawEllipse(QPointF(ex + 1 * s, ey + 1 * s), 5.5 * s, 6.5 * s);
        // 高光
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex - 2 * s, ey - 3 * s), 2.5 * s, 2.5 * s);
    };
    drawEye(cx - 11 * s, cy - 5 * s);
    drawEye(cx + 11 * s, cy - 5 * s);

    // 鼻子
    p.setBrush(QColor("#FF8C94"));
    QPolygonF nose;
    nose << QPointF(cx - 3 * s, cy + 6 * s)
         << QPointF(cx + 3 * s, cy + 6 * s)
         << QPointF(cx, cy + 9.5 * s);
    p.drawPolygon(nose);

    // 嘴巴
    p.setPen(QPen(QColor("#5D3A1A"), 1.2 * s));
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRectF(cx - 5 * s, cy + 7 * s, 10 * s, 5 * s), 0, -180 * 16);

    // 胡须
    p.setPen(QPen(QColor("#5D3A1A"), 0.8 * s));
    for (int side = -1; side <= 1; side += 2) {
        qreal bx = cx + side * 14 * s;
        qreal by = cy + 6 * s;
        p.drawLine(QPointF(bx, by), QPointF(bx + side * 18 * s, by - 4 * s));
        p.drawLine(QPointF(bx, by + 2 * s), QPointF(bx + side * 18 * s, by + 1 * s));
        p.drawLine(QPointF(bx, by + 4 * s), QPointF(bx + side * 16 * s, by + 7 * s));
    }

    // 前爪
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#FFD59A"));
    p.drawRoundedRect(QRectF(cx - 16 * s, cy + 45 * s, 12 * s, 10 * s), 5 * s, 5 * s);
    p.drawRoundedRect(QRectF(cx + 4 * s, cy + 45 * s, 12 * s, 10 * s), 5 * s, 5 * s);
}

// ==================== Dog 绘制 + 对话 ====================

QString Dog::speak() const
{
    auto rng = QRandomGenerator::global();
    if (isHungry()) {
        QStringList lines = {
            QString::fromUtf8("汪汪！闻到肉骨头的香味了！🦴"),
            QString::fromUtf8("饿死啦！快给我吃的，不然拆家了！"),
            QString::fromUtf8("呜～汪！饭碗已经空了..."),
            QString::fromUtf8("盯着你看...快发现我饿了！！"),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (isSick()) {
        QStringList lines = {
            QString::fromUtf8("呜...汪...我好像发烧了..."),
            QString::fromUtf8("鼻子好干...我不舒服...抱抱我..."),
            QString::fromUtf8("今天不想散步了...汪..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (!isHappy()) {
        QStringList lines = {
            QString::fromUtf8("汪...主人你是不是不爱我了？"),
            QString::fromUtf8("好想出去玩...飞盘都积灰了..."),
            QString::fromUtf8("哼！我要生气了！不理你了！(五秒后就理)"),
            QString::fromUtf8("呜呜呜～看我可怜的小眼神..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (m_energy < 30) {
        QStringList lines = {
            QString::fromUtf8("哈欠～我先趴一会儿..."),
            QString::fromUtf8("今天跑太多了...腿都软了..."),
            QString::fromUtf8("zzZ...在梦里追兔子..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    QStringList happy = {
        QString::fromUtf8("汪汪汪！主人回来啦！好开心！"),
        QString::fromUtf8("汪！我们去散步吧！"),
        QString::fromUtf8("尾巴摇摇～最喜欢和主人一起玩了！"),
        QString::fromUtf8("汪呜～你是世界上最好的主人！"),
        QString::fromUtf8("我刚刚学会了新技能：握手！🤝"),
        QString::fromUtf8("楼下有个快递！我帮你拿！"),
        QString::fromUtf8("嗷呜～～～(开心得学狼叫)"),
        QString::fromUtf8("飞盘飞盘飞盘！快扔飞盘！"),
        QString::fromUtf8("今天在公园交到了一个新朋友！🐕"),
        QString::fromUtf8("舔舔主人的手～汪！"),
    };
    return happy[rng->bounded(happy.size())];
}

void Dog::draw(QPainter &p, const QRectF &bounds, int frame) const
{
    p.setRenderHint(QPainter::Antialiasing);
    qreal s = qMin(bounds.width(), bounds.height()) / 200.0;
    qreal cx = bounds.center().x();
    qreal cy = bounds.center().y() + 10 * s;

    // 尾巴 — 左右摇摆
    qreal wag = qSin(frame * 0.2) * 15 * s;
    p.setPen(QPen(QColor("#C8945A"), 5 * s, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    QPainterPath tailPath;
    tailPath.moveTo(cx + 38 * s, cy - 8 * s);
    tailPath.cubicTo(cx + 55 * s, cy - 25 * s + wag,
                     cx + 50 * s, cy - 45 * s + wag,
                     cx + 35 * s, cy - 50 * s + wag);
    p.drawPath(tailPath);

    // 身体
    QRadialGradient bodyGrad(cx, cy + 10 * s, 45 * s);
    bodyGrad.setColorAt(0, QColor("#E8C97A"));
    bodyGrad.setColorAt(0.7, QColor("#C8945A"));
    bodyGrad.setColorAt(1, QColor("#A6773B"));
    p.setPen(Qt::NoPen);
    p.setBrush(bodyGrad);
    p.drawEllipse(QPointF(cx, cy + 10 * s), 36 * s, 40 * s);

    // 耳朵 (垂耳)
    p.setBrush(QColor("#B5824A"));
    p.drawEllipse(QPointF(cx - 30 * s, cy - 5 * s), 8 * s, 18 * s);
    p.drawEllipse(QPointF(cx + 30 * s, cy - 5 * s), 8 * s, 18 * s);

    // 脸部浅色区域
    p.setBrush(QColor("#F5DEB3"));
    p.drawEllipse(QPointF(cx, cy - 2 * s), 28 * s, 25 * s);

    // 眼睛
    auto drawDogEye = [&](qreal ex, qreal ey) {
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex, ey), 8 * s, 9 * s);
        p.setBrush(QColor("#3D1C00"));
        p.drawEllipse(QPointF(ex + 1 * s, ey + 1 * s), 5 * s, 6 * s);
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex - 1.5 * s, ey - 2.5 * s), 2 * s, 2 * s);
    };
    drawDogEye(cx - 10 * s, cy - 8 * s);
    drawDogEye(cx + 10 * s, cy - 8 * s);

    // 眉毛
    p.setPen(QPen(QColor("#5D3A1A"), 2 * s, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    p.drawLine(QPointF(cx - 17 * s, cy - 19 * s), QPointF(cx - 5 * s, cy - 16 * s));
    p.drawLine(QPointF(cx + 17 * s, cy - 19 * s), QPointF(cx + 5 * s, cy - 16 * s));

    // 鼻子
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#2D1B00"));
    p.drawEllipse(QPointF(cx, cy + 3 * s), 6 * s, 5 * s);

    // 嘴巴
    p.setPen(QPen(QColor("#5D3A1A"), 1.2 * s));
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRectF(cx - 6 * s, cy + 4 * s, 12 * s, 6 * s), 0, -180 * 16);

    // 舌头
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#FF8888"));
    QPainterPath tongue;
    tongue.addRoundedRect(QRectF(cx - 2 * s, cy + 10 * s, 4 * s, 7 * s), 2 * s, 2 * s);
    p.drawPath(tongue);

    // 前爪
    p.setBrush(QColor("#F5DEB3"));
    p.drawRoundedRect(QRectF(cx - 18 * s, cy + 42 * s, 14 * s, 10 * s), 6 * s, 6 * s);
    p.drawRoundedRect(QRectF(cx + 4 * s, cy + 42 * s, 14 * s, 10 * s), 6 * s, 6 * s);
}

// ==================== Bunny 绘制 + 对话 ====================

QString Bunny::speak() const
{
    auto rng = QRandomGenerator::global();
    if (isHungry()) {
        QStringList lines = {
            QString::fromUtf8("噗噗... 胡萝卜在哪里呀？🥕"),
            QString::fromUtf8("兔兔的肚子在唱歌～咕噜噜～"),
            QString::fromUtf8("有没有新鲜的草草吃呀..."),
            QString::fromUtf8("我要吃！给我吃！不给就蹦你脸上！"),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (isSick()) {
        QStringList lines = {
            QString::fromUtf8("嘤... 兔兔生病了，要抱抱..."),
            QString::fromUtf8("鼻子上没水了...好难受..."),
            QString::fromUtf8("不要丢下兔兔一个人..."),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (!isHappy()) {
        QStringList lines = {
            QString::fromUtf8("噗... 没有人陪兔兔玩吗？"),
            QString::fromUtf8("兔兔好孤独...谁来摸摸我的头..."),
            QString::fromUtf8("我要蹦走了！去远方冒险！"),
            QString::fromUtf8("噘嘴...哼！兔兔不开心了！"),
        };
        return lines[rng->bounded(lines.size())];
    }
    if (m_energy < 30) {
        QStringList lines = {
            QString::fromUtf8("蹦不动了...兔兔要睡觉觉..."),
            QString::fromUtf8("呼... 今天蹦跶太多了...💤"),
            QString::fromUtf8("给我一个软软的枕头～"),
        };
        return lines[rng->bounded(lines.size())];
    }
    QStringList happy = {
        QString::fromUtf8("噗噗！今天是最开心的一天！"),
        QString::fromUtf8("蹦蹦跳跳～兔兔爱主人！"),
        QString::fromUtf8("(◕‿◕) 主人摸摸头～好舒服～"),
        QString::fromUtf8("噗～给你一朵小花花！🌸"),
        QString::fromUtf8("看我一蹦三尺高！嘿！"),
        QString::fromUtf8("兔兔的耳朵在跳舞～♪"),
        QString::fromUtf8("我刚刚藏了一颗糖，但我忘了藏哪里了..."),
        QString::fromUtf8("主人主人！你看我新学的后空翻！"),
        QString::fromUtf8("世界上最幸福的兔子就是本兔啦！"),
        QString::fromUtf8("蹭蹭蹭～主人的手好暖～"),
    };
    return happy[rng->bounded(happy.size())];
}

void Bunny::draw(QPainter &p, const QRectF &bounds, int frame) const
{
    p.setRenderHint(QPainter::Antialiasing);
    qreal s = qMin(bounds.width(), bounds.height()) / 200.0;
    qreal cx = bounds.center().x();
    qreal cy = bounds.center().y() + 12 * s;

    // 耳朵 (左) — 微微抖动
    qreal earWobble = qSin(frame * 0.1) * 3 * s;
    QPainterPath earL;
    earL.addRoundedRect(QRectF(cx - 18 * s, cy - 78 * s, 12 * s, 52 * s), 6 * s, 6 * s);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#F5F5F5"));
    p.rotate(earWobble);
    p.drawPath(earL);
    p.resetTransform();
    // 左内耳
    QPainterPath earLIn;
    earLIn.addRoundedRect(QRectF(cx - 15 * s, cy - 68 * s, 6 * s, 36 * s), 3 * s, 3 * s);
    p.setBrush(QColor("#FFD1DC"));
    p.rotate(earWobble);
    p.drawPath(earLIn);
    p.resetTransform();

    // 耳朵 (右)
    QPainterPath earR;
    earR.addRoundedRect(QRectF(cx + 6 * s, cy - 80 * s, 12 * s, 52 * s), 6 * s, 6 * s);
    p.setBrush(QColor("#F5F5F5"));
    p.rotate(-earWobble);
    p.drawPath(earR);
    p.resetTransform();
    QPainterPath earRIn;
    earRIn.addRoundedRect(QRectF(cx + 9 * s, cy - 70 * s, 6 * s, 36 * s), 3 * s, 3 * s);
    p.setBrush(QColor("#FFD1DC"));
    p.rotate(-earWobble);
    p.drawPath(earRIn);
    p.resetTransform();

    // 身体 — 椭圆
    QRadialGradient bodyGrad(cx, cy + 5 * s, 38 * s);
    bodyGrad.setColorAt(0, QColor("#FFFFFF"));
    bodyGrad.setColorAt(1, QColor("#E8E8E8"));
    p.setBrush(bodyGrad);
    p.drawEllipse(QPointF(cx, cy + 10 * s), 32 * s, 36 * s);

    // 脸部
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(QPointF(cx, cy - 5 * s), 26 * s, 24 * s);

    // 腮红
    p.setBrush(QColor(255, 200, 200, 100));
    p.drawEllipse(QPointF(cx - 14 * s, cy), 7 * s, 5 * s);
    p.drawEllipse(QPointF(cx + 14 * s, cy), 7 * s, 5 * s);

    // 眼睛
    auto drawBunnyEye = [&](qreal ex, qreal ey) {
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex, ey), 7 * s, 8 * s);
        p.setBrush(QColor("#3D0000"));
        p.drawEllipse(QPointF(ex + 1 * s, ey), 4 * s, 5 * s);
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(ex - 1.5 * s, ey - 2.5 * s), 2 * s, 2 * s);
    };
    drawBunnyEye(cx - 9 * s, cy - 8 * s);
    drawBunnyEye(cx + 9 * s, cy - 8 * s);

    // 鼻子 — 粉色三角
    p.setBrush(QColor("#FFB0C0"));
    QPolygonF nose;
    nose << QPointF(cx - 2.5 * s, cy)
         << QPointF(cx + 2.5 * s, cy)
         << QPointF(cx, cy + 3 * s);
    p.drawPolygon(nose);

    // 嘴巴
    p.setPen(QPen(QColor("#999"), 1 * s));
    p.setBrush(Qt::NoBrush);
    p.drawLine(QPointF(cx, cy + 3 * s), QPointF(cx, cy + 7 * s));
    p.drawLine(QPointF(cx - 3 * s, cy + 5 * s), QPointF(cx, cy + 7 * s));
    p.drawLine(QPointF(cx + 3 * s, cy + 5 * s), QPointF(cx, cy + 7 * s));

    // 胡须
    p.setPen(QPen(QColor("#CCC"), 0.6 * s));
    for (int side = -1; side <= 1; side += 2) {
        qreal bx = cx + side * 12 * s;
        p.drawLine(QPointF(bx, cy), QPointF(bx + side * 12 * s, cy - 3 * s));
        p.drawLine(QPointF(bx, cy + 2 * s), QPointF(bx + side * 12 * s, cy + 1 * s));
    }

    // 前爪
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#F0F0F0"));
    p.drawRoundedRect(QRectF(cx - 14 * s, cy + 38 * s, 10 * s, 8 * s), 4 * s, 4 * s);
    p.drawRoundedRect(QRectF(cx + 4 * s, cy + 38 * s, 10 * s, 8 * s), 4 * s, 4 * s);

    // 蓬松尾巴
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(QPointF(cx - 3 * s, cy + 44 * s), 8 * s, 7 * s);
}
