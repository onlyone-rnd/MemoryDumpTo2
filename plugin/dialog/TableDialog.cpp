
#include "TableDialog.h"
#include "ui_TableDialog.h"
#include "../res/version.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QRegularExpression>
#include <QtGui/QClipboard>
#include <QtGui/QWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtGui/QRegularExpressionValidator>

#if defined(_DEBUG)
#include <QDebug>
#endif

#pragma warning(disable : 26813)

bool isDark();

TableDialog::TableDialog(
    QWidget* qWidget,
    MEMDUMP* pMemDump)
    : QDialog(qWidget),
    ui(new Ui::TableDialog),
    memDump(pMemDump)
{
    ui->setupUi(this);

    m_defaultSize = size();
    m_saveSizeTimer.setSingleShot(true);
    m_saveSizeTimer.setInterval(250);

    is_dark = isDark();

    setModal(true);
    setWindowFlags(windowFlags() &
        ~Qt::WindowContextHelpButtonHint);
    setDialogParam();

    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TableDialog::languageChanged);
    connect(ui->dataWidthComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TableDialog::dataWidthChanged);
    connect(ui->endiannessComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TableDialog::endiannessChanged);
    connect(ui->perLineComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TableDialog::perLineChanged);
    connect(ui->symbolNameLineEdit, &QLineEdit::textChanged, this, &TableDialog::symbolNameChanged);
    connect(ui->copyButton, &QPushButton::clicked, this, &TableDialog::copyClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &TableDialog::saveClicked);
    connect(&m_saveSizeTimer, &QTimer::timeout, this, &TableDialog::saveDialogSizeDeferred);

    InitializeFormatControls();
    restoreDialogSize();
    m_initialized = true;
}

TableDialog::~TableDialog()
{
    delete ui;
}

