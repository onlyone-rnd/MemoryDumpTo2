
#include "utils.h"
#include <QDir>
#include <QFile>
#include <QtGui/QIcon>
#include <QStandardPaths>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

static QByteArray qt_resource;

bool _plugin_qt_getimagerefromsource(
    _In_ const char* url,
    _Out_ void** data,
    _Out_ size_t* size)
{
    if (!data || !size)
        return false;

    *data = nullptr;
    *size = 0;

    QFile f(url);
    if (!f.open(QIODevice::ReadOnly))
        return FALSE;

    qt_resource = f.readAll();
    if (!qt_resource.isEmpty())
    {
        *data = (void*)qt_resource.constData();
        *size = (size_t)qt_resource.size();
    }
    return !qt_resource.isEmpty();
}

bool _plugin_qt_getsavefilename(
    _In_ HWND hParent,
    _In_z_ const wchar_t* defaultName,
    _In_ uchar* memStart,
    _In_ uchar* memEnd,
    _Out_writes_z_(MAX_PATH) wchar_t* fileName)
{
    if (!fileName)
        return false;

    fileName[0] = L'\0';

    QWidget* qWidget = QWidget::find((WId)hParent);
    if (!qWidget)
        return false;

    const QString baseName =
        QString::fromWCharArray(defaultName)
        .arg(qulonglong(memStart), 0, 16)
        .arg(qulonglong(memEnd), 0, 16);

    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (desktop.isEmpty())
        desktop = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    const QString suggested =
        QDir::toNativeSeparators(QDir(desktop).filePath(baseName));

    const QString filter = QStringLiteral("bin files (*.bin);;All files (*.*)");

    QString filePath = QFileDialog::getSaveFileName(
        qWidget,
        QStringLiteral("Save Memory Dump File"),
        suggested,
        filter);

    if (filePath.isEmpty())
        return true;

    const QString native = QDir::toNativeSeparators(filePath);
    wcscpy(fileName, reinterpret_cast<const wchar_t*>(native.utf16()));
    return true;
}

void _plugin_qt_lasterrormessage(
    _In_ HWND hParent,
    _In_opt_ ulong error)
{
    wchar_t* Message = nullptr;

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr,
        error ? error : GetLastError(),
        MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
        (wchar_t*)&Message,
        0,
        nullptr);
    if (Message)
    {
        QWidget* qWidget = QWidget::find((WId)hParent);
        if (qWidget)
        {
            QMessageBox mb(qWidget);
            mb.setIcon(QMessageBox::Critical);
            mb.setWindowTitle(QStringLiteral("[ERROR]"));
            mb.setTextFormat(Qt::PlainText);
            mb.setText(QString::fromWCharArray(Message));
            mb.setStandardButtons(QMessageBox::Ok);
            mb.setWindowIcon(QIcon(":/ico/main.ico"));
            mb.setMinimumWidth(400);
            mb.setMinimumHeight(200);
            mb.exec();
        }
        LocalFree(Message);
    }
}

bool isDark()
{
    char* theme = (char*)malloc(MAX_SETTING_SIZE);
    if (!theme)
        return FALSE;

    bool res = BridgeSettingGet("Theme", "Selected", theme);
    if (res)
        res = !_stricmp(theme, "Dark") ? TRUE : FALSE;
    free(theme);
    return res;
}