#ifndef JOBDIALOG_H
#define JOBDIALOG_H

#include <QDialog>
#include <QVector>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

class Job;

class JobDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JobDialog(QWidget *parent = nullptr);
    ~JobDialog();

private slots:
    void startJob(int index);
    void onJobFinished();

private:
    void setupUI();
    QVector<Job *> m_jobs;
    QLabel *m_coinLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QTimer *m_jobTimer = nullptr;
    Job *m_currentJob = nullptr;
    int m_elapsed = 0;
    bool m_working = false;
};

#endif // JOBDIALOG_H