void TableDialog::setDialogParam()
{
    QFont font("Lucida Console", 8);

    ui->arrayTextEdit->setFont(font);
    ui->arrayTextEdit->setReadOnly(true);
    ui->arrayTextEdit->setFocusPolicy(Qt::StrongFocus);

    ui->arrayTextEdit->setLineNumberColors(QColor("#C9C9C9"), QColor("#0078D7"));

    char* buf = (char*)malloc(MAX_SETTING_SIZE);
    if (!buf)
        return;

    if (BridgeSettingGet("Colors", "AbstractTableViewSeparatorColor", buf))
    {
        QString color = QString::fromLatin1(buf);
        if (!color.startsWith('#'))
            color.prepend('#');

        const char* textEdit = R"(
        QPlainTextEdit#arrayTextEdit {
            border-radius: 2px;
            border: 1px solid %1;
        }
        QPlainTextEdit#arrayTextEdit:hover {
            border: 1px solid %1;
        })";
        ui->arrayTextEdit->setFrameStyle(QFrame::NoFrame);
        ui->arrayTextEdit->setStyleSheet(QString(textEdit).arg(color));

        const char* lineEditDefault = R"(
        QLineEdit#symbolNameLineEdit {
            border-radius: 2px;
            background-color: #FFF8F0;
            border: 1px solid %1;
        }
        QLineEdit#symbolNameLineEdit:hover {
            border: 1px solid %1;
        })";

        const char* lineEditDark = R"(
        QLineEdit#symbolNameLineEdit {
            border-radius: 2px;
            border: 1px solid %1;
        }
        QLineEdit#symbolNameLineEdit:hover {
            border: 1px solid %1;
        })";

        ui->symbolNameLineEdit->setFrame(QFrame::NoFrame);
        if (is_dark)
            ui->symbolNameLineEdit->setStyleSheet(QString(lineEditDark).arg(color));
        else
            ui->symbolNameLineEdit->setStyleSheet(QString(lineEditDefault).arg(color));

        const char* groupBox = R"(
        QGroupBox#%1 {
            border: 1px solid %2;
            border-radius: 2px;
            margin-top: 10px;
            padding: 5px;
            min-width: %3;
        }
        QGroupBox#%1::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 5px;
            padding: 0 5px;
            margin-top: -1px;
            padding-left: 2px;
            padding-right: 2px;
        })";
        ui->languageGroupBox->setStyleSheet(QString(groupBox).arg("languageGroupBox", color, "208px"));
        if (is_dark)
            ui->dataWidthGroupBox->setStyleSheet(QString(groupBox).arg("dataWidthGroupBox", color, "130px"));
        else
            ui->dataWidthGroupBox->setStyleSheet(QString(groupBox).arg("dataWidthGroupBox", color, "132px"));
            ui->endiannessGroupBox->setStyleSheet(QString(groupBox).arg("endiannessGroupBox", color, "114px"));
            ui->perLineGroupBox->setStyleSheet(QString(groupBox).arg("perLineGroupBox", color, "58px"));
            ui->actionsGroupBox->setStyleSheet(QString(groupBox).arg("actionsGroupBox", color, "228px"));

        if (is_dark)
        {
            const char* comboBox = R"(
            QComboBox#%1 {
                border: 1px solid %2;
                border-radius: 2px;
                padding: 5px;
                min-width: %3;
                max-height: 17px;
            })";
            ui->languageComboBox->setStyleSheet(QString(comboBox).arg("languageComboBox", color, "176px"));
            ui->dataWidthComboBox->setStyleSheet(QString(comboBox).arg("dataWidthComboBox", color, "93px"));
            ui->endiannessComboBox->setStyleSheet(QString(comboBox).arg("endiannessComboBox", color, "80px"));
            ui->perLineComboBox->setStyleSheet(QString(comboBox).arg("perLineComboBox", color, "20px"));

            const char* pressed = R"(
            QPushButton#%1 {
                border: 1px solid %2;
                color: #FFFFFF;
                min-height: 17px;
                max-height: 17px;
                background-image: url(:/png/images/copy.png);
                background-repeat: no-repeat;
                background-position: center;
            })";
            ui->saveButton->setStyleSheet(QString(pressed).arg("saveButton", color));
            ui->copyButton->setStyleSheet(QString(pressed).arg("copyButton", color));
        }
        else
        {
            const char* comboBox = R"(
            QComboBox#%1 {
                padding: 5px;
                min-width: %2;
                max-height: 17px;
            })";
            ui->languageComboBox->setStyleSheet(QString(comboBox).arg("languageComboBox", "176px"));
            ui->dataWidthComboBox->setStyleSheet(QString(comboBox).arg("dataWidthComboBox", "93px"));
            ui->endiannessComboBox->setStyleSheet(QString(comboBox).arg("endiannessComboBox", "80px"));
            ui->perLineComboBox->setStyleSheet(QString(comboBox).arg("perLineComboBox", "20px"));

            const char* pressed = R"(
            QPushButton#%1 {
                border: 1px solid %2;
                color: #FFFFFF;
                min-height: 27px;
                max-height: 27px;
                background-image: url(:/png/images/copy.png);
                background-repeat: no-repeat;
                background-position: center;
            }
            QPushButton#%1:pressed {
                padding-top: -15px;
                padding-bottom: -17px;
            })";
            ui->saveButton->setStyleSheet(QString(pressed).arg("saveButton", color));
            ui->copyButton->setStyleSheet(QString(pressed).arg("copyButton", color));
        }
    }
    free(buf);
}

SyntaxLanguage TableDialog::ToSyntaxLanguage(LanguageId id)
{
    switch (id)
    {
        case LanguageId::Masm:   return SyntaxLanguage::Masm;
        case LanguageId::Nasm:   return SyntaxLanguage::Nasm;
        case LanguageId::Fasm:   return SyntaxLanguage::Fasm;
        case LanguageId::Gas:    return SyntaxLanguage::Gas;
        case LanguageId::Cpp:    return SyntaxLanguage::Cpp;
        case LanguageId::CSharp: return SyntaxLanguage::CSharp;
        case LanguageId::Python: return SyntaxLanguage::Python;
        case LanguageId::Delphi: return SyntaxLanguage::Delphi;
        case LanguageId::NodeJs: return SyntaxLanguage::NodeJs;
        default:                 return SyntaxLanguage::None;
    }
}

void TableDialog::InitializeFormatControls()
{
    fillLanguageComboBox(ui->languageComboBox);
    fillEndiannessGroupBox(ui->endiannessComboBox);
    fillPerLineComboBox(ui->perLineComboBox);
    setSymbolName(ui->symbolNameLineEdit, "Table");
    setupHotkeys();

    LanguageId langId = CurrentLanguageId();
    RebuildDataWidthCombo(ui->dataWidthComboBox, langId, DataWidth::Byte);
    RebuildWindowTitle(langId);
    RebuildItemsPerLine(ui->perLineComboBox, langId, DataWidth::Byte);
    UpdateFormattedDump();
}

