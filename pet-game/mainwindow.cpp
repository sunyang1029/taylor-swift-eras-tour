#include "mainwindow.h"
#include "petmanager.h"
#include "shopdialog.h"
#include "jobdialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QScreen>
#include <QApplication>
#include <QGuiApplication>
#include <QLinearGradient>
#include <QRandomGenerator>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFixedSize(220, 300);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeo = screen->availableGeometry();
    move(screenGeo.right() - 240, screenGeo.bottom() - 320);

    m_chatBubble = new QLabel(this);
    m_chatBubble->setStyleSheet(
        "QLabel {"
        "  background: white;"
        "  border: 2px solid #FFB347;"
        "  border-radius: 12px;"
        "  padding: 8px 14px;"
        "  font-size: 13px;"
        "  color: #333;"
        "}");
    m_chatBubble->setWordWrap(true);
    m_chatBubble->setMaximumWidth(220);
    m_chatBubble->hide();

    m_bubbleTimer = new QTimer(this);
    m_bubbleTimer->setSingleShot(true);
    connect(m_bubbleTimer, &QTimer::timeout, this, &MainWindow::hideChatBubble);

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, [this]() {
        m_animFrame++;
        // 动作帧递减
        if (m_actionFrame > 0) {
            m_actionFrame--;
            if (m_actionFrame == 0)
                m_actionState = ActionNone;
        }
        update();
    });
    m_animTimer->start(50);

    // 粒子更新定时器 (30fps)
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, &MainWindow::updateParticles);
    m_particleTimer->start(33);

    PetManager *pm = PetManager::instance();
    connect(pm, &PetManager::statsChanged, this, &MainWindow::onStatsChanged);
    connect(pm, &PetManager::coinsChanged, this, &MainWindow::onCoinsChanged);
    connect(pm, &PetManager::petChanged, this, &MainWindow::onPetChanged);

    pm->startTick();
    updateTooltip();
}

MainWindow::~MainWindow()
{
    PetManager::instance()->pauseTick();
}

// ==================== 粒子系统 ====================

void MainWindow::spawnParticles(const QStringList &emojis, int count)
{
    auto *rng = QRandomGenerator::global();
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = QPointF(90 + rng->bounded(40), 100 + rng->bounded(20));
        p.vel = QPointF(rng->bounded(30) - 15, -(rng->bounded(40) + 20));
        p.emoji = emojis[rng->bounded(emojis.size())];
        p.lifeMs = 1200 + rng->bounded(800);
        p.elapsed = 0;
        m_particles.append(p);
    }
}

void MainWindow::updateParticles()
{
    for (int i = m_particles.size() - 1; i >= 0; i--) {
        Particle &p = m_particles[i];
        p.elapsed += 33;
        p.pos += p.vel * 0.033;
        p.vel += QPointF(0, 15 * 0.033); // 轻微重力
        if (p.elapsed >= p.lifeMs)
            m_particles.removeAt(i);
    }
    if (!m_particles.isEmpty()) update();
}

void MainWindow::drawParticles(QPainter &p)
{
    p.setFont(QFont("Segoe UI Emoji", 16));
    for (const auto &pt : m_particles) {
        qreal alpha = 1.0 - (qreal)pt.elapsed / pt.lifeMs;
        p.setPen(QColor(0, 0, 0, (int)(alpha * 255)));
        p.drawText(QRectF(pt.pos.x() - 10, pt.pos.y() - 10, 40, 40), Qt::AlignCenter, pt.emoji);
    }
}

void MainWindow::triggerAction(ActionState state)
{
    m_actionState = state;
    m_actionFrame = 15; // ~750ms 动画时长

    switch (state) {
    case ActionFeeding:
        spawnParticles({QString::fromUtf8("🍖"), QString::fromUtf8("🍗"),
                        QString::fromUtf8("🐟"), QString::fromUtf8("🥕"),
                        QString::fromUtf8("❤️"), QString::fromUtf8("✨")}, 8);
        break;
    case ActionPlaying:
        spawnParticles({QString::fromUtf8("💕"), QString::fromUtf8("💖"),
                        QString::fromUtf8("⭐"), QString::fromUtf8("✨"),
                        QString::fromUtf8("🎾"), QString::fromUtf8("🎉")}, 10);
        break;
    case ActionHealing:
        spawnParticles({QString::fromUtf8("💊"), QString::fromUtf8("💚"),
                        QString::fromUtf8("✨"), QString::fromUtf8("🌟"),
                        QString::fromUtf8("🍀"), QString::fromUtf8("💪")}, 8);
        break;
    default:
        break;
    }
}

