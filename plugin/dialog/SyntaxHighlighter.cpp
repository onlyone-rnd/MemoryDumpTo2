#include "SyntaxHighlighter.h"
#include <QtGui/QColor>
#include <QtGui/QFont>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent, bool darkTheme)
    : QSyntaxHighlighter(parent),
    m_darkTheme(darkTheme)
{
    setupFormats();
    rebuild();
}

void SyntaxHighlighter::setLanguage(SyntaxLanguage language)
{
    if (m_language == language)
        return;

    m_language = language;
    rebuild();
    rehighlight();
}

void SyntaxHighlighter::setDarkTheme(bool darkTheme)
{
    if (m_darkTheme == darkTheme)
        return;

    m_darkTheme = darkTheme;
    setupFormats();
    rebuild();
    rehighlight();
}

void SyntaxHighlighter::clearRules()
{
    m_rules.clear();
    m_singleLineCommentRules.clear();
    m_multiLineRules.clear();
}

void SyntaxHighlighter::setupFormats()
{
    if (m_darkTheme)
    {
        m_keywordFormat.setForeground(QColor("#569CD6"));
        m_typeFormat.setForeground(QColor("#4EC9A5"));
        m_commentFormat.setForeground(QColor("#6A9955"));
        m_stringFormat.setForeground(QColor("#CE9178"));
        m_numberFormat.setForeground(QColor("#B5CEA8"));
        m_functionFormat.setForeground(QColor("#DCDCAA"));
        m_preprocessorFormat.setForeground(QColor("#C586C0"));
        m_directiveFormat.setForeground(QColor("#C586C0"));
        m_registerFormat.setForeground(QColor("#9CDCFE"));
        m_decoratorFormat.setForeground(QColor("#D7BA7D"));
    }
    else
    {
        m_keywordFormat.setForeground(QColor("#0000CC"));
        m_typeFormat.setForeground(QColor("#267799"));
        m_commentFormat.setForeground(QColor("#008000"));
        m_stringFormat.setForeground(QColor("#A31515"));
        m_numberFormat.setForeground(QColor("#000000"));
        m_functionFormat.setForeground(QColor("#795E26"));
        m_preprocessorFormat.setForeground(QColor("#AF00DB"));
        m_directiveFormat.setForeground(QColor("#AF00DB"));
        m_registerFormat.setForeground(QColor("#0451A5"));
        m_decoratorFormat.setForeground(QColor("#B26700"));
    }
}

void SyntaxHighlighter::addRule(
    const QString& pattern,
    const QTextCharFormat& format,
    QRegularExpression::PatternOptions options)
{
    m_rules.push_back({ QRegularExpression(pattern, options), format });
}

void SyntaxHighlighter::addSingleLineCommentRule(
    const QString& pattern,
    const QTextCharFormat& format,
    QRegularExpression::PatternOptions options)
{
    m_singleLineCommentRules.push_back({ QRegularExpression(pattern, options), format });
}

void SyntaxHighlighter::addKeywordRules(
    const QStringList& keywords,
    const QTextCharFormat& format,
    QRegularExpression::PatternOptions options)
{
    for (const QString& kw : keywords)
    {
        const QString pattern =
            QStringLiteral(R"(\b%1\b)").arg(QRegularExpression::escape(kw));
        addRule(pattern, format, options);
    }
}

void SyntaxHighlighter::rebuild()
{
    clearRules();

    switch (m_language)
    {
        case SyntaxLanguage::Masm:
            buildMasmRules();
            break;
        case SyntaxLanguage::Nasm:
            buildNasmRules();
            break;
        case SyntaxLanguage::Fasm:
            buildFasmRules();
            break;
        case SyntaxLanguage::Gas:
            buildGasRules();
            break;
        case SyntaxLanguage::Cpp:
            buildCppRules();
            break;
        case SyntaxLanguage::CSharp:
            buildCSharpRules();
            break;
        case SyntaxLanguage::Python:
            buildPythonRules();
            break;
        case SyntaxLanguage::Delphi:
            buildDelphiRules();
            break;
        case SyntaxLanguage::NodeJs:
            buildNodeJsRules();
            break;
        case SyntaxLanguage::None:
        default:
            break;
    }
}

