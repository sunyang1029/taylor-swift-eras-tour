#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QLabel>
#include <QVector>
#include "pet.h"

struct Particle
{
    QPointF pos;
    QPointF vel;
    QString emoji;
    int lifeMs;
    int elapsed = 0;
};

enum ActionState { ActionNone, ActionFeeding, ActionPlaying, ActionHealing };

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onStatsChanged();
    void onCoinsChanged(int coins);
    void onPetChanged();
    void showShop();
    void showJobs();
    void feedPet();
    void playPet();
    void healPet();
    void switchToCat();
    void switchToDog();
    void switchToBunny();
    void updateParticles();

private:
    void updateTooltip();
    void showChatBubble(const QString &text = QString());
    void hideChatBubble();
    void drawStatusBars(QPainter &p);
    void drawCoins(QPainter &p);
    void drawParticles(QPainter &p);
    void spawnParticles(const QStringList &emojis, int count);
    void triggerAction(ActionState state);

    QPoint m_dragPos;
    bool m_dragging = false;
    QLabel *m_chatBubble = nullptr;
    QTimer *m_bubbleTimer = nullptr;
    QTimer *m_animTimer = nullptr;
    QTimer *m_particleTimer = nullptr;
    int m_animFrame = 0;
    bool m_hovered = false;

    // 粒子系统
    QVector<Particle> m_particles;

    // 动作动画状态
    ActionState m_actionState = ActionNone;
    int m_actionFrame = 0;
};

#endif // MAINWINDOW_H
