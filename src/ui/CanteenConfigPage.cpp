#include "CanteenConfigPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QScrollArea>

CanteenConfigPage::CanteenConfigPage(Core::ConfigManager *configMgr, QWidget *parent)
    : QWidget(parent)
    , m_configMgr(configMgr)
{
    setupUi();
    loadFromConfig();
}

void CanteenConfigPage::setupUi()
{
    // 用 ScrollArea 包裹，支持内容超出时滚动
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget;
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== 食堂基本信息 =====
    auto *basicGroup = new QGroupBox(QStringLiteral("食堂基本信息"));
    auto *basicForm = new QFormLayout(basicGroup);

    m_canteenNameEdit = new QLineEdit;
    m_canteenNameEdit->setPlaceholderText(QStringLiteral("例如：留园食堂"));
    basicForm->addRow(QStringLiteral("食堂名称："), m_canteenNameEdit);

    mainLayout->addWidget(basicGroup);

    // ===== 打饭窗口配置 =====
    auto *windowGroup = new QGroupBox(QStringLiteral("打饭窗口配置"));
    auto *windowLayout = new QVBoxLayout(windowGroup);

    // 窗口表格
    m_windowTable = new QTableWidget;
    m_windowTable->setColumnCount(4);
    m_windowTable->setHorizontalHeaderLabels({
        QStringLiteral("窗口名称"),
        QStringLiteral("平均服务时长（秒/人）"),
        QStringLiteral("服务员工数"),
        QStringLiteral("队列容量")
    });
    m_windowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_windowTable->horizontalHeader()->setMinimumSectionSize(140);
    m_windowTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_windowTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    m_windowTable->setAlternatingRowColors(true);
    m_windowTable->verticalHeader()->setVisible(false);
    windowLayout->addWidget(m_windowTable);

    // 增删按钮行
    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();

    m_addWindowBtn = new QPushButton(QStringLiteral("+ 添加窗口"));
    m_addWindowBtn->setObjectName(QStringLiteral("addBtn"));
    m_addWindowBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addWindowBtn, &QPushButton::clicked, this, &CanteenConfigPage::onAddWindow);

    m_removeWindowBtn = new QPushButton(QStringLiteral("- 删除选中"));
    m_removeWindowBtn->setObjectName(QStringLiteral("removeBtn"));
    m_removeWindowBtn->setCursor(Qt::PointingHandCursor);
    connect(m_removeWindowBtn, &QPushButton::clicked, this, &CanteenConfigPage::onRemoveWindow);

    btnRow->addWidget(m_addWindowBtn);
    btnRow->addWidget(m_removeWindowBtn);
    windowLayout->addLayout(btnRow);

    mainLayout->addWidget(windowGroup);

    // ===== 座位区域配置 =====
    auto *seatGroup = new QGroupBox(QStringLiteral("座位区域配置"));
    auto *seatForm = new QFormLayout(seatGroup);

    m_tableCountSpin = new QSpinBox;
    m_tableCountSpin->setRange(1, 200);
    m_tableCountSpin->setValue(15);

    m_seatsPerTableSpin = new QSpinBox;
    m_seatsPerTableSpin->setRange(1, 12);
    m_seatsPerTableSpin->setValue(4);

    m_totalSeatsLabel = new QLabel(QStringLiteral("0 个座位"));
    m_totalSeatsLabel->setStyleSheet(QStringLiteral("font-weight: bold; color: #1a237e;"));

    auto *seatRow = new QHBoxLayout;
    seatRow->addWidget(m_tableCountSpin);
    seatRow->addWidget(new QLabel(QStringLiteral("桌 ×")));
    seatRow->addWidget(m_seatsPerTableSpin);
    seatRow->addWidget(new QLabel(QStringLiteral("座/桌 =")));
    seatRow->addWidget(m_totalSeatsLabel);

    seatForm->addRow(QStringLiteral("桌子数量："), seatRow);

    auto updateSeats = [this]() {
        m_totalSeatsLabel->setText(
            QString("%1 个座位").arg(m_tableCountSpin->value() * m_seatsPerTableSpin->value()));
    };
    connect(m_tableCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), updateSeats);
    connect(m_seatsPerTableSpin, QOverload<int>::of(&QSpinBox::valueChanged), updateSeats);

    mainLayout->addWidget(seatGroup);

    // ===== 备用窗口配置 =====
    auto *backupGroup = new QGroupBox(QStringLiteral("备用窗口配置（高负载自动开放）"));
    auto *backupForm = new QFormLayout(backupGroup);

    m_backupThresholdSpin = new QSpinBox;
    m_backupThresholdSpin->setRange(1, 200);
    m_backupThresholdSpin->setSuffix(QStringLiteral(" 人"));
    m_backupThresholdSpin->setToolTip(QStringLiteral("当排队总人数达到此阈值时自动开放备用窗口"));
    backupForm->addRow(QStringLiteral("触发阈值："), m_backupThresholdSpin);

    m_backupNameEdit = new QLineEdit(QStringLiteral("备用快餐口"));
    backupForm->addRow(QStringLiteral("窗口名称："), m_backupNameEdit);

    m_backupServiceTimeSpin = new QDoubleSpinBox;
    m_backupServiceTimeSpin->setRange(1.0, 600.0);
    m_backupServiceTimeSpin->setSuffix(QStringLiteral(" 秒"));
    m_backupServiceTimeSpin->setDecimals(1);
    backupForm->addRow(QStringLiteral("平均服务时长："), m_backupServiceTimeSpin);

    m_backupQueueLenSpin = new QSpinBox;
    m_backupQueueLenSpin->setRange(1, 100);
    m_backupQueueLenSpin->setSuffix(QStringLiteral(" 人"));
    backupForm->addRow(QStringLiteral("队列容量："), m_backupQueueLenSpin);

    mainLayout->addWidget(backupGroup);

    mainLayout->addStretch();

    scrollArea->setWidget(container);

    // 将 ScrollArea 作为页面根布局
    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);

    // 表格变更时自动保存
    connect(m_windowTable, &QTableWidget::cellChanged,
            this, &CanteenConfigPage::onWindowCellChanged);
}

