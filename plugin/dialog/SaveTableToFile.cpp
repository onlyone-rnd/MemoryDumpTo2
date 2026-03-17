
#include "TableDialog.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#if defined(_DEBUG)
#include <QDebug>
#endif

void TableDialog::SaveTableToFile(
    const QString baseName,
    const QString& text)
{
    auto filterForLanguage = [](LanguageId id) -> QString
    {
        switch (id)
        {
            case LanguageId::Masm:
            case LanguageId::Nasm:
            case LanguageId::Fasm:
                return QStringLiteral(
                    "Assembly source files (*.asm);;"
                    "Assembly include files (*.inc);;"
                    "All files (*.*)");
            case LanguageId::Gas:
                return QStringLiteral(
                    "GAS source files (*.s);;"
                    "All files (*.*)");
            case LanguageId::Cpp:
                return QStringLiteral(
                    "C++ source files (*.cpp);;"
                    "C header files (*.h);;"
                    "C++ header files (*.hpp);;"
                    "All files (*.*)");
            case LanguageId::CSharp:
                return QStringLiteral(
                    "C# source files (*.cs);;"
                    "All files (*.*)");
            case LanguageId::Python:
                return QStringLiteral(
                    "Python files (*.py);;"
                    "All files (*.*)");
            case LanguageId::Delphi:
                return QStringLiteral(
                    "Delphi unit files (*.pas);;"
                    "Delphi project files (*.dpr);;"
                    "All files (*.*)");
            case LanguageId::NodeJs:
                return QStringLiteral(
                    "JavaScript files (*.js);;"
                    "Module files (*.mjs);;"
                    "CommonJS files (*.cjs);;"
                    "All files (*.*)");
            default:
                return QStringLiteral("Text files (*.txt);;All files (*.*)");
        }
    };
    const QString filter = filterForLanguage(CurrentLanguageId());

    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (desktop.isEmpty())
        desktop = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    const QString suggested =
        QDir::toNativeSeparators(QDir(desktop).filePath(baseName));

    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Save Table File"),
        suggested,
        filter);

    if (filePath.isEmpty())
        return;

    QFileInfo fi(filePath);
    if (fi.suffix().isEmpty())
        filePath += QStringLiteral(".txt");

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::warning(
            this,
            tr("Save error"),
            tr("Failed to save file:\n%1").arg(QDir::toNativeSeparators(filePath)));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << text;
    file.close();
}