void TableDialog::setSymbolName(
    QLineEdit* edit,
    const QString& text)
{
    QSignalBlocker blocker(edit);
    edit->clear();
    edit->setMaxLength(128);
    static const QRegularExpression rx(QStringLiteral("[A-Za-z_][A-Za-z0-9_]{0,127}"));
    auto* validator = new QRegularExpressionValidator(rx, edit);
    edit->setValidator(validator);
    QFont font("Lucida Console", 8);
    edit->setFont(font);
    edit->setText(text);
}

void TableDialog::setDataWidthShortcut(DataWidth width)
{
    const int idx = FindComboDataIndex(
        ui->dataWidthComboBox, static_cast<int>(width));
    if (idx >= 0)
        ui->dataWidthComboBox->setCurrentIndex(idx);
}

void TableDialog::setupHotkeys()
{
    auto addShortcut = [this](const QString& seq, auto fn)
    {
        auto* sc = new QShortcut(QKeySequence(seq), this);
        sc->setContext(Qt::WindowShortcut);
        connect(sc, &QShortcut::activated, this, fn);
    };

    addShortcut(QStringLiteral("Ctrl+C"), [this]()
    {
        copyClicked();
    });

    addShortcut(QStringLiteral("Ctrl+S"), [this]()
    {
        saveClicked();
    });

    addShortcut(QStringLiteral("Alt+B"), [this]() { setDataWidthShortcut(DataWidth::Byte);  });
    addShortcut(QStringLiteral("Alt+W"), [this]() { setDataWidthShortcut(DataWidth::Word);  });
    addShortcut(QStringLiteral("Alt+D"), [this]() { setDataWidthShortcut(DataWidth::Dword); });
    addShortcut(QStringLiteral("Alt+Q"), [this]() { setDataWidthShortcut(DataWidth::Qword); });
}

void TableDialog::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);

    if (!m_initialized)
        return;
    if (m_restoringSize)
        return;
    if (isMaximized() || isFullScreen())
        return;
    m_saveSizeTimer.start();
}

QString TableDialog::getPluginConfigPath()
{
    wchar_t modulePath[MAX_PATH] = {};
    const DWORD len = ::GetModuleFileNameW(g_hModule, modulePath, MAX_PATH);
    if (!len || len >= MAX_PATH)
        return QString();

    const QString dllPath = QString::fromWCharArray(modulePath);
    const QFileInfo fi(dllPath);
    return QDir(fi.absolutePath()).filePath(QStringLiteral("MemoryDumpTo.cfg"));
}

bool TableDialog::readSavedSize(const QString& filePath, QSize& outSize)
{
    if (filePath.isEmpty() || !QFile::exists(filePath))
        return false;

    QSettings cfg(filePath, QSettings::IniFormat);

    bool okW = false;
    bool okH = false;

    const int w = cfg.value(QStringLiteral("Dialog/Width")).toInt(&okW);
    const int h = cfg.value(QStringLiteral("Dialog/Height")).toInt(&okH);

    if (!okW || !okH || w <= 0 || h <= 0)
        return false;

    outSize = QSize(w, h);
    return true;
}

bool TableDialog::writeSavedSize(const QString& filePath, const QSize& size)
{
    if (filePath.isEmpty())
        return false;

    QSettings cfg(filePath, QSettings::IniFormat);
    cfg.setValue(QStringLiteral("Dialog/Width"), size.width());
    cfg.setValue(QStringLiteral("Dialog/Height"), size.height());
    cfg.sync();

    return (cfg.status() == QSettings::NoError);
}

