#include "shopdialog.h"
#include "petmanager.h"
#include "item.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QMessageBox>
#include <QGroupBox>

ShopDialog::ShopDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("🛒 宠物商城"));
    setMinimumSize(420, 360);
    resize(560, 520);
    setStyleSheet(
        "QDialog { background: #FFF8F0; }"
        "QGroupBox { font-weight: bold; border: 1px solid #FFD8B0; "
        "  border-radius: 8px; margin-top: 8px; padding-top: 14px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #E87A36; }"
    );

    // 预设商品 (堆上分配，dialog 结束时清理)
    m_shopItems.append(new Food(QString::fromUtf8("小鱼干"),    QString::fromUtf8("猫咪最爱 +25饱食"), 15, 25));
    m_shopItems.append(new Food(QString::fromUtf8("肉骨头"),    QString::fromUtf8("狗狗狂喜 +30饱食"), 20, 30));
    m_shopItems.append(new Food(QString::fromUtf8("胡萝卜"),    QString::fromUtf8("兔兔零食 +20饱食"), 12, 20));
    m_shopItems.append(new Food(QString::fromUtf8("宠物粮"),    QString::fromUtf8("通用主食 +15饱食"), 10, 15));
    m_shopItems.append(new Food(QString::fromUtf8("肉罐头"),    QString::fromUtf8("高能美味 +35饱食"), 28, 35));
    m_shopItems.append(new Food(QString::fromUtf8("小蛋糕"),    QString::fromUtf8("甜品零食 +10饱食 +5心情"), 18, 10));
    m_shopItems.append(new Toy(QString::fromUtf8("毛线球"),     QString::fromUtf8("猫の玩具 +20心情"), 15, 20));
    m_shopItems.append(new Toy(QString::fromUtf8("飞盘"),       QString::fromUtf8("户外运动 +25心情"), 20, 25));
    m_shopItems.append(new Toy(QString::fromUtf8("磨牙棒"),     QString::fromUtf8("兔兔最爱 +20心情"), 12, 20));
    m_shopItems.append(new Toy(QString::fromUtf8("铃铛球"),     QString::fromUtf8("叮叮当当 +15心情"), 10, 15));
    m_shopItems.append(new Medicine(QString::fromUtf8("药膏"),  QString::fromUtf8("通用治疗 +30健康"), 25, 30));
    m_shopItems.append(new Medicine(QString::fromUtf8("宠物疫苗"), QString::fromUtf8("强效防护 +50健康"), 40, 50));

    setupUI();
}

ShopDialog::~ShopDialog()
{
    qDeleteAll(m_shopItems);
}

void ShopDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 金币显示
    m_coinLabel = new QLabel();
    m_coinLabel->setAlignment(Qt::AlignCenter);
    m_coinLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #E8A030; padding: 4px;");
    mainLayout->addWidget(m_coinLabel);

    // ====== 商城区域 ======
    QGroupBox *shopGroup = new QGroupBox(QString::fromUtf8("🏪 商城"));
    QScrollArea *shopScroll = new QScrollArea();
    shopScroll->setWidgetResizable(true);
    shopScroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    m_shopGrid = new QWidget();
    QGridLayout *shopLayout = new QGridLayout(m_shopGrid);
    shopLayout->setSpacing(8);

    for (int i = 0; i < m_shopItems.size(); i++) {
        Item *item = m_shopItems[i];
        QFrame *card = new QFrame();
        card->setStyleSheet(
            "QFrame { background: white; border: 1px solid #F0E0D0; "
            "border-radius: 10px; padding: 8px; }"
            "QFrame:hover { border-color: #FFB347; background: #FFF5EB; }"
        );
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(4);

        QLabel *icon = new QLabel(item->icon());
        icon->setAlignment(Qt::AlignCenter);
        icon->setStyleSheet("font-size: 28px; border: none;");
        cardLayout->addWidget(icon);

        QLabel *name = new QLabel(item->name());
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("font-weight: bold; font-size: 11px; border: none;");
        cardLayout->addWidget(name);

        QLabel *desc = new QLabel(item->description());
        desc->setAlignment(Qt::AlignCenter);
        desc->setWordWrap(true);
        desc->setStyleSheet("font-size: 9px; color: #888; border: none;");
        cardLayout->addWidget(desc);

        QPushButton *btn = new QPushButton(QString::fromUtf8("💰 %1").arg(item->price()));
        btn->setStyleSheet(
            "QPushButton { background: #FFB347; color: white; border: none; "
            "border-radius: 6px; padding: 4px 10px; font-weight: bold; }"
            "QPushButton:hover { background: #FF9A1F; }"
        );
        connect(btn, &QPushButton::clicked, this, [this, i]() { buyItem(i); });
        cardLayout->addWidget(btn);

        shopLayout->addWidget(card, i / 3, i % 3);
    }
    shopScroll->setWidget(m_shopGrid);
    QVBoxLayout *shopGroupLayout = new QVBoxLayout(shopGroup);
    shopGroupLayout->addWidget(shopScroll);
    mainLayout->addWidget(shopGroup);

    // ====== 背包区域 ======
    QGroupBox *invGroup = new QGroupBox(QString::fromUtf8("🎒 我的背包"));
    QScrollArea *invScroll = new QScrollArea();
    invScroll->setWidgetResizable(true);
    invScroll->setMaximumHeight(120);
    invScroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    m_inventoryGrid = new QWidget();
    QHBoxLayout *invLayout = new QHBoxLayout(m_inventoryGrid);
    invLayout->setSpacing(6);

    // 背包内容由 refreshUI 更新
    invScroll->setWidget(m_inventoryGrid);
    QVBoxLayout *invGroupLayout = new QVBoxLayout(invGroup);
    invGroupLayout->addWidget(invScroll);
    mainLayout->addWidget(invGroup);

    refreshUI();
}

