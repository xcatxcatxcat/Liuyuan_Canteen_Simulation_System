#include "SimulationParamsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QTimeEdit>
#include <QComboBox>
#include <QScrollArea>

SimulationParamsPage::SimulationParamsPage(Core::ConfigManager *configMgr, QWidget *parent)
    : QWidget(parent)
    , m_configMgr(configMgr)
{
    setupUi();
    loadFromConfig();
}

void SimulationParamsPage::setupUi()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget;
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== 时间设置 =====
    auto *timeGroup = new QGroupBox(QStringLiteral("仿真时间设置"));
    auto *timeForm = new QFormLayout(timeGroup);

    m_startTimeEdit = new QTimeEdit;
    m_startTimeEdit->setDisplayFormat(QStringLiteral("HH:mm:ss"));
    m_startTimeEdit->setToolTip(QStringLiteral("仿真模拟的开始时间"));
    timeForm->addRow(QStringLiteral("仿真开始时间："), m_startTimeEdit);

    m_durationSpin = new QSpinBox;
    m_durationSpin->setRange(60, 28800);
    m_durationSpin->setValue(7200);
    m_durationSpin->setSingleStep(300);
    m_durationSpin->setSuffix(QStringLiteral(" 秒"));
    m_durationSpin->setToolTip(QStringLiteral("仿真模拟的总时长"));
    timeForm->addRow(QStringLiteral("仿真总时长："), m_durationSpin);

    m_endTimeLabel = new QLabel;
    m_endTimeLabel->setStyleSheet(QStringLiteral("font-weight: bold; color: #1a237e;"));
    timeForm->addRow(QStringLiteral("仿真结束时间："), m_endTimeLabel);
    connect(m_durationSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SimulationParamsPage::onDurationChanged);

    m_timeStepSpin = new QSpinBox;
    m_timeStepSpin->setRange(1, 600);
    m_timeStepSpin->setValue(60);
    m_timeStepSpin->setSuffix(QStringLiteral(" 秒"));
    m_timeStepSpin->setToolTip(QStringLiteral("仿真时钟每次推进的步长"));
    timeForm->addRow(QStringLiteral("时间步长："), m_timeStepSpin);

    m_ratioLabel = new QLabel;
    m_ratioLabel->setStyleSheet(QStringLiteral("color: #78909c; font-size: 12px;"));
    timeForm->addRow(m_ratioLabel);

    mainLayout->addWidget(timeGroup);

    // ===== 运行设置 =====
    auto *runGroup = new QGroupBox(QStringLiteral("运行设置"));
    auto *runForm = new QFormLayout(runGroup);

    m_runModeCombo = new QComboBox;
    m_runModeCombo->addItem(QStringLiteral("可视化模式"), static_cast<int>(Models::RunMode::Visual));
    m_runModeCombo->addItem(QStringLiteral("无头模式（纯计算）"), static_cast<int>(Models::RunMode::Headless));
    m_runModeCombo->setToolTip(QStringLiteral("可视化模式会动态展示仿真过程，无头模式仅计算结果"));
    runForm->addRow(QStringLiteral("运行模式："), m_runModeCombo);

    m_logLevelCombo = new QComboBox;
    m_logLevelCombo->addItems(Models::logLevelOptions());
    m_logLevelCombo->setToolTip(QStringLiteral("控制仿真过程中记录的事件详细程度"));
    runForm->addRow(QStringLiteral("日志记录级别："), m_logLevelCombo);

    mainLayout->addWidget(runGroup);

    // ===== 说明 =====
    auto *noteGroup = new QGroupBox(QStringLiteral("参数说明"));
    auto *noteLayout = new QVBoxLayout(noteGroup);

    QStringList notes = {
        QStringLiteral("• 仿真时长：模拟的时间跨度，如 7200 秒 = 2 小时"),
        QStringLiteral("• 时间步长：仿真引擎每次状态更新的间隔，步长越短精度越高但计算量越大"),
        QStringLiteral("• 仿真比例：时间步长对应的真实时间，60 秒步长默认 1 分钟:1 秒的播放速度"),
        QStringLiteral("• 可视化模式：实时渲染食堂平面图、学生移动和排队动态"),
        QStringLiteral("• 无头模式：关闭动画，专注于快速计算统计结果")
    };

    for (const auto &note : notes) {
        auto *lbl = new QLabel(note);
        lbl->setWordWrap(true);
        lbl->setStyleSheet(QStringLiteral("color: #607d8b; font-size: 12px; padding: 2px 0;"));
        noteLayout->addWidget(lbl);
    }

    mainLayout->addWidget(noteGroup);
    mainLayout->addStretch();

    scrollArea->setWidget(container);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);

    // 步长变化时更新比例说明
    connect(m_timeStepSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        int step = m_timeStepSpin->value();
        int minutes = step / 60;
        if (minutes > 0) {
            m_ratioLabel->setText(QString("比例：1 分钟仿真 ≈ %1 秒播放").arg(minutes));
        } else {
            m_ratioLabel->setText(QString("比例：1 秒仿真 ≈ 1 秒播放"));
        }
    });
}

void SimulationParamsPage::loadFromConfig()
{
    m_updatingFromConfig = true;

    auto &cfg = m_configMgr->config().simulation;

    // 解析开始时间
    QStringList parts = cfg.simulationStartTime.split(':');
    if (parts.size() >= 3) {
        m_startTimeEdit->setTime(QTime(parts[0].toInt(), parts[1].toInt(), parts[2].toInt()));
    } else {
        m_startTimeEdit->setTime(QTime(17, 0, 0));
    }

    m_durationSpin->setValue(cfg.simulationDuration);
    m_timeStepSpin->setValue(cfg.timeStep);

    // 运行模式
    for (int i = 0; i < m_runModeCombo->count(); ++i) {
        if (m_runModeCombo->itemData(i).toInt() == static_cast<int>(cfg.runMode)) {
            m_runModeCombo->setCurrentIndex(i);
            break;
        }
    }

    m_logLevelCombo->setCurrentText(Models::logLevelToString(cfg.logLevel));

    // 刷新结束时间和比例
    onDurationChanged(cfg.simulationDuration);

    m_updatingFromConfig = false;
}

void SimulationParamsPage::saveToConfig()
{
    auto &cfg = m_configMgr->config().simulation;

    QTime startTime = m_startTimeEdit->time();
    cfg.simulationStartTime = startTime.toString(QStringLiteral("HH:mm:ss"));
    cfg.simulationDuration  = m_durationSpin->value();
    cfg.timeStep            = m_timeStepSpin->value();

    cfg.runMode  = static_cast<Models::RunMode>(m_runModeCombo->currentData().toInt());
    cfg.logLevel = Models::stringToLogLevel(m_logLevelCombo->currentText());
}

void SimulationParamsPage::onDurationChanged(int seconds)
{
    QTime startTime = m_startTimeEdit->time();
    QTime endTime = startTime.addSecs(seconds);
    m_endTimeLabel->setText(
        QStringLiteral("%1（共 %2 小时 %3 分钟）")
            .arg(endTime.toString(QStringLiteral("HH:mm:ss")))
            .arg(seconds / 3600)
            .arg((seconds % 3600) / 60));
}
