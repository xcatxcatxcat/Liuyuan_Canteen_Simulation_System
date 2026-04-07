#include "StudentBehaviorPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QScrollArea>

StudentBehaviorPage::StudentBehaviorPage(Core::ConfigManager *configMgr, QWidget *parent)
    : QWidget(parent)
    , m_configMgr(configMgr)
{
    setupUi();
    loadFromConfig();
}

void StudentBehaviorPage::setupUi()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget;
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== 基础参数 =====
    auto *basicGroup = new QGroupBox(QStringLiteral("学生基础参数"));
    auto *basicForm = new QFormLayout(basicGroup);

    m_totalStudentsSpin = new QSpinBox;
    m_totalStudentsSpin->setRange(1, 50000);
    m_totalStudentsSpin->setSuffix(QStringLiteral(" 人"));
    m_totalStudentsSpin->setSingleStep(100);
    basicForm->addRow(QStringLiteral("预计就餐学生总数："), m_totalStudentsSpin);

    mainLayout->addWidget(basicGroup);

    // ===== 高峰时段人流分布 =====
    auto *distGroup = new QGroupBox(QStringLiteral("高峰时段人流分布"));
    auto *distLayout = new QVBoxLayout(distGroup);

    auto *distHint = new QLabel(
        QStringLiteral("指定仿真时间段内各时间点学生抵达食堂的速率（人/分钟）。"
                        "系统将在相邻时间点之间进行线性插值。"));
    distHint->setWordWrap(true);
    distHint->setStyleSheet(QStringLiteral("color: #78909c; font-size: 12px; padding-bottom: 8px;"));
    distLayout->addWidget(distHint);

    m_distTable = new QTableWidget;
    m_distTable->setColumnCount(2);
    m_distTable->setHorizontalHeaderLabels({
        QStringLiteral("时间"),
        QStringLiteral("抵达速率（人/分钟）")
    });
    m_distTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_distTable->horizontalHeader()->setMinimumSectionSize(200);
    m_distTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_distTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_distTable->setAlternatingRowColors(true);
    m_distTable->verticalHeader()->setVisible(false);
    distLayout->addWidget(m_distTable);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    QPushButton *addBtn = new QPushButton(QStringLiteral("+ 添加时间点"));
    addBtn->setObjectName(QStringLiteral("addBtn"));
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &StudentBehaviorPage::onAddTimePoint);

    QPushButton *removeBtn = new QPushButton(QStringLiteral("- 删除选中"));
    removeBtn->setObjectName(QStringLiteral("removeBtn"));
    removeBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBtn, &QPushButton::clicked, this, &StudentBehaviorPage::onRemoveTimePoint);

    btnRow->addWidget(addBtn);
    btnRow->addWidget(removeBtn);
    distLayout->addLayout(btnRow);

    mainLayout->addWidget(distGroup);

    // ===== 行为偏好 =====
    auto *prefGroup = new QGroupBox(QStringLiteral("学生行为偏好"));
    auto *prefForm = new QFormLayout(prefGroup);

    m_windowPrefCombo = new QComboBox;
    m_windowPrefCombo->addItems(Models::windowPreferenceOptions());
    m_windowPrefCombo->setToolTip(QStringLiteral("学生选择打饭窗口时的决策策略"));
    prefForm->addRow(QStringLiteral("窗口选择策略："), m_windowPrefCombo);

    m_seatPrefCombo = new QComboBox;
    m_seatPrefCombo->addItems(Models::seatPreferenceOptions());
    m_seatPrefCombo->setToolTip(QStringLiteral("学生选择就座位置时的决策策略"));
    prefForm->addRow(QStringLiteral("座位选择策略："), m_seatPrefCombo);

    mainLayout->addWidget(prefGroup);

    // ===== 就餐时长 =====
    auto *mealGroup = new QGroupBox(QStringLiteral("就餐时长分布（正态分布）"));
    auto *mealForm = new QFormLayout(mealGroup);

    m_mealMeanSpin = new QDoubleSpinBox;
    m_mealMeanSpin->setRange(60, 7200);
    m_mealMeanSpin->setValue(1200);
    m_mealMeanSpin->setSuffix(QStringLiteral(" 秒"));
    m_mealMeanSpin->setSingleStep(60);
    m_mealMeanSpin->setToolTip(QStringLiteral("平均就餐时间，默认 20 分钟"));
    mealForm->addRow(QStringLiteral("平均值："), m_mealMeanSpin);

    m_mealStddevSpin = new QDoubleSpinBox;
    m_mealStddevSpin->setRange(10, 3600);
    m_mealStddevSpin->setValue(300);
    m_mealStddevSpin->setSuffix(QStringLiteral(" 秒"));
    m_mealStddevSpin->setSingleStep(30);
    m_mealStddevSpin->setToolTip(QStringLiteral("就餐时间标准差，默认 5 分钟"));
    mealForm->addRow(QStringLiteral("标准差："), m_mealStddevSpin);

    auto *mealHint = new QLabel(QStringLiteral("实际就餐时长 = 均值 + 随机偏移 × 标准差"));
    mealHint->setStyleSheet(QStringLiteral("color: #78909c; font-size: 12px;"));
    mealForm->addRow(mealHint);

    mainLayout->addWidget(mealGroup);

    mainLayout->addStretch();

    scrollArea->setWidget(container);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);

    connect(m_distTable, &QTableWidget::cellChanged,
            this, &StudentBehaviorPage::onDistCellChanged);
}