void ShopDialog::refreshUI()
{
    PetManager *pm = PetManager::instance();
    m_coinLabel->setText(QString::fromUtf8("🪙 当前金币：%1").arg(pm->coins()));

    // 清空背包网格
    QHBoxLayout *invLayout = qobject_cast<QHBoxLayout *>(m_inventoryGrid->layout());
    if (!invLayout) return;
    QLayoutItem *child;
    while ((child = invLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    // 重建背包卡片
    const QVector<Item *> &inv = pm->inventory();
    if (inv.isEmpty()) {
        QLabel *empty = new QLabel(QString::fromUtf8("背包空空如也～去商城买点东西吧！"));
        empty->setStyleSheet("color: #AAA; font-size: 11px; padding: 8px;");
        empty->setAlignment(Qt::AlignCenter);
        invLayout->addWidget(empty);
    } else {
        for (int i = 0; i < inv.size(); i++) {
            Item *item = inv[i];
            QFrame *card = new QFrame();
            card->setStyleSheet(
                "QFrame { background: white; border: 1px solid #E0E0E0; "
                "border-radius: 8px; padding: 4px; }"
            );
            QVBoxLayout *cl = new QVBoxLayout(card);
            cl->setSpacing(2);

            QLabel *icon = new QLabel(item->icon());
            icon->setAlignment(Qt::AlignCenter);
            icon->setStyleSheet("font-size: 22px; border: none;");
            cl->addWidget(icon);

            QPushButton *useBtn = new QPushButton(QString::fromUtf8("使用"));
            useBtn->setStyleSheet(
                "QPushButton { background: #6BCB77; color: white; border: none; "
                "border-radius: 4px; padding: 2px 8px; font-size: 10px; }"
                "QPushButton:hover { background: #5BB868; }"
            );
            connect(useBtn, &QPushButton::clicked, this, [this, i]() { useInventoryItem(i); });
            cl->addWidget(useBtn);

            invLayout->addWidget(card);
        }
        invLayout->addStretch();
    }
}

void ShopDialog::buyItem(int index)
{
    if (index < 0 || index >= m_shopItems.size()) return;

    PetManager *pm = PetManager::instance();
    Item *templateItem = m_shopItems[index];
    int price = templateItem->price();

    if (!pm->spendCoins(price)) {
        QMessageBox::warning(this, QString::fromUtf8("金币不足"),
                             QString::fromUtf8("金币不够啦！去打打工赚点钱吧～ 💼"));
        return;
    }

    // 根据类型 clone 一个道具加入背包
    Item *newItem = nullptr;
    if (templateItem->category() == QString::fromUtf8("食物"))
        newItem = new Food(templateItem->name(), templateItem->description(), price, templateItem->effect());
    else if (templateItem->category() == QString::fromUtf8("玩具"))
        newItem = new Toy(templateItem->name(), templateItem->description(), price, templateItem->effect());
    else
        newItem = new Medicine(templateItem->name(), templateItem->description(), price, templateItem->effect());

    pm->addItem(newItem);
    refreshUI();
}

void ShopDialog::useInventoryItem(int index)
{
    PetManager::instance()->useItem(index);
    refreshUI();
}