void SyntaxHighlighter::buildMasmRules()
{
    addRule(R"((?<!\w)\.data\b)",
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addKeywordRules(
        { "BYTE", "WORD", "DWORD", "QWORD" },
        m_typeFormat,
        QRegularExpression::CaseInsensitiveOption);

    addSingleLineCommentRule(R"(;[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b[0-9A-Fa-f]+h\b)", m_numberFormat, QRegularExpression::CaseInsensitiveOption);
    addRule(R"(\b\d+\b)", m_numberFormat);
}

void SyntaxHighlighter::buildNasmRules()
{
    addKeywordRules(
        { "section" },
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addRule(R"((?<!\w)\.data\b)",
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addKeywordRules(
        { "db", "dw", "dd", "dq" },
        m_typeFormat,
        QRegularExpression::CaseInsensitiveOption);

    addSingleLineCommentRule(R"(;[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b[0-9A-Fa-f]+h\b)", m_numberFormat, QRegularExpression::CaseInsensitiveOption);
    addRule(R"(\b\d+\b)", m_numberFormat);
}

void SyntaxHighlighter::buildFasmRules()
{
    addKeywordRules(
        { "section" },
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addRule(R"(\bdata\s+readable\s+writeable\b)",
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addKeywordRules(
        { "db", "dw", "dd", "dq" },
        m_typeFormat,
        QRegularExpression::CaseInsensitiveOption);

    addSingleLineCommentRule(R"(;[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b[0-9A-Fa-f]+h\b)", m_numberFormat, QRegularExpression::CaseInsensitiveOption);
    addRule(R"(\b\d+\b)", m_numberFormat);
}

void SyntaxHighlighter::buildGasRules()
{
    addRule(R"((?<!\w)\.section\b)",
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addRule(R"((?<!\w)\.data\b)",
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addRule(R"((?<!\w)\.(byte|word|long|quad)\b)",
        m_typeFormat,
        QRegularExpression::CaseInsensitiveOption);

    addRule(R"((?<!\w)\$-?(?:0[xX][0-9A-Fa-f]+|\d+)\b)", m_numberFormat);
    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b\d+\b)", m_numberFormat);

    addSingleLineCommentRule(R"(#[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);
}

void SyntaxHighlighter::buildCppRules()
{
    addKeywordRules(
        { "static", "const" },
        m_keywordFormat);

    addSingleLineCommentRule(R"(//[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b\d+(?:\.\d+)?(?:[eE][+-]?\d+)?\b)", m_numberFormat);

    addRule(R"(\b[A-Za-z_]\w*(?=\s*\())", m_functionFormat);

    addKeywordRules(
        { "uint8_t", "uint16_t", "uint32_t", "uint64_t" },
        m_typeFormat);

    m_multiLineRules.push_back({
    QRegularExpression(R"(/\*)"),
    QRegularExpression(R"(\*/)"),
    m_commentFormat,
    1
    });
}

void SyntaxHighlighter::buildCSharpRules()
{
    addKeywordRules(
        { "static", "readonly", "new" },
        m_keywordFormat);

    addSingleLineCommentRule(R"(//[^\n]*)", m_commentFormat);
    addRule(R"(@\"(?:[^\"]|\"\")*\")", m_stringFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b\d+\b)", m_numberFormat);

    addRule(R"(\b[A-Za-z_]\w*(?=\s*\())", m_functionFormat);

    addKeywordRules(
        { "byte", "ushort", "uint", "ulong" },
        m_typeFormat);

    m_multiLineRules.push_back({
    QRegularExpression(R"(/\*)"),
    QRegularExpression(R"(\*/)"),
    m_commentFormat,
    2
    });
}

void SyntaxHighlighter::buildPythonRules()
{
    addKeywordRules(
        { "from", "import", "ctypes" },
        m_keywordFormat);

    addSingleLineCommentRule(R"(#[^\n]*)", m_commentFormat);
    addRule(R"(([rRuUbBfF]{0,2}"(\\.|[^"\\])*"))", m_stringFormat);
    addRule(R"(([rRuUbBfF]{0,2}'(\\.|[^'\\])*'))", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+\b)", m_numberFormat);
    addRule(R"(\b\d+(?:\.\d+)?(?:[eE][+-]?\d+)?j?\b)", m_numberFormat);

    addKeywordRules(
        { "c_uint8", "c_uint16", "c_uint32", "c_uint64" },
        m_typeFormat);

    m_multiLineRules.push_back({
    QRegularExpression(R"(""")"),
    QRegularExpression(R"(""")"),
    m_stringFormat,
    3
    });

    m_multiLineRules.push_back({
    QRegularExpression(R"(''')"),
    QRegularExpression(R"(''')"),
    m_stringFormat,
    4
    });
}

void SyntaxHighlighter::buildDelphiRules()
{
    addKeywordRules(
        { "const", "array", "of" },
        m_keywordFormat,
        QRegularExpression::CaseInsensitiveOption);

    addSingleLineCommentRule(R"(//[^\n]*)", m_commentFormat);
    addRule(R"('(?:''|[^'])*')", m_stringFormat);

    addRule(R"(\$[0-9A-Fa-f]+)", m_numberFormat);
    addRule(R"(\b\d+(?:\.\d+)?\b)", m_numberFormat);

    addRule(R"(\b[A-Za-z_]\w*(?=\s*\())", m_functionFormat);

    addKeywordRules(
        { "Byte", "Word", "DWord", "QWord" },
        m_typeFormat,
        QRegularExpression::CaseInsensitiveOption);

    m_multiLineRules.push_back({
    QRegularExpression(R"(\{)"),
    QRegularExpression(R"(\})"),
    m_commentFormat,
    5
    });

    m_multiLineRules.push_back({
    QRegularExpression(R"(\(\*)"),
    QRegularExpression(R"(\*\))"),
    m_commentFormat,
    6
    });
}

void SyntaxHighlighter::buildNodeJsRules()
{
    addKeywordRules(
        { "const", "new" },
        m_keywordFormat);

    addSingleLineCommentRule(R"(//[^\n]*)", m_commentFormat);
    addRule(R"("(\\.|[^"\\])*")", m_stringFormat);
    addRule(R"('(\\.|[^'\\])*')", m_stringFormat);
    addRule(R"(`[^`]*`)", m_stringFormat);

    addRule(R"(\b0[xX][0-9A-Fa-f]+n?\b)", m_numberFormat);
    addRule(R"(\b\d+n\b)", m_numberFormat);
    addRule(R"(\b\d+(?:\.\d+)?(?:[eE][+-]?\d+)?\b)", m_numberFormat);

    addRule(R"(\b[A-Za-z_$][A-Za-z0-9_$]*(?=\s*\())", m_functionFormat);

    addKeywordRules(
        { "Uint8Array", "Uint16Array", "Uint32Array", "BigUint64Array" },
        m_typeFormat);

    m_multiLineRules.push_back({
    QRegularExpression(R"(/\*)"),
    QRegularExpression(R"(\*/)"),
    m_commentFormat,
    7
    });
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const auto& rule : m_rules)
    {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext())
        {
            const auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    for (const auto& rule : m_singleLineCommentRules)
    {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext())
        {
            const auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    for (const auto& rule : m_multiLineRules)
    {
        int startIndex = -1;
        int startLength = 0;
        bool continuing = (previousBlockState() == rule.state);

        if (continuing)
            startIndex = 0;
        else
        {
            const auto startMatch = rule.start.match(text);
            if (startMatch.hasMatch())
            {
                startIndex = startMatch.capturedStart();
                startLength = startMatch.capturedLength();
            }
        }

        while (startIndex >= 0)
        {
            const int searchFrom = continuing ? startIndex : (startIndex + qMax(1, startLength));
            const auto endMatch = rule.end.match(text, searchFrom);
            const int endIndex = endMatch.hasMatch() ? endMatch.capturedStart() : -1;

            int length = 0;
            if (endIndex < 0)
            {
                setCurrentBlockState(rule.state);
                length = text.length() - startIndex;
            }
            else
                length = endIndex - startIndex + endMatch.capturedLength();

            setFormat(startIndex, length, rule.format);

            const auto nextStart = rule.start.match(text, startIndex + length);
            if (!nextStart.hasMatch())
                break;

            startIndex = nextStart.capturedStart();
            startLength = nextStart.capturedLength();
            continuing = false;
        }
    }
}