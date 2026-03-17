
#include "AboutDialog.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QRegularExpression>

#include "ui_AboutDialog.h"
#include "../res/version.h"
#include "pluginsdk/_plugins.h"

bool isDark();

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    m_dark = isDark();

    setModal(true);
    setWindowFlags(windowFlags() &
        ~Qt::WindowContextHelpButtonHint |
        Qt::MSWindowsFixedSizeDialogHint);

    QString title = QStringLiteral("%1 %2")
        .arg("About")
        .arg(QString::fromWCharArray(PRODUCT_NAME_STRING));
    setWindowTitle(title);

    ui->copyrightLabel->setText(QString(QString::fromWCharArray(LEGAL_COPYRIGHT_STRING)));
    ui->versionLabel->setText(QString().sprintf("Version: %.1f", PLUGIN_VERSION));
    ui->releaseLabel->setText(ReleaseString(CompileDate()));
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
    ui->okButton->setDefault(true);
    ui->okButton->setAutoDefault(true);
    connect(ui->aboutQtButton, &QPushButton::clicked, this, [this] { AboutQt(); });
    setColor();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setColor()
{
    if (m_dark)
    {
        char* buf = (char*)malloc(MAX_SETTING_SIZE);
        if (!buf)
            return;
        if (BridgeSettingGet("Colors", "LinkColor", buf))
        {
            QString linkColor = QString::fromLatin1(buf);
            if (!linkColor.startsWith('#'))
                linkColor.prepend('#');

            QString html = ui->authorLabel->text();
            html.replace(QRegularExpression(R"(color:\s*#[0-9a-fA-F]{6})"), "color:" + linkColor);
            ui->authorLabel->setText(html);
        }
        if (BridgeSettingGet("Colors", "AbstractTableViewSeparatorColor", buf))
        {
            QString lineColor = QString::fromLatin1(buf);
            if (!lineColor.startsWith('#'))
                lineColor.prepend('#');

            ui->sepLine->setStyleSheet(QString("QFrame#sepLine{ border-top: 1px solid %1; }").arg(lineColor));
        }
        free(buf);
    }
    const char* background = R"(
    QPushButton {
        color: #FFFFFF;
        border-radius: 2px;
        background-image: url(:/png/images/ok.png);
        background-repeat: no-repeat;
        background-position: center;
    })";
        const char* pressed = R"(
    QPushButton:pressed {
        padding-top: -15px;
        padding-bottom: -17px;
    })";
        const char* border = R"(
    QPushButton {
        border: 1px solid #ADADAD;
        border-radius: 2px;
    })";

    QString qss;
    if (m_dark)
    {
        qss = QString(background);
        ui->okButton->setFocusPolicy(Qt::NoFocus);
        ui->aboutQtButton->setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        qss = QString(background) + QString(pressed);
        ui->aboutQtButton->setStyleSheet(QString(border) + QString(pressed));
    }
    ui->okButton->setStyleSheet(qss);
}

void AboutDialog::AboutQt()
{
    auto w = new QWidget(this);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowIcon(QIcon(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));
    QMessageBox::aboutQt(w);
    w->deleteLater();
}

void ShowAboutDialog(
    _In_ HWND hParent)
{
    QWidget* qWidget = QWidget::find((WId)hParent);
    if (qWidget)
    {
        AboutDialog dlg(qWidget);
        dlg.exec();
    }
}