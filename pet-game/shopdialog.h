#ifndef SHOPDIALOG_H
#define SHOPDIALOG_H

#include <QDialog>
#include <QVector>
#include <QLabel>

class Item;

class ShopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShopDialog(QWidget *parent = nullptr);
    ~ShopDialog();

private slots:
    void buyItem(int index);
    void useInventoryItem(int index);

private:
    void setupUI();
    void refreshUI();
    QVector<Item *> m_shopItems;
    QLabel *m_coinLabel = nullptr;
    QWidget *m_shopGrid = nullptr;
    QWidget *m_inventoryGrid = nullptr;
};

#endif // SHOPDIALOG_H
