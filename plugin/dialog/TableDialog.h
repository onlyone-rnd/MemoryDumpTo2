#pragma once

#include "../global.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QApplication>
#include <QTimer>
#include <QtGui/QFont>
#include <QtGui/QStandardItemModel>
#include "CodeEditor.h"
#include "ComboBoxEx.h"
#include "DumpFormatter.h"
#include "SyntaxHighlighter.h"

namespace Ui { class TableDialog; }

class TableDialog final : public QDialog {

    Q_OBJECT

public:
    explicit TableDialog(
        QWidget* qWidget,
        MEMDUMP* pMemDump);

    ~TableDialog() override;

private:
    enum {
        UserIdRole = Qt::UserRole + 1
    };

    void setDialogParam();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:

    void languageChanged(int index);
    void dataWidthChanged(int index);
    void endiannessChanged(int index);
    void perLineChanged(int index);
    void symbolNameChanged(const QString& text);
    void saveDialogSizeDeferred();

private:
    void InitializeFormatControls();
    void restoreDialogSize();
    void saveDialogSizeNow() const;
    void deleteDialogSizeConfig() const;

    static QString getPluginConfigPath();
    static bool readSavedSize(const QString& filePath, QSize& outSize);
    static bool writeSavedSize(const QString& filePath, const QSize& size);

    int CurrentPerLine() const;
    DataWidth CurrentDataWidth() const;
    LanguageId CurrentLanguageId() const;
    Endianness CurrentEndianness() const;
    QString CurrentSymbolName() const;
    int FindComboDataIndex(const QComboBox* combo, int value) const;

    void copyClicked();
    void saveClicked();

    void fillLanguageComboBox(ComboBoxEx* combo);
    void fillEndiannessGroupBox(QComboBox* combo);
    void fillPerLineComboBox(QComboBox* combo);
    void setSymbolName(QLineEdit* edit, const QString& text);
    void setDataWidthShortcut(DataWidth width);
    void setupHotkeys();
    void SaveTableToFile(const QString baseName, const QString& text);
    void RebuildWindowTitle(LanguageId langId);
    void RebuildDataWidthCombo(QComboBox* combo, LanguageId langId, DataWidth prefWidth);
    void RebuildDataWidthForEndianness(QComboBox* combo, DataWidth width);
    void RebuildItemsPerLine(QComboBox* combo, LanguageId langId, DataWidth width);

    void ApplyLanguageDefaults(DumpFormatOptions& fmt);
    void UpdateFormattedDump();

    SyntaxLanguage ToSyntaxLanguage(LanguageId id);

private:
    Ui::TableDialog* ui = nullptr;
    MEMDUMP* memDump = nullptr;
    bool is_dark = false;

    QSize  m_defaultSize;
    QTimer m_saveSizeTimer;
    bool   m_initialized = false;
    bool   m_restoringSize = false;
};

Q_DECLARE_METATYPE(LanguageId);
Q_DECLARE_METATYPE(Endianness);
Q_DECLARE_METATYPE(DataWidth);

bool ShowTableDialog(
    _In_ HWND hParent,
    _In_ MEMDUMP* pMemDump);