void TableDialog::restoreDialogSize()
{
    const QString cfgPath = getPluginConfigPath();
    if (cfgPath.isEmpty())
        return;

    QSize savedSize;
    if (!readSavedSize(cfgPath, savedSize))
    {
        QFile::remove(cfgPath);
        return;
    }

    if (savedSize == m_defaultSize)
    {
        QFile::remove(cfgPath);
        return;
    }

    savedSize.setWidth(qMax(savedSize.width(), minimumWidth()));
    savedSize.setHeight(qMax(savedSize.height(), minimumHeight()));

    if (QScreen* screen = windowHandle() ? windowHandle()->screen() : QGuiApplication::primaryScreen())
    {
        const QRect avail = screen->availableGeometry();
        savedSize.setWidth(qMin(savedSize.width(), avail.width()));
        savedSize.setHeight(qMin(savedSize.height(), avail.height()));
    }
    m_restoringSize = true;
    resize(savedSize);
    m_restoringSize = false;
}

void TableDialog::saveDialogSizeDeferred()
{
    saveDialogSizeNow();
}

void TableDialog::saveDialogSizeNow() const
{
    const QString cfgPath = getPluginConfigPath();
    if (cfgPath.isEmpty())
        return;

    const QSize currentSize = size();
    if (currentSize == m_defaultSize)
    {
        QFile::remove(cfgPath);
        return;
    }
    writeSavedSize(cfgPath, currentSize);
}

void TableDialog::deleteDialogSizeConfig() const
{
    const QString cfgPath = getPluginConfigPath();
    if (!cfgPath.isEmpty())
        QFile::remove(cfgPath);
}

void TableDialog::fillLanguageComboBox(ComboBoxEx* combo)
{
    QSignalBlocker blocker(combo);
    combo->clear();

    auto* model = qobject_cast<QStandardItemModel*>(combo->model());
    if (!model)
        return;

    auto addHeader = [&](LanguageId id)
    {
        const auto* lang = DumpFormatter::FindLanguage(id);
        if (!lang)
            return;

        combo->insertSeparator(combo->count());
        combo->addItem(QString::fromLatin1(lang->groupName));
        const int row = combo->count() - 1;

        if (QStandardItem* item = model->item(row))
        {
            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
            item->setFlags(Qt::ItemIsEnabled);
            if (is_dark)
                item->setData(QBrush(QColor("#009AF9")), Qt::ForegroundRole);
            else
                item->setData(QBrush(QColor("#0078D7")), Qt::ForegroundRole);
            item->setData(QVariant(), UserIdRole);
        }
        combo->insertSeparator(combo->count());
    };

    auto addLanguage = [&](LanguageId id)
    {
        const auto* lang = DumpFormatter::FindLanguage(id);
        if (!lang)
            return;

        combo->addItem(QString::fromLatin1(lang->displayName));
        const int row = combo->count() - 1;
        combo->setItemData(row, QVariant::fromValue(id), UserIdRole);
    };

    addHeader(LanguageId::Masm);
    addLanguage(LanguageId::Masm);
    addLanguage(LanguageId::Nasm);
    addLanguage(LanguageId::Fasm);
    addLanguage(LanguageId::Gas);
    addHeader(LanguageId::Cpp);
    addLanguage(LanguageId::Cpp);
    addLanguage(LanguageId::CSharp);
    addHeader(LanguageId::Python);
    addLanguage(LanguageId::Python);
    addHeader(LanguageId::Delphi);
    addLanguage(LanguageId::Delphi);
    addLanguage(LanguageId::NodeJs);

    const int firstIndex = FindComboDataIndex(combo, static_cast<int>(LanguageId::Masm));
    combo->setCurrentIndex(firstIndex >= 0 ? firstIndex : 0);
}

void TableDialog::fillEndiannessGroupBox(QComboBox* combo)
{
    QSignalBlocker blocker(combo);
    combo->clear();

    auto addByteOrder = [&](const QString& text, Endianness order)
    {
        combo->addItem(text);
        const int row = combo->count() - 1;
        combo->setItemData(row, QVariant::fromValue(order), UserIdRole);
    };
    addByteOrder(QStringLiteral("LittleEndian"), Endianness::LittleEndian);
    addByteOrder(QStringLiteral("BigEndian"), Endianness::BigEndian);

    const int firstIndex = FindComboDataIndex(combo, static_cast<int>(Endianness::LittleEndian));
    combo->setCurrentIndex(firstIndex >= 0 ? firstIndex : 0);
}

void TableDialog::RebuildDataWidthForEndianness(
    QComboBox* combo,
    DataWidth width)
{
    if (width == DataWidth::Byte)
    {
        const int firstIndex = FindComboDataIndex(combo, static_cast<int>(Endianness::LittleEndian));
        combo->setCurrentIndex(firstIndex >= 0 ? firstIndex : 0);
    }
}

