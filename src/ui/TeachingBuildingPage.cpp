#include "TeachingBuildingPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QScrollArea>
#include <QMessageBox>

TeachingBuildingPage::TeachingBuildingPage(Core::ConfigManager *configMgr, QWidget *parent)
    : QWidget(parent)
    , m_configMgr(configMgr)
{
    setupUi();
    loadFromConfig();
}

void TeachingBuildingPage::setupUi()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget;
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== 提示信息 =====
    auto *hint = new QLabel(
        QStringLiteral("配置教学楼及下课时间表，用于宏观仿真中模拟下课后的学生涌入高峰。"
                        "不同教学楼的下课时间直接影响食堂的客流分布。"));
    hint->setWordWrap(true);
    hint->setStyleSheet(QStringLiteral(
        "background: #e3f2fd; border: 1px solid #90caf9; border-radius: 6px; "
        "padding: 12px 16px; color: #1565c0; font-size: 12px;"));
    mainLayout->addWidget(hint);

    // ===== 教学楼列表 =====
    auto *buildingGroup = new QGroupBox(QStringLiteral("教学楼列表"));
    auto *buildingLayout = new QVBoxLayout(buildingGroup);

    m_buildingTable = new QTableWidget;
    m_buildingTable->setColumnCount(2);
    m_buildingTable->setHorizontalHeaderLabels({
        QStringLiteral("教学楼名称"),
        QStringLiteral("位置")
    });
    m_buildingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_buildingTable->horizontalHeader()->setMinimumSectionSize(200);
    m_buildingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_buildingTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_buildingTable->setAlternatingRowColors(true);
    m_buildingTable->verticalHeader()->setVisible(false);
    m_buildingTable->setMaximumHeight(160);
    buildingLayout->addWidget(m_buildingTable);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    auto *addBuildBtn = new QPushButton(QStringLiteral("+ 添加教学楼"));
    addBuildBtn->setObjectName(QStringLiteral("addBtn"));
    addBuildBtn->setCursor(Qt::PointingHandCursor);
    connect(addBuildBtn, &QPushButton::clicked, this, &TeachingBuildingPage::onAddBuilding);

    auto *removeBuildBtn = new QPushButton(QStringLiteral("- 删除选中"));
    removeBuildBtn->setObjectName(QStringLiteral("removeBtn"));
    removeBuildBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBuildBtn, &QPushButton::clicked, this, &TeachingBuildingPage::onRemoveBuilding);

    btnRow->addWidget(addBuildBtn);
    btnRow->addWidget(removeBuildBtn);
    buildingLayout->addLayout(btnRow);

    mainLayout->addWidget(buildingGroup);

    // ===== 下课时间表 =====
    auto *scheduleGroup = new QGroupBox(QStringLiteral("下课时间表"));
    auto *scheduleLayout = new QVBoxLayout(scheduleGroup);

    m_currentBuildingLabel = new QLabel(QStringLiteral("请选择上方的教学楼以查看/编辑下课时间表"));
    m_currentBuildingLabel->setStyleSheet(QStringLiteral("font-weight: bold; color: #1a237e;"));
    scheduleLayout->addWidget(m_currentBuildingLabel);

    m_scheduleTable = new QTableWidget;
    m_scheduleTable->setColumnCount(2);
    m_scheduleTable->setHorizontalHeaderLabels({
        QStringLiteral("下课时间"),
        QStringLiteral("下课学生人数")
    });
    m_scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_scheduleTable->horizontalHeader()->setMinimumSectionSize(200);
    m_scheduleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_scheduleTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_scheduleTable->setAlternatingRowColors(true);
    m_scheduleTable->verticalHeader()->setVisible(false);
    m_scheduleTable->setMaximumHeight(200);
    scheduleLayout->addWidget(m_scheduleTable);

    auto *schedBtnRow = new QHBoxLayout;
    schedBtnRow->addStretch();

    auto *addSchedBtn = new QPushButton(QStringLiteral("+ 添加时间"));
    addSchedBtn->setObjectName(QStringLiteral("addBtn"));
    addSchedBtn->setCursor(Qt::PointingHandCursor);
    connect(addSchedBtn, &QPushButton::clicked, this, &TeachingBuildingPage::onAddSchedule);

    auto *removeSchedBtn = new QPushButton(QStringLiteral("- 删除选中"));
    removeSchedBtn->setObjectName(QStringLiteral("removeBtn"));
    removeSchedBtn->setCursor(Qt::PointingHandCursor);
    connect(removeSchedBtn, &QPushButton::clicked, this, &TeachingBuildingPage::onRemoveSchedule);

    schedBtnRow->addWidget(addSchedBtn);
    schedBtnRow->addWidget(removeSchedBtn);
    scheduleLayout->addLayout(schedBtnRow);

    mainLayout->addWidget(scheduleGroup);

    // ===== 错峰方案预设 =====
    auto *presetGroup = new QGroupBox(QStringLiteral("错峰下课方案预设"));
    auto *presetForm = new QFormLayout(presetGroup);

    auto *presetHint = new QLabel(
        QStringLiteral("选择预设方案可快速调整所有教学楼的下课时间。"
                        "应用后请检查下课时间表是否正确。"));
    presetHint->setWordWrap(true);
    presetHint->setStyleSheet(QStringLiteral("color: #78909c; font-size: 12px; padding-bottom: 4px;"));
    presetForm->addRow(presetHint);

    m_presetCombo = new QComboBox;
    m_presetCombo->addItem(QStringLiteral("（无预设 — 自定义）"));
    m_presetCombo->addItem(QStringLiteral("方案 A：逸夫楼延后 20 分钟"));
    m_presetCombo->addItem(QStringLiteral("方案 B：全部错峰 15 分钟"));
    m_presetCombo->addItem(QStringLiteral("方案 C：思源楼提前 10 分钟"));
    m_presetCombo->setToolTip(QStringLiteral("选择预设方案后自动调整下课时间"));
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TeachingBuildingPage::onPresetSchemeChanged);
    presetForm->addRow(QStringLiteral("预设方案："), m_presetCombo);

    mainLayout->addWidget(presetGroup);
    mainLayout->addStretch();

    scrollArea->setWidget(container);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);

    // 教学楼选中时刷新下课时间表
    connect(m_buildingTable, &QTableWidget::cellClicked,
            this, &TeachingBuildingPage::onCurrentBuildingChanged);
    connect(m_buildingTable, &QTableWidget::cellChanged,
            this, &TeachingBuildingPage::onBuildingCellChanged);
}