// ==================== 绘制 ====================

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 背景
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, m_hovered ? 60 : 10));
    p.drawRoundedRect(rect().adjusted(5, 5, -5, -5), 20, 20);

    Pet *pet = PetManager::instance()->currentPet();
    if (!pet) return;

    // 动作动画偏移 — 弹跳/摇晃效果
    qreal actionOffsetY = 0;
    qreal actionOffsetX = 0;
    if (m_actionState != ActionNone) {
        qreal t = m_actionFrame / 15.0;
        switch (m_actionState) {
        case ActionFeeding:
            // 上下弹跳
            actionOffsetY = -qAbs(qSin(t * M_PI * 3)) * 12;
            break;
        case ActionPlaying:
            // 左右摇摆
            actionOffsetX = qSin(t * M_PI * 4) * 8;
            break;
        case ActionHealing:
            // 旋转式弹跳
            actionOffsetY = -qAbs(qSin(t * M_PI * 2.5)) * 6;
            actionOffsetX = qSin(t * M_PI * 3) * 4;
            break;
        default: break;
        }
    }

    // 宠物绘制区域
    QRectF petBounds(10 + actionOffsetX, 15 + actionOffsetY, 200, 200);
    pet->draw(p, petBounds, m_animFrame);

    // 粒子
    drawParticles(p);

    // 状态栏 + 金币
    drawStatusBars(p);
    drawCoins(p);

    // 头顶表情
    if (m_actionState != ActionNone) {
        // 交互动画中 — 显示对应 emoji
        p.setFont(QFont("Segoe UI Emoji", 18));
        QString actionEmoji;
        switch (m_actionState) {
        case ActionFeeding: actionEmoji = QString::fromUtf8("😋"); break;
        case ActionPlaying: actionEmoji = QString::fromUtf8("😆"); break;
        case ActionHealing: actionEmoji = QString::fromUtf8("😊"); break;
        default: break;
        }
        p.drawText(QRectF(90, 2, 40, 35), Qt::AlignCenter, actionEmoji);
    } else if (pet->isHungry() || pet->isSick()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(100, 100, 100, 80));
        p.drawEllipse(QPointF(110, 20), 14, 10);
        p.drawEllipse(QPointF(100, 16), 10, 7);
        p.setPen(QPen(Qt::white, 2));
        p.setFont(QFont("Segoe UI Emoji", 14));
        if (pet->isSick())
            p.drawText(QRectF(90, 5, 40, 30), Qt::AlignCenter, QString::fromUtf8("🤒"));
        else if (pet->isHungry())
            p.drawText(QRectF(90, 5, 40, 30), Qt::AlignCenter, QString::fromUtf8("😿"));
    } else if (pet->isHappy()) {
        p.setPen(QPen(Qt::white, 2));
        p.setFont(QFont("Segoe UI Emoji", 14));
        p.drawText(QRectF(90, 2, 40, 30), Qt::AlignCenter, QString::fromUtf8("💕"));
    }
}

