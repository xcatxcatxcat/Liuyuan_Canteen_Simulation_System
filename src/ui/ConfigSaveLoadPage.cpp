#include "ConfigSaveLoadPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QScrollArea>

ConfigSaveLoadPage::ConfigSaveLoadPage(Core::ConfigManager *configMgr, QWidget *parent)
    : QWidget(parent)
    , m_configMgr(configMgr)
{
    setupUi();
    loadFromConfig();
}

void ConfigSaveLoadPage::setupUi()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget;
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== 保存配置 =====
    auto *saveGroup = new QGroupBox(QStringLiteral("保存当前配置"));
    auto *saveLayout = new QVBoxLayout(saveGroup);

    auto *hint = new QLabel(
        QStringLiteral("将当前所有标签页中的配置保存为命名方案，方便日后快速加载和对比不同方案。"));
    hint->setWordWrap(true);
    hint->setStyleSheet(QStringLiteral("color: #78909c; font-size: 12px; padding-bottom: 8px;"));
    saveLayout->addWidget(hint);

    auto *nameForm = new QFormLayout;
    m_configNameEdit = new QLineEdit;
    m_configNameEdit->setPlaceholderText(QStringLiteral("例如：方案 A — 基准配置"));
    nameForm->addRow(QStringLiteral("配置名称："), m_configNameEdit);
    saveLayout->addLayout(nameForm);

    auto *saveBtnRow = new QHBoxLayout;
    saveBtnRow->addStretch();

    auto *saveBtn = new QPushButton(QStringLiteral("  保存配置"));
    saveBtn->setObjectName(QStringLiteral("startBtn"));
    saveBtn->setFixedWidth(140);
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onSaveConfig);
    saveBtnRow->addWidget(saveBtn);

    saveLayout->addLayout(saveBtnRow);
    mainLayout->addWidget(saveGroup);

    // ===== 已保存配置列表 =====
    auto *listGroup = new QGroupBox(QStringLiteral("已保存的配置方案"));
    auto *listLayout = new QVBoxLayout(listGroup);

    m_configListWidget = new QListWidget;
    m_configListWidget->setAlternatingRowColors(true);
    m_configListWidget->setMinimumHeight(200);
    listLayout->addWidget(m_configListWidget);

    auto *listBtnRow = new QHBoxLayout;
    listBtnRow->addStretch();

    auto *loadBtn = new QPushButton(QStringLiteral("加载选中"));
    loadBtn->setObjectName(QStringLiteral("loadBtn"));
    loadBtn->setCursor(Qt::PointingHandCursor);
    connect(loadBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onLoadConfig);

    auto *deleteBtn = new QPushButton(QStringLiteral("删除选中"));
    deleteBtn->setObjectName(QStringLiteral("removeBtn"));
    deleteBtn->setCursor(Qt::PointingHandCursor);
    connect(deleteBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onDeleteConfig);

    auto *refreshBtn = new QPushButton(QStringLiteral("刷新列表"));
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onRefreshList);

    listBtnRow->addWidget(loadBtn);
    listBtnRow->addWidget(deleteBtn);
    listBtnRow->addWidget(refreshBtn);
    listLayout->addLayout(listBtnRow);

    mainLayout->addWidget(listGroup);

    // ===== 导入/导出 =====
    auto *ioGroup = new QGroupBox(QStringLiteral("导入 / 导出"));
    auto *ioLayout = new QHBoxLayout(ioGroup);

    auto *exportBtn = new QPushButton(QStringLiteral("  导出为 JSON 文件…"));
    exportBtn->setCursor(Qt::PointingHandCursor);
    exportBtn->setToolTip(QStringLiteral("将当前配置导出为 JSON 文件，可在其他地方使用"));
    connect(exportBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onExportToJson);
    ioLayout->addWidget(exportBtn);

    ioLayout->addStretch();

    auto *importBtn = new QPushButton(QStringLiteral("  从 JSON 文件导入…"));
    importBtn->setCursor(Qt::PointingHandCursor);
    importBtn->setToolTip(QStringLiteral("从外部 JSON 文件导入配置"));
    connect(importBtn, &QPushButton::clicked, this, &ConfigSaveLoadPage::onImportFromJson);
    ioLayout->addWidget(importBtn);

    mainLayout->addWidget(ioGroup);

    // ===== 信息区 =====
    m_infoLabel = new QLabel;
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setStyleSheet(QStringLiteral(
        "background: #e8f5e9; border: 1px solid #a5d6a7; border-radius: 6px; "
        "padding: 10px 14px; color: #2e7d32; font-size: 12px;"));
    mainLayout->addWidget(m_infoLabel);

    mainLayout->addStretch();

    scrollArea->setWidget(container);

    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addWidget(scrollArea);
}

void ConfigSaveLoadPage::loadFromConfig()
{
    refreshConfigList();
    m_infoLabel->setText(QStringLiteral("就绪。您可以保存当前配置或加载已保存的方案。"));
}

