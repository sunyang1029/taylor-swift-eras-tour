#include "petmanager.h"
#include <QDebug>

PetManager::PetManager(QObject *parent)
    : QObject(parent)
{
    m_pet = new Cat();  // 默认猫咪
    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, [this]() {
        if (m_pet) {
            m_pet->tick();
            emit statsChanged();
        }
    });
}

PetManager::~PetManager()
{
    delete m_pet;
    qDeleteAll(m_inventory);
}

PetManager *PetManager::instance()
{
    static PetManager *inst = new PetManager();
    return inst;
}

void PetManager::switchPet(int type)
{
    delete m_pet;
    switch (type) {
    case 0: m_pet = new Cat();    break;
    case 1: m_pet = new Dog();    break;
    case 2: m_pet = new Bunny();  break;
    default: m_pet = new Cat();   break;
    }
    emit petChanged();
    emit statsChanged();
}

void PetManager::addCoins(int amount)
{
    m_coins += amount;
    emit coinsChanged(m_coins);
}

bool PetManager::spendCoins(int amount)
{
    if (m_coins >= amount) {
        m_coins -= amount;
        emit coinsChanged(m_coins);
        return true;
    }
    return false;
}

void PetManager::addItem(Item *item)
{
    m_inventory.append(item);
    emit inventoryChanged();
}

void PetManager::useItem(int index)
{
    if (index < 0 || index >= m_inventory.size()) return;
    Item *item = m_inventory.takeAt(index);
    item->use(m_pet);
    delete item;
    emit inventoryChanged();
    emit statsChanged();
}

void PetManager::startTick()
{
    m_tickTimer->start(15000); // 每 15 秒衰减一次
}

void PetManager::pauseTick()
{
    m_tickTimer->stop();
}