void MainWindow::drawStatusBars(QPainter &p)
{
    Pet *pet = PetManager::instance()->currentPet();
    if (!pet) return;

    struct Bar { QString label; int value; QColor color; };
    Bar bars[] = {
        {QString::fromUtf8("饱食"), pet->hunger(), QColor("#FF6B6B")},
        {QString::fromUtf8("心情"), pet->mood(),   QColor("#FFD93D")},
        {QString::fromUtf8("健康"), pet->health(), QColor("#6BCB77")},
        {QString::fromUtf8("精力"), pet->energy(), QColor("#4D96FF")},
    };

    qreal y = 228;
    for (const auto &bar : bars) {
        p.setPen(QColor("#555"));
        p.setFont(QFont("Microsoft YaHei", 8));
        p.drawText(QRectF(20, y, 35, 12), Qt::AlignLeft | Qt::AlignVCenter, bar.label);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(220, 220, 220));
        p.drawRoundedRect(QRectF(54, y + 2, 100, 8), 4, 4);

        QLinearGradient grad(54, 0, 154, 0);
        grad.setColorAt(0, bar.color.lighter(130));
        grad.setColorAt(1, bar.color);
        p.setBrush(grad);
        p.drawRoundedRect(QRectF(54, y + 2, bar.value, 8), 4, 4);

        p.setPen(QColor("#888"));
        p.setFont(QFont("Microsoft YaHei", 7));
        p.drawText(QRectF(158, y, 40, 12), Qt::AlignRight | Qt::AlignVCenter,
                   QString("%1%").arg(bar.value));

        y += 15;
    }
}

void MainWindow::drawCoins(QPainter &p)
{
    PetManager *pm = PetManager::instance();
    p.setPen(QColor("#E8A030"));
    p.setFont(QFont("Segoe UI Emoji", 10));
    p.drawText(QRectF(20, 288, 180, 12), Qt::AlignRight | Qt::AlignVCenter,
               QString::fromUtf8("🪙 %1 金币").arg(pm->coins()));
}

// ==================== 拖拽 ====================

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPos() - frameGeometry().topLeft();
        m_dragging = true;
        showChatBubble();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint dist = (event->globalPos() - frameGeometry().topLeft() - m_dragPos);
        if (dist.manhattanLength() > 5)
            move(event->globalPos() - m_dragPos);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

// ==================== 右键菜单 ====================

void MainWindow::contextMenuEvent(QContextMenuEvent *)
{
    QMenu menu;

    QMenu *interactMenu = menu.addMenu(QString::fromUtf8("🍖 互动"));
    interactMenu->addAction(QString::fromUtf8("喂食"), this, &MainWindow::feedPet);
    interactMenu->addAction(QString::fromUtf8("玩耍"), this, &MainWindow::playPet);
    interactMenu->addAction(QString::fromUtf8("治疗"), this, &MainWindow::healPet);

    menu.addAction(QString::fromUtf8("🛒 商城"), this, &MainWindow::showShop);
    menu.addAction(QString::fromUtf8("💼 打工"), this, &MainWindow::showJobs);

    QMenu *switchMenu = menu.addMenu(QString::fromUtf8("🔄 切换宠物"));
    switchMenu->addAction(QString::fromUtf8("🐱 小橘 (猫咪)"), this, &MainWindow::switchToCat);
    switchMenu->addAction(QString::fromUtf8("🐶 旺财 (狗狗)"), this, &MainWindow::switchToDog);
    switchMenu->addAction(QString::fromUtf8("🐰 团子 (兔兔)"), this, &MainWindow::switchToBunny);

    menu.addSeparator();
    menu.addAction(QString::fromUtf8("❌ 退出"), qApp, &QApplication::quit);

    menu.setStyleSheet(
        "QMenu { background: white; border: 1px solid #DDD; border-radius: 8px; padding: 4px; }"
        "QMenu::item { padding: 6px 24px; border-radius: 4px; }"
        "QMenu::item:selected { background: #FFE0B2; }"
    );

    menu.exec(QCursor::pos());
}

// ==================== 悬停 ====================

void MainWindow::enterEvent(QEvent *)
{
    m_hovered = true;
    update();
}

void MainWindow::leaveEvent(QEvent *)
{
    m_hovered = false;
    update();
}

// ==================== 聊天泡泡 ====================

void MainWindow::showChatBubble(const QString &text)
{
    Pet *pet = PetManager::instance()->currentPet();
    if (!pet) return;

    QString msg = text.isEmpty() ? pet->speak() : text;
    m_chatBubble->setText(msg);

    QPoint globalPos = mapToGlobal(QPoint(10, -m_chatBubble->sizeHint().height() - 15));
    m_chatBubble->move(globalPos);
    m_chatBubble->adjustSize();
    m_chatBubble->show();
    m_chatBubble->raise();

    m_bubbleTimer->start(3500);
}

