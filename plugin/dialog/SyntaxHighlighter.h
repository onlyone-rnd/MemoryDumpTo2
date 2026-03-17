#pragma once

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>
#include <QtCore/QRegularExpression>
#include <QtCore/QVector>
#include <QtCore/QStringList>

enum class SyntaxLanguage : int
{
    None = 0,
    Masm,
    Nasm,
    Fasm,
    Gas,
    Cpp,
    CSharp,
    Python,
    Delphi,
    NodeJs
};

class SyntaxHighlighter final : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighlighter(QTextDocument* parent = nullptr, bool darkTheme = true);

    void setLanguage(SyntaxLanguage language);
    SyntaxLanguage language() const noexcept { return m_language; }

    void setDarkTheme(bool darkTheme);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightRule
    {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };

    struct MultiLineRule
    {
        QRegularExpression start;
        QRegularExpression end;
        QTextCharFormat    format;
        int                state = 0;
    };

private:
    void rebuild();
    void clearRules();
    void setupFormats();

    void addRule(
        const QString& pattern,
        const QTextCharFormat& format,
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption);

    void addSingleLineCommentRule(
        const QString& pattern,
        const QTextCharFormat& format,
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption);

    void addKeywordRules(
        const QStringList& keywords,
        const QTextCharFormat& format,
        QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption);

    void buildMasmRules();
    void buildNasmRules();
    void buildFasmRules();
    void buildGasRules();
    void buildCppRules();
    void buildCSharpRules();
    void buildPythonRules();
    void buildDelphiRules();
    void buildNodeJsRules();

private:
    QVector<HighlightRule> m_rules;
    QVector<HighlightRule> m_singleLineCommentRules;
    QVector<MultiLineRule> m_multiLineRules;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_preprocessorFormat;
    QTextCharFormat m_directiveFormat;
    QTextCharFormat m_registerFormat;
    QTextCharFormat m_decoratorFormat;

    SyntaxLanguage m_language = SyntaxLanguage::None;
    bool m_darkTheme = true;
};