void TableDialog::RebuildDataWidthCombo(
    QComboBox* combo,
    LanguageId langId,
    DataWidth prefWidth)
{
    QSignalBlocker blocker(combo);
    combo->clear();

    const auto* lang = DumpFormatter::FindLanguage(langId);
    if (!lang)
        return;

    auto addDataType = [&](const QString& text, DataWidth width)
        {
            combo->addItem(text);
            const int row = combo->count() - 1;
            combo->setItemData(row, QVariant::fromValue(width), UserIdRole);
        };
    addDataType(QString::fromLatin1(lang->byteType), DataWidth::Byte);
    addDataType(QString::fromLatin1(lang->wordType), DataWidth::Word);
    addDataType(QString::fromLatin1(lang->dwordType), DataWidth::Dword);
    addDataType(QString::fromLatin1(lang->qwordType), DataWidth::Qword);

    const int firstIndex = FindComboDataIndex(combo, static_cast<int>(prefWidth));
    combo->setCurrentIndex(firstIndex >= 0 ? firstIndex : 0);
}

void TableDialog::RebuildWindowTitle(LanguageId langId)
{
    const auto* lang = DumpFormatter::FindLanguage(langId);
    if (!lang)
        return;

    QString title = QStringLiteral("%1 - %2 [ %3 ]")
        .arg(QString::fromWCharArray(PRODUCT_NAME_STRING))
        .arg(QString::fromLatin1(lang->groupName))
        .arg(QString::fromLatin1(lang->displayName));
    setWindowTitle(title);
}

void TableDialog::RebuildItemsPerLine(
    QComboBox* combo,
    LanguageId langId,
    DataWidth width)
{
    int idx = 0;

    switch (langId)
    {
        case LanguageId::Masm:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 14);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Nasm:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 14);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Fasm:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 14);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Gas:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Cpp:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::CSharp:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Python:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::Delphi:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 14);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 5);
            break;
        }
        case LanguageId::NodeJs:
        {
            if (width == DataWidth::Byte)
                idx = FindComboDataIndex(combo, 16);
            else if (width == DataWidth::Word)
                idx = FindComboDataIndex(combo, 12);
            else if (width == DataWidth::Dword)
                idx = FindComboDataIndex(combo, 8);
            else if (width == DataWidth::Qword)
                idx = FindComboDataIndex(combo, 4);
            break;
        }
    }
    combo->setCurrentIndex(idx);
}

QString TableDialog::CurrentSymbolName() const
{
    const QString text = ui->symbolNameLineEdit->text().trimmed();
    return text.isEmpty() ? QStringLiteral("Table") : text;
}

int TableDialog::CurrentPerLine() const
{
    const QVariant v = ui->perLineComboBox->currentData(UserIdRole);
    int n = v.toInt();
    return qBound(4, n, 16);
}

DataWidth TableDialog::CurrentDataWidth() const
{
    const QVariant v = ui->dataWidthComboBox->currentData(UserIdRole);
    return static_cast<DataWidth>(v.toInt());
}

Endianness TableDialog::CurrentEndianness() const
{
    const QVariant v = ui->endiannessComboBox->currentData(UserIdRole);
    return static_cast<Endianness>(v.toInt());
}

LanguageId TableDialog::CurrentLanguageId() const
{
    const QVariant v = ui->languageComboBox->currentData(UserIdRole);
    return static_cast<LanguageId>(v.toInt());
}

int TableDialog::FindComboDataIndex(const QComboBox* combo, int value) const
{
    if (!combo)
        return -1;
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemData(i, UserIdRole).toInt() == value)
            return i;
    }
    return -1;
}

void TableDialog::fillPerLineComboBox(QComboBox* combo)
{
    QSignalBlocker blocker(combo);
    combo->clear();


    auto addPerLine = [&](const QString& text, int line)
    {
        combo->addItem(text);
        const int row = combo->count() - 1;
        combo->setItemData(row, static_cast<int>(line), UserIdRole);
    };

    for (int i = 4; i <= 16; ++i)
        addPerLine(QString::number(i), i);

    const int idx16 = FindComboDataIndex(combo, 16);
    combo->setCurrentIndex(idx16 >= 0 ? idx16 : 0);
}

