#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QMessageBox>
#include <QStatusBar>
#include <QMenuBar>

#include "CanteenConfigPage.h"
#include "StudentBehaviorPage.h"
#include "TeachingBuildingPage.h"
#include "SimulationParamsPage.h"
#include "ConfigSaveLoadPage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_configManager(new Core::ConfigManager(this))
{
    setupUi();
    setupMenuBar();
    setupStyleSheet();

    // 配置验证信号连接
    connect(m_configManager, &Core::ConfigManager::configValidationFailed,
            this, &MainWindow::onConfigValidationFailed);

    m_statusLabel->setText(QStringLiteral("就绪 — 请配置仿真参数后点击「开始仿真」"));
}

MainWindow::~MainWindow() = default;

// ============================================================
//  UI 构建
// ============================================================

void MainWindow::setupUi()
{
    setWindowTitle(QStringLiteral("留园食堂就餐仿真系统 — 配置"));
    setMinimumSize(1100, 700);
    resize(1200, 750);

    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu(QStringLiteral("文件(&F)"));
    fileMenu->addAction(QStringLiteral("新建配置"), this, [this]() {
        m_configManager->resetToDefaults();
        m_statusLabel->setText(QStringLiteral("已重置为默认配置"));
    }, QKeySequence::New);
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("退出"), this, &QMainWindow::close, QKeySequence::Quit);

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu(QStringLiteral("帮助(&H)"));
    helpMenu->addAction(QStringLiteral("关于"), this, [this]() {
        QMessageBox::about(this, QStringLiteral("关于"),
            QStringLiteral("<h3>留园食堂就餐仿真系统</h3>"
                           "<p>版本 1.0.0</p>"
                           "<p>北京交通大学 计算机科学与技术学院</p>"
                           "<p>小组编号：15</p>"
                           "<p>成员：井博冉、杨心茹、蒋思语</p>"
                           "<p>指导教师：韩升</p>"));
    });
}

void MainWindow::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ---- 顶部标题栏 ----
    QWidget *headerWidget = new QWidget;
    headerWidget->setObjectName(QStringLiteral("headerBar"));
    headerWidget->setFixedHeight(56);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(24, 0, 24, 0);

    m_titleLabel = new QLabel(QStringLiteral("  留园食堂就餐仿真系统"));
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    QLabel *subLabel = new QLabel(QStringLiteral("配置中心"));
    subLabel->setObjectName(QStringLiteral("subTitleLabel"));
    headerLayout->addWidget(subLabel);

    mainLayout->addWidget(headerWidget);

    // ---- 中部：标签页 + 内容区 ----
    setupTabBar();
    mainLayout->addWidget(m_tabWidget, 1);

    // ---- 底部操作栏 ----
    QWidget *footerWidget = new QWidget;
    footerWidget->setObjectName(QStringLiteral("footerBar"));
    footerWidget->setFixedHeight(64);

    QHBoxLayout *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(24, 0, 24, 0);

    footerLayout->addStretch();

    m_resetBtn = new QPushButton(QStringLiteral("恢复默认"));
    m_resetBtn->setObjectName(QStringLiteral("resetBtn"));
    m_resetBtn->setFixedHeight(38);
    m_resetBtn->setCursor(Qt::PointingHandCursor);
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onResetConfig);
    footerLayout->addWidget(m_resetBtn);

    m_startBtn = new QPushButton(QStringLiteral("  开始仿真"));
    m_startBtn->setObjectName(QStringLiteral("startBtn"));
    m_startBtn->setFixedHeight(38);
    m_startBtn->setFixedWidth(160);
    m_startBtn->setCursor(Qt::PointingHandCursor);
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    footerLayout->addWidget(m_startBtn);

    mainLayout->addWidget(footerWidget);
}

void MainWindow::setupTabBar()
{
    m_tabWidget = new QTabWidget;
    m_tabWidget->setObjectName(QStringLiteral("mainTabWidget"));

    // 创建各配置标签页
    auto *canteenCfgPage = new CanteenConfigPage(m_configManager);
    auto *studentCfgPage = new StudentBehaviorPage(m_configManager);
    auto *teachingCfgPage = new TeachingBuildingPage(m_configManager);
    auto *simParamsPage   = new SimulationParamsPage(m_configManager);
    auto *configIOPage    = new ConfigSaveLoadPage(m_configManager);

    // 用图标前缀增加辨识度
    m_tabWidget->addTab(canteenCfgPage,    QStringLiteral("  食堂配置"));
    m_tabWidget->addTab(studentCfgPage,    QStringLiteral("  学生行为配置"));
    m_tabWidget->addTab(teachingCfgPage,   QStringLiteral("  教学楼与排课"));
    m_tabWidget->addTab(simParamsPage,     QStringLiteral("  仿真运行参数"));
    m_tabWidget->addTab(configIOPage,      QStringLiteral("  保存 / 加载"));

    // 设置标签页样式为左侧垂直布局
    m_tabWidget->setTabPosition(QTabWidget::West);
    m_tabWidget->setDocumentMode(true);
}

void MainWindow::setupStatusBar()
{
    QStatusBar *statusBar = this->statusBar();
    statusBar->setObjectName(QStringLiteral("statusBar"));
    m_statusLabel = new QLabel;
    statusBar->addWidget(m_statusLabel, 1);
}