void ConfigSaveLoadPage::refreshConfigList()
{
    m_configListWidget->clear();

    QDir configDir(QStringLiteral("data"));
    if (!configDir.exists()) return;

    QFileInfoList files = configDir.entryInfoList(
        QStringList() << QStringLiteral("*.json"),
        QDir::Files, QDir::Time);

    for (const auto &f : files) {
        QString displayName = f.baseName();
        QString lastModified = f.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        m_configListWidget->addItem(
            QString("%1  [%2]").arg(displayName).arg(lastModified));
    }

    if (m_configListWidget->count() == 0) {
        m_configListWidget->addItem(QStringLiteral("（暂无已保存的配置）"));
    }
}

void ConfigSaveLoadPage::onSaveConfig()
{
    QString name = m_configNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"),
            QStringLiteral("请输入配置名称。"));
        return;
    }

    // 清理文件名中的非法字符
    QString safeName = name;
    safeName.replace(QRegularExpression(QStringLiteral("[\\\\/:*?\"<>|]")), QStringLiteral("_"));

    QDir dir(QStringLiteral("data"));
    if (!dir.exists()) dir.mkpath(QStringLiteral("."));

    QString filePath = QStringLiteral("data/%1.json").arg(safeName);

    if (m_configMgr->saveToFile(filePath)) {
        m_infoLabel->setText(QStringLiteral("配置已保存：%1").arg(filePath));
        m_infoLabel->setStyleSheet(QStringLiteral(
            "background: #e8f5e9; border: 1px solid #a5d6a7; border-radius: 6px; "
            "padding: 10px 14px; color: #2e7d32; font-size: 12px;"));
        refreshConfigList();
    } else {
        m_infoLabel->setText(QStringLiteral("保存失败，请检查 data/ 目录权限。"));
        m_infoLabel->setStyleSheet(QStringLiteral(
            "background: #ffebee; border: 1px solid #ef9a9a; border-radius: 6px; "
            "padding: 10px 14px; color: #c62828; font-size: 12px;"));
    }
}

void ConfigSaveLoadPage::onLoadConfig()
{
    int row = m_configListWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
            QStringLiteral("请先选择要加载的配置方案。"));
        return;
    }

    QString itemText = m_configListWidget->item(row)->text();
    // 提取文件名（去掉方括号后面的时间）
    QString fileName = itemText.section(' ', 0, 0);
    QString filePath = QStringLiteral("data/%1.json").arg(fileName);

    if (m_configMgr->loadFromFile(filePath)) {
        m_infoLabel->setText(QStringLiteral("已加载配置：%1").arg(fileName));
        m_infoLabel->setStyleSheet(QStringLiteral(
            "background: #e3f2fd; border: 1px solid #90caf9; border-radius: 6px; "
            "padding: 10px 14px; color: #1565c0; font-size: 12px;"));
    } else {
        QMessageBox::warning(this, QStringLiteral("加载失败"),
            QStringLiteral("无法加载配置文件：%1").arg(filePath));
    }
}

void ConfigSaveLoadPage::onDeleteConfig()
{
    int row = m_configListWidget->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
            QStringLiteral("请先选择要删除的配置方案。"));
        return;
    }

    auto ret = QMessageBox::question(this, QStringLiteral("确认删除"),
        QStringLiteral("确定要删除选中的配置方案吗？此操作不可恢复。"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        QString itemText = m_configListWidget->item(row)->text();
        QString fileName = itemText.section(' ', 0, 0);
        QString filePath = QStringLiteral("data/%1.json").arg(fileName);

        if (QFile::remove(filePath)) {
            m_infoLabel->setText(QStringLiteral("已删除：%1").arg(fileName));
            refreshConfigList();
        } else {
            QMessageBox::warning(this, QStringLiteral("删除失败"),
                QStringLiteral("无法删除文件：%1").arg(filePath));
        }
    }
}

void ConfigSaveLoadPage::onRefreshList()
{
    refreshConfigList();
}

void ConfigSaveLoadPage::onExportToJson()
{
    QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出配置"),
        QStringLiteral("simulation_config.json"),
        QStringLiteral("JSON 文件 (*.json)"));

    if (filePath.isEmpty()) return;

    if (m_configMgr->saveToFile(filePath)) {
        m_infoLabel->setText(QStringLiteral("已导出到：%1").arg(filePath));
        m_infoLabel->setStyleSheet(QStringLiteral(
            "background: #e8f5e9; border: 1px solid #a5d6a7; border-radius: 6px; "
            "padding: 10px 14px; color: #2e7d32; font-size: 12px;"));
    } else {
        QMessageBox::warning(this, QStringLiteral("导出失败"),
            QStringLiteral("无法写入文件：%1").arg(filePath));
    }
}

void ConfigSaveLoadPage::onImportFromJson()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, QStringLiteral("导入配置"),
        QString(),
        QStringLiteral("JSON 文件 (*.json)"));

    if (filePath.isEmpty()) return;

    if (m_configMgr->loadFromFile(filePath)) {
        m_configNameEdit->setText(QFileInfo(filePath).baseName());
        m_infoLabel->setText(QStringLiteral("已从文件导入：%1").arg(filePath));
        m_infoLabel->setStyleSheet(QStringLiteral(
            "background: #e3f2fd; border: 1px solid #90caf9; border-radius: 6px; "
            "padding: 10px 14px; color: #1565c0; font-size: 12px;"));
    } else {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
            QStringLiteral("无法解析文件：%1\n\n请确保文件格式正确。").arg(filePath));
    }
}