void CanteenConfigPage::loadFromConfig()
{
    m_updatingFromConfig = true;

    auto &cfg = m_configMgr->config().canteen;
    m_canteenNameEdit->setText(cfg.canteenName);

    // 加载窗口表格
    m_windowTable->setRowCount(0);
    m_windowTable->blockSignals(true);
    for (const auto &w : cfg.windows) {
        int row = m_windowTable->rowCount();
        m_windowTable->insertRow(row);
        m_windowTable->setItem(row, 0, new QTableWidgetItem(w.windowName));
        m_windowTable->setItem(row, 1, new QTableWidgetItem(QString::number(w.avgServiceTime)));
        m_windowTable->setItem(row, 2, new QTableWidgetItem(QString::number(w.staffCount)));
        m_windowTable->setItem(row, 3, new QTableWidgetItem(QString::number(w.maxQueueLength)));
    }
    m_windowTable->blockSignals(false);

    // 加载桌子配置（简化为统一桌型）
    if (!cfg.tables.isEmpty()) {
        m_tableCountSpin->setValue(cfg.tables.size());
        m_seatsPerTableSpin->setValue(cfg.tables.first().seatCount);
    }

    // 加载备用窗口
    m_backupThresholdSpin->setValue(cfg.backupWindow.triggerThreshold);
    m_backupNameEdit->setText(cfg.backupWindow.windowName);
    m_backupServiceTimeSpin->setValue(cfg.backupWindow.avgServiceTime);
    m_backupQueueLenSpin->setValue(cfg.backupWindow.maxQueueLength);

    m_updatingFromConfig = false;
}

void CanteenConfigPage::saveToConfig()
{
    auto &cfg = m_configMgr->config().canteen;
    cfg.canteenName = m_canteenNameEdit->text();

    // 保存窗口
    cfg.windows.clear();
    for (int i = 0; i < m_windowTable->rowCount(); ++i) {
        Core::WindowConfig wc;
        wc.windowName      = m_windowTable->item(i, 0)->text();
        wc.avgServiceTime  = m_windowTable->item(i, 1)->text().toDouble();
        wc.staffCount      = m_windowTable->item(i, 2)->text().toInt();
        wc.maxQueueLength  = m_windowTable->item(i, 3)->text().toInt();
        cfg.windows.append(wc);
    }

    // 保存桌子
    cfg.tables.clear();
    for (int i = 0; i < m_tableCountSpin->value(); ++i) {
        cfg.tables.append({ i + 1, m_seatsPerTableSpin->value() });
    }

    // 保存备用窗口
    cfg.backupWindow.triggerThreshold = m_backupThresholdSpin->value();
    cfg.backupWindow.windowName       = m_backupNameEdit->text();
    cfg.backupWindow.avgServiceTime   = m_backupServiceTimeSpin->value();
    cfg.backupWindow.maxQueueLength   = m_backupQueueLenSpin->value();
}

// ============================================================
//  槽函数
// ============================================================

void CanteenConfigPage::onAddWindow()
{
    int row = m_windowTable->rowCount();
    m_windowTable->insertRow(row);
    m_windowTable->setItem(row, 0, new QTableWidgetItem(QStringLiteral("新窗口")));
    m_windowTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("30")));
    m_windowTable->setItem(row, 2, new QTableWidgetItem(QStringLiteral("1")));
    m_windowTable->setItem(row, 3, new QTableWidgetItem(QStringLiteral("20")));
    m_windowTable->selectRow(row);
    m_windowTable->editItem(m_windowTable->item(row, 0));
}

void CanteenConfigPage::onRemoveWindow()
{
    auto items = m_windowTable->selectedItems();
    if (items.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"),
            QStringLiteral("请先选中要删除的窗口行。"));
        return;
    }

    // 收集选中行（去重）
    QSet<int> rows;
    for (auto *item : items) rows.insert(item->row());

    // 从大到小删除，避免索引偏移
    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    for (int r : sortedRows) m_windowTable->removeRow(r);
}

void CanteenConfigPage::onWindowCellChanged(int row, int col)
{
    if (m_updatingFromConfig) return;
    // 自动同步到 ConfigManager
    saveToConfig();
}