// ============================================================
//  QSS 样式表
// ============================================================

void MainWindow::setupStyleSheet()
{
    setStyleSheet(QStringLiteral(R"(
        /* ===== 全局 ===== */
        QMainWindow {
            background-color: #f5f6fa;
        }
        QWidget {
            font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif;
            font-size: 13px;
        }

        /* ===== 顶部标题栏 ===== */
        #headerBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #1a237e, stop:1 #283593);
        }
        #titleLabel {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            letter-spacing: 1px;
        }
        #subTitleLabel {
            color: rgba(255,255,255,0.7);
            font-size: 13px;
            padding-right: 8px;
        }

        /* ===== 标签页 ===== */
        #mainTabWidget::pane {
            border: none;
            background: #ffffff;
            border-radius: 0 8px 8px 0;
            margin-left: -1px;
        }
        #mainTabWidget QTabBar::tab {
            background: #e8eaf6;
            color: #3949ab;
            border: none;
            border-right: 2px solid #c5cae9;
            padding: 16px 28px;
            margin: 2px 0;
            min-width: 160px;
            font-size: 13px;
        }
        #mainTabWidget QTabBar::tab:selected {
            background: #ffffff;
            color: #1a237e;
            font-weight: bold;
            border-right: 3px solid #1a237e;
        }
        #mainTabWidget QTabBar::tab:hover:!selected {
            background: #f0f1ff;
        }

        /* ===== 底部操作栏 ===== */
        #footerBar {
            background: #ffffff;
            border-top: 1px solid #e0e0e0;
        }

        /* ===== 按钮 ===== */
        #startBtn {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #43a047, stop:1 #2e7d32);
            color: #ffffff;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
            padding: 0 24px;
        }
        #startBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #4caf50, stop:1 #388e3c);
        }
        #startBtn:pressed {
            background: #1b5e20;
        }

        #resetBtn {
            background: #ffffff;
            color: #616161;
            border: 1px solid #bdbdbd;
            border-radius: 6px;
            font-size: 13px;
            padding: 0 20px;
        }
        #resetBtn:hover {
            background: #f5f5f5;
            border-color: #9e9e9e;
            color: #424242;
        }

        /* ===== 状态栏 ===== */
        #statusBar {
            background: #fafafa;
            border-top: 1px solid #e0e0e0;
            color: #757575;
            font-size: 12px;
        }

        /* ===== 通用控件增强 ===== */
        QGroupBox {
            font-weight: bold;
            color: #37474f;
            border: 1px solid #cfd8dc;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 18px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 16px;
            padding: 0 8px;
        }

        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            border: 1px solid #cfd8dc;
            border-radius: 4px;
            padding: 6px 10px;
            background: #ffffff;
            color: #263238;
            selection-background-color: #c5cae9;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border: 2px solid #3f51b5;
        }

        QTableWidget {
            gridline-color: #e0e0e0;
            border: 1px solid #cfd8dc;
            border-radius: 4px;
            selection-background-color: #e8eaf6;
            selection-color: #1a237e;
        }
        QTableWidget::item {
            padding: 6px;
        }
        QHeaderView::section {
            background: #eceff1;
            color: #37474f;
            border: none;
            border-bottom: 2px solid #b0bec5;
            border-right: 1px solid #cfd8dc;
            padding: 8px 12px;
            font-weight: bold;
        }

        QPushButton {
            border-radius: 4px;
            padding: 6px 16px;
        }

        QScrollArea {
            border: none;
            background: transparent;
        }

        QLabel {
            color: #455a64;
        }
    )"));
}

// ============================================================
//  槽函数
// ============================================================

void MainWindow::onStartSimulation()
{
    // 先从各页面收集最新配置（此处先做基本验证）
    QStringList errors;
    if (!m_configManager->validate(errors)) {
        QMessageBox::warning(this, QStringLiteral("配置验证失败"),
            QStringLiteral("请检查以下配置项：\n\n") + errors.join(QStringLiteral("\n")));
        return;
    }

    // 保存配置到文件
    QString configPath = QStringLiteral("data/") + Core::ConfigManager::defaultConfigFileName();
    if (m_configManager->saveToFile(configPath)) {
        m_statusLabel->setText(QStringLiteral("配置已保存，准备启动仿真引擎…（仿真运行界面开发中）"));
        QMessageBox::information(this, QStringLiteral("就绪"),
            QStringLiteral("仿真配置已保存至：%1\n\n仿真运行界面正在开发中，敬请期待。").arg(configPath));
    } else {
        m_statusLabel->setText(QStringLiteral("配置保存失败"));
        QMessageBox::critical(this, QStringLiteral("错误"),
            QStringLiteral("无法保存配置文件，请检查 data/ 目录是否可写。"));
    }
}

void MainWindow::onResetConfig()
{
    auto ret = QMessageBox::question(this, QStringLiteral("确认重置"),
        QStringLiteral("确定要恢复所有配置为默认值吗？\n当前未保存的修改将丢失。"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        m_configManager->resetToDefaults();
        m_statusLabel->setText(QStringLiteral("已恢复默认配置"));
    }
}

void MainWindow::onConfigValidationFailed(const QStringList &errors)
{
    Q_UNUSED(errors);
    m_statusLabel->setText(QStringLiteral("配置存在错误，请检查各标签页"));
}