void TeachingBuildingPage::loadFromConfig()
{
    m_updatingFromConfig = true;

    auto &buildings = m_configMgr->config().teachingBuildings;

    m_buildingTable->setRowCount(0);
    m_buildingTable->blockSignals(true);
    for (const auto &b : buildings) {
        int row = m_buildingTable->rowCount();
        m_buildingTable->insertRow(row);
        m_buildingTable->setItem(row, 0, new QTableWidgetItem(b.buildingName));
        m_buildingTable->setItem(row, 1, new QTableWidgetItem(b.location));
    }
    m_buildingTable->blockSignals(false);

    // 默认选中第一行
    if (m_buildingTable->rowCount() > 0) {
        m_buildingTable->selectRow(0);
        refreshScheduleTable();
    }

    m_updatingFromConfig = false;
}

void TeachingBuildingPage::saveToConfig()
{
    auto &buildings = m_configMgr->config().teachingBuildings;

    // 先保存当前编辑中的下课时间表
    int currentRow = m_buildingTable->currentRow();
    if (currentRow >= 0 && currentRow < buildings.size()) {
        buildings[currentRow].classEndSchedule.clear();
        for (int i = 0; i < m_scheduleTable->rowCount(); ++i) {
            buildings[currentRow].classEndSchedule.append({
                m_scheduleTable->item(i, 0)->text(),
                m_scheduleTable->item(i, 1)->text().toInt()
            });
        }
    }

    // 更新教学楼名称和位置
    for (int i = 0; i < m_buildingTable->rowCount(); ++i) {
        if (i < buildings.size()) {
            buildings[i].buildingName = m_buildingTable->item(i, 0)->text();
            buildings[i].location     = m_buildingTable->item(i, 1)->text();
        }
    }

    // 移除多余项（如果有删除操作）
    while (buildings.size() > m_buildingTable->rowCount())
        buildings.removeLast();
}

void TeachingBuildingPage::refreshScheduleTable()
{
    int row = m_buildingTable->currentRow();
    auto &buildings = m_configMgr->config().teachingBuildings;

    m_scheduleTable->setRowCount(0);
    m_scheduleTable->blockSignals(true);

    if (row >= 0 && row < buildings.size()) {
        m_currentBuildingLabel->setText(
            QString("当前编辑：%1").arg(buildings[row].buildingName));

        for (const auto &s : buildings[row].classEndSchedule) {
            int r = m_scheduleTable->rowCount();
            m_scheduleTable->insertRow(r);
            m_scheduleTable->setItem(r, 0, new QTableWidgetItem(s.first));
            m_scheduleTable->setItem(r, 1, new QTableWidgetItem(QString::number(s.second)));
        }
    } else {
        m_currentBuildingLabel->setText(QStringLiteral("请选择上方的教学楼以查看/编辑下课时间表"));
    }

    m_scheduleTable->blockSignals(false);
}