void MainWindow::hideChatBubble()
{
    m_chatBubble->hide();
}

// ==================== 交互槽函数 ====================

void MainWindow::feedPet()
{
    PetManager *pm = PetManager::instance();
    bool usedItem = false;
    QString itemName;
    for (int i = 0; i < pm->inventory().size(); i++) {
        Item *item = pm->inventory()[i];
        if (item->category() == QString::fromUtf8("食物")) {
            itemName = item->name();
            pm->useItem(i);
            usedItem = true;
            break;
        }
    }
    if (!usedItem) {
        pm->currentPet()->feed(10);
        pm->currentPet()->tick();
        emit pm->statsChanged();
        itemName = QString::fromUtf8("小零食");
    }

    triggerAction(ActionFeeding);
    QString msg = usedItem
        ? QString::fromUtf8("嗷呜～%1好好吃！谢谢主人！😋").arg(itemName)
        : QString::fromUtf8("好吃好吃～虽然只是小零食但也很满足！");
    showChatBubble(msg);
}

void MainWindow::playPet()
{
    PetManager *pm = PetManager::instance();
    bool usedItem = false;
    QString itemName;
    for (int i = 0; i < pm->inventory().size(); i++) {
        Item *item = pm->inventory()[i];
        if (item->category() == QString::fromUtf8("玩具")) {
            itemName = item->name();
            pm->useItem(i);
            usedItem = true;
            break;
        }
    }
    if (!usedItem) {
        pm->currentPet()->play(10);
        pm->currentPet()->tick();
        emit pm->statsChanged();
    }

    triggerAction(ActionPlaying);
    QStringList cheers = {
        QString::fromUtf8("哈哈！好好玩！再来一次！"),
        QString::fromUtf8("耶～和主人一起玩最开心了！🎉"),
        QString::fromUtf8("蹦蹦跳跳！今天运动量达标啦！"),
        QString::fromUtf8("开心开心！转圈圈～"),
    };
    auto *rng = QRandomGenerator::global();
    QString msg = usedItem
        ? QString::fromUtf8("哇！%1好好玩！主人陪我一起！").arg(itemName)
        : cheers[rng->bounded(cheers.size())];
    showChatBubble(msg);
}

void MainWindow::healPet()
{
    PetManager *pm = PetManager::instance();
    bool usedItem = false;
    for (int i = 0; i < pm->inventory().size(); i++) {
        Item *item = pm->inventory()[i];
        if (item->category() == QString::fromUtf8("药品")) {
            pm->useItem(i);
            usedItem = true;
            break;
        }
    }
    if (!usedItem) {
        pm->currentPet()->heal(10);
        pm->currentPet()->tick();
        emit pm->statsChanged();
    }

    triggerAction(ActionHealing);
    showChatBubble(QString::fromUtf8("呼～感觉好多了！谢谢主人照顾我！💚"));
}

void MainWindow::showShop()
{
    ShopDialog dlg;
    dlg.exec();
}

void MainWindow::showJobs()
{
    JobDialog dlg;
    dlg.exec();
}

void MainWindow::switchToCat()    { PetManager::instance()->switchPet(0); }
void MainWindow::switchToDog()    { PetManager::instance()->switchPet(1); }
void MainWindow::switchToBunny()  { PetManager::instance()->switchPet(2); }

void MainWindow::onStatsChanged()  { update(); }
void MainWindow::onCoinsChanged(int) { update(); }
void MainWindow::onPetChanged()
{
    m_animFrame = 0;
    update();
    updateTooltip();
}

void MainWindow::updateTooltip()
{
    Pet *pet = PetManager::instance()->currentPet();
    if (!pet) return;
    setToolTip(QString::fromUtf8("%1 · %2\n拖拽移动 | 左键聊天 | 右键菜单")
               .arg(pet->name(), pet->species()));
}