void TableDialog::languageChanged(int index)
{
    Q_UNUSED(index);
    LanguageId langId = CurrentLanguageId();
    RebuildWindowTitle(langId);
    RebuildDataWidthCombo(ui->dataWidthComboBox, langId, CurrentDataWidth());
    RebuildItemsPerLine(ui->perLineComboBox, langId, CurrentDataWidth());
    UpdateFormattedDump();
}

void TableDialog::dataWidthChanged(int index)
{
    Q_UNUSED(index);
    RebuildDataWidthForEndianness(ui->endiannessComboBox, CurrentDataWidth());
    RebuildItemsPerLine(ui->perLineComboBox, CurrentLanguageId(), CurrentDataWidth());
    UpdateFormattedDump();
}

void TableDialog::endiannessChanged(int index)
{
    Q_UNUSED(index);

    RebuildDataWidthForEndianness(ui->endiannessComboBox, CurrentDataWidth());
    UpdateFormattedDump();
}

void TableDialog::perLineChanged(int index)
{
    Q_UNUSED(index);
    UpdateFormattedDump();
}

void TableDialog::symbolNameChanged(const QString& text)
{
    if (text.isEmpty())
        ui->arrayTextEdit->clear();
    else
        UpdateFormattedDump();
}

void TableDialog::copyClicked()
{
    if (!ui->arrayTextEdit->toPlainText().isEmpty())
        QApplication::clipboard()->setText(ui->arrayTextEdit->toPlainText());
}

void TableDialog::saveClicked()
{
    if (ui->arrayTextEdit->toPlainText().isEmpty())
        return;
    const QString text = ui->arrayTextEdit->toPlainText();
    if (text.isEmpty())
        return;

    SaveTableToFile(CurrentSymbolName(), text);
}

void TableDialog::ApplyLanguageDefaults(DumpFormatOptions& fmt)
{
    fmt.includeSection = false;
    fmt.padIncompleteTail = true;
    fmt.useCustomLiteralDecorators = false;
    fmt.forceUppercaseHex = false;
    fmt.uppercaseHex = true;

    fmt.cppStaticConst = true;
    fmt.csharpStaticReadonly = false;
    fmt.pythonIncludeCtypesImport = true;

    switch (fmt.languageId)
    {
        case LanguageId::Masm:
        {
            fmt.includeSection = true;
            break;
        }
        case LanguageId::Nasm:
        {
            fmt.includeSection = true;
            break;
        }
        case LanguageId::Fasm:
        {
            fmt.includeSection = true;
            break;
        }
        case LanguageId::Gas:
        {
            fmt.includeSection = true;
            break;
        }
        case LanguageId::Cpp:
        {
            fmt.cppStaticConst = true;
            break;
        }
        case LanguageId::CSharp:
        {
            fmt.csharpStaticReadonly = true;
            break;
        }
        case LanguageId::Python:
        {
            fmt.pythonIncludeCtypesImport = true;
            break;
        }
        case LanguageId::Delphi:
            break;

        case LanguageId::NodeJs:
            break;
    }
}

void TableDialog::UpdateFormattedDump()
{
    ui->arrayTextEdit->clear();
    
    DumpFormatOptions fmt;
    fmt.languageId = CurrentLanguageId();
    fmt.dataWidth = CurrentDataWidth();
    fmt.endianness = CurrentEndianness();
    fmt.elementsPerLine = CurrentPerLine();
    fmt.symbolName = CurrentSymbolName();

    ApplyLanguageDefaults(fmt);

    ui->arrayTextEdit->setSyntaxLanguage(ToSyntaxLanguage(fmt.languageId));

    const QString text = DumpFormatter::FormatDump(
        memDump->memBase,
        static_cast<int>(memDump->memSize),
        fmt);

    if (ui->arrayTextEdit->toPlainText() != text)
        ui->arrayTextEdit->setPlainText(text);
}

bool ShowTableDialog(
    _In_ HWND hParent,
    _In_ MEMDUMP* pMemDump)
{
    QWidget* qWidget = QWidget::find((WId)hParent);
    if (!qWidget)
        return false;

    TableDialog dlg(qWidget, pMemDump);
    dlg.exec();
    return true;
}