void TeachingBuildingPage::onAddBuilding()
{
    // 先保存当前数据
    saveToConfig();

    auto &buildings = m_configMgr->config().teachingBuildings;
    buildings.append({ QStringLiteral("新教学楼"), QStringLiteral("未知") });

    int row = m_buildingTable->rowCount();
    m_buildingTable->insertRow(row);
    m_buildingTable->setItem(row, 0, new QTableWidgetItem(QStringLiteral("新教学楼")));
    m_buildingTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("未知")));
    m_buildingTable->selectRow(row);
}

void TeachingBuildingPage::onRemoveBuilding()
{
    auto items = m_buildingTable->selectedItems();
    if (items.isEmpty()) return;

    QSet<int> rows;
    for (auto *item : items) rows.insert(item->row());

    // 保存当前修改
    saveToConfig();

    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

    auto &buildings = m_configMgr->config().teachingBuildings;
    for (int r : sortedRows) {
        if (r < buildings.size()) buildings.removeAt(r);
        m_buildingTable->removeRow(r);
    }

    loadFromConfig();
}

void TeachingBuildingPage::onAddSchedule()
{
    int r = m_scheduleTable->rowCount();
    m_scheduleTable->insertRow(r);
    m_scheduleTable->setItem(r, 0, new QTableWidgetItem(QStringLiteral("17:00")));
    m_scheduleTable->setItem(r, 1, new QTableWidgetItem(QStringLiteral("100")));
}

void TeachingBuildingPage::onRemoveSchedule()
{
    auto items = m_scheduleTable->selectedItems();
    if (items.isEmpty()) return;

    QSet<int> rows;
    for (auto *item : items) rows.insert(item->row());

    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    for (int r : sortedRows) m_scheduleTable->removeRow(r);
}

void TeachingBuildingPage::onBuildingCellChanged(int row, int col)
{
    if (m_updatingFromConfig) return;
    saveToConfig();
    refreshScheduleTable();
}

void TeachingBuildingPage::onCurrentBuildingChanged(int row, int col)
{
    Q_UNUSED(col);
    // 先保存之前编辑的
    saveToConfig();
    // 刷新新选中的
    refreshScheduleTable();
}

void TeachingBuildingPage::onPresetSchemeChanged(int index)
{
    if (index <= 0) return; // 无预设

    saveToConfig();
    auto &buildings = m_configMgr->config().teachingBuildings;

    if (buildings.isEmpty()) return;

    auto shiftTime = [](const QString &timeStr, int offsetMinutes) -> QString {
        // 简易时间偏移：解析 HH:MM 偏移 offset 分钟
        QStringList parts = timeStr.split(':');
        if (parts.size() != 2) return timeStr;
        int h = parts[0].toInt();
        int m = parts[1].toInt();
        int totalMin = h * 60 + m + offsetMinutes;
        // 归一化到 0-1440
        totalMin = ((totalMin % 1440) + 1440) % 1440;
        return QString("%1:%2").arg(totalMin / 60, 2, 10, QChar('0'))
                              .arg(totalMin % 60, 2, 10, QChar('0'));
    };

    switch (index) {
    case 1: // 方案 A：逸夫楼延后 20 分钟
        for (auto &b : buildings) {
            if (b.buildingName.contains(QStringLiteral("逸夫"))) {
                for (auto &s : b.classEndSchedule)
                    s.first = shiftTime(s.first, 20);
            }
        }
        break;
    case 2: // 方案 B：全部错峰 15 分钟
        for (auto &b : buildings) {
            for (auto &s : b.classEndSchedule)
                s.first = shiftTime(s.first, 15);
        }
        break;
    case 3: // 方案 C：思源楼提前 10 分钟
        for (auto &b : buildings) {
            if (b.buildingName.contains(QStringLiteral("思源"))) {
                for (auto &s : b.classEndSchedule)
                    s.first = shiftTime(s.first, -10);
            }
        }
        break;
    }

    loadFromConfig();

    // 重置 combo 回"无预设"
    m_presetCombo->blockSignals(true);
    m_presetCombo->setCurrentIndex(0);
    m_presetCombo->blockSignals(false);
}


