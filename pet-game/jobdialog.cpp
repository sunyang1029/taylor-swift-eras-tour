#include "jobdialog.h"
#include "petmanager.h"
#include "job.h"
#include "pet.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QGroupBox>

JobDialog::JobDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("💼 宠物打工"));
    setMinimumSize(360, 300);
    resize(480, 400);
    setStyleSheet(
        "QDialog { background: #FFF8F0; }"
        "QGroupBox { font-weight: bold; border: 1px solid #FFD8B0; "
        "  border-radius: 8px; margin-top: 8px; padding-top: 14px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #E87A36; }"
    );

    // 工种列表
    m_jobs.append(new WaiterJob());
    m_jobs.append(new DeliveryJob());
    m_jobs.append(new GuardJob());

    m_jobTimer = new QTimer(this);
    m_jobTimer->setInterval(1000); // 每秒更新
    connect(m_jobTimer, &QTimer::timeout, this, [this]() {
        m_elapsed++;
        if (m_currentJob) {
            m_progressBar->setValue(m_elapsed);
            if (m_elapsed >= m_currentJob->durationSec()) {
                onJobFinished();
            }
        }
    });

    setupUI();
}

JobDialog::~JobDialog()
{
    qDeleteAll(m_jobs);
}

void JobDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 金币显示
    m_coinLabel = new QLabel();
    m_coinLabel->setAlignment(Qt::AlignCenter);
    m_coinLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #E8A030;");
    mainLayout->addWidget(m_coinLabel);

    // 状态显示
    m_statusLabel = new QLabel(QString::fromUtf8("选择一个工作，让宠物去打工赚钱吧～"));
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size: 12px; color: #888; padding: 4px;");
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    // 进度条
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 10);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid #DDD; border-radius: 6px; background: #F0F0F0; height: 16px; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, "
        "  stop:0 #FFB347, stop:1 #FF6B6B); border-radius: 5px; }"
    );
    mainLayout->addWidget(m_progressBar);

    // 工作列表
    QGroupBox *jobGroup = new QGroupBox(QString::fromUtf8("📋 可选工作"));
    QGridLayout *jobLayout = new QGridLayout();
    jobLayout->setSpacing(10);

    for (int i = 0; i < m_jobs.size(); i++) {
        Job *job = m_jobs[i];

        QFrame *card = new QFrame();
        card->setStyleSheet(
            "QFrame { background: white; border: 1px solid #F0E0D0; "
            "border-radius: 10px; padding: 12px; }"
            "QFrame:hover { border-color: #FFB347; }"
        );
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(4);

        // 标题行
        QHBoxLayout *titleRow = new QHBoxLayout();
        QLabel *emoji = new QLabel(job->emoji());
        emoji->setStyleSheet("font-size: 24px; border: none;");
        titleRow->addWidget(emoji);

        QLabel *name = new QLabel(job->name());
        name->setStyleSheet("font-weight: bold; font-size: 13px; border: none;");
        titleRow->addWidget(name);
        titleRow->addStretch();
        cardLayout->addLayout(titleRow);

        // 描述
        QLabel *desc = new QLabel(job->description());
        desc->setStyleSheet("color: #777; font-size: 10px; border: none;");
        desc->setWordWrap(true);
        cardLayout->addWidget(desc);

        // 参数
        QLabel *params = new QLabel(
            QString::fromUtf8("⏱ %1秒 | 💰 +%2金币 | ⚡ -%3精力 | ⚠ 风险Lv.%4")
                .arg(job->durationSec()).arg(job->reward())
                .arg(job->energyCost()).arg(job->risk()));
        params->setStyleSheet("font-size: 10px; color: #555; border: none;");
        cardLayout->addWidget(params);

        // 按钮
        QPushButton *btn = new QPushButton(QString::fromUtf8("🚀 开始打工"));
        btn->setStyleSheet(
            "QPushButton { background: #FF6B6B; color: white; border: none; "
            "border-radius: 6px; padding: 6px 16px; font-weight: bold; }"
            "QPushButton:hover { background: #FF5252; }"
            "QPushButton:disabled { background: #CCC; }"
        );
        btn->setEnabled(!m_working);
        connect(btn, &QPushButton::clicked, this, [this, i]() { startJob(i); });
        cardLayout->addWidget(btn);

        jobLayout->addWidget(card, 0, i);
    }
    jobGroup->setLayout(jobLayout);
    mainLayout->addWidget(jobGroup);

    m_coinLabel->setText(QString::fromUtf8("🪙 当前金币：%1").arg(PetManager::instance()->coins()));
}

void JobDialog::startJob(int index)
{
    if (index < 0 || index >= m_jobs.size() || m_working) return;

    PetManager *pm = PetManager::instance();
    Pet *pet = pm->currentPet();

    // 检查精力
    Job *job = m_jobs[index];
    if (pet->energy() < job->energyCost()) {
        QMessageBox::warning(this, QString::fromUtf8("精力不足"),
                             QString::fromUtf8("宠物太累了，让它休息一会儿再来工作吧～ 😴"));
        return;
    }

    m_currentJob = job;
    m_working = true;
    m_elapsed = 0;

    m_progressBar->setRange(0, job->durationSec());
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);

    m_statusLabel->setText(QString::fromUtf8("工作中... %1").arg(job->name()));

    // 禁用所有工作按钮
    QList<QPushButton *> buttons = findChildren<QPushButton *>();
    for (auto *btn : buttons) {
        if (btn->text().contains(QString::fromUtf8("开始打工")))
            btn->setEnabled(false);
    }

    m_jobTimer->start();
}

void JobDialog::onJobFinished()
{
    m_jobTimer->stop();
    m_working = false;

    PetManager *pm = PetManager::instance();
    int earned = 0;
    QString result = m_currentJob->execute(pm->currentPet(), earned);
    pm->addCoins(earned);

    m_statusLabel->setText(result);
    m_progressBar->setVisible(false);
    m_coinLabel->setText(QString::fromUtf8("🪙 当前金币：%1").arg(pm->coins()));

    emit pm->statsChanged();

    // 恢复按钮
    QList<QPushButton *> buttons = findChildren<QPushButton *>();
    for (auto *btn : buttons) {
        if (btn->text().contains(QString::fromUtf8("开始打工")))
            btn->setEnabled(true);
    }
}