void StudentBehaviorPage::loadFromConfig()
{
    m_updatingFromConfig = true;

    auto &cfg = m_configMgr->config().student;
    m_totalStudentsSpin->setValue(cfg.totalStudents);
    m_windowPrefCombo->setCurrentText(
        Models::windowPreferenceToString(cfg.windowPreference));
    m_seatPrefCombo->setCurrentText(
        Models::seatPreferenceToString(cfg.seatPreference));
    m_mealMeanSpin->setValue(cfg.mealDurationMean);
    m_mealStddevSpin->setValue(cfg.mealDurationStddev);

    // 加载抵达分布
    m_distTable->setRowCount(0);
    m_distTable->blockSignals(true);
    for (const auto &d : cfg.arrivalDistribution) {
        int row = m_distTable->rowCount();
        m_distTable->insertRow(row);
        m_distTable->setItem(row, 0, new QTableWidgetItem(d.time));
        m_distTable->setItem(row, 1, new QTableWidgetItem(QString::number(d.rate)));
    }
    m_distTable->blockSignals(false);

    m_updatingFromConfig = false;
}

void StudentBehaviorPage::saveToConfig()
{
    auto &cfg = m_configMgr->config().student;
    cfg.totalStudents   = m_totalStudentsSpin->value();
    cfg.windowPreference = Models::stringToWindowPreference(m_windowPrefCombo->currentText());
    cfg.seatPreference   = Models::stringToSeatPreference(m_seatPrefCombo->currentText());
    cfg.mealDurationMean   = m_mealMeanSpin->value();
    cfg.mealDurationStddev = m_mealStddevSpin->value();

    cfg.arrivalDistribution.clear();
    for (int i = 0; i < m_distTable->rowCount(); ++i) {
        Core::ArrivalDistributionPoint p;
        p.time = m_distTable->item(i, 0)->text();
        p.rate = m_distTable->item(i, 1)->text().toInt();
        cfg.arrivalDistribution.append(p);
    }
}

void StudentBehaviorPage::onAddTimePoint()
{
    int row = m_distTable->rowCount();
    m_distTable->insertRow(row);
    m_distTable->setItem(row, 0, new QTableWidgetItem(QStringLiteral("18:00")));
    m_distTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("0")));
    m_distTable->selectRow(row);
}

void StudentBehaviorPage::onRemoveTimePoint()
{
    auto items = m_distTable->selectedItems();
    if (items.isEmpty()) return;

    QSet<int> rows;
    for (auto *item : items) rows.insert(item->row());

    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    for (int r : sortedRows) m_distTable->removeRow(r);
}

void StudentBehaviorPage::onDistCellChanged(int row, int col)
{
    if (m_updatingFromConfig) return;
    saveToConfig();
}
