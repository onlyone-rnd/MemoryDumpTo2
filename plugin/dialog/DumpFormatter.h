#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QtGlobal>

#pragma warning(disable : 26813)

enum class LanguageId : int {
    // Assemblers
    None = 0,
    Masm,
    Nasm,
    Fasm,
    Gas,
    // Native/Systems
    Cpp,
    CSharp,
    // Scripting/Tooling
    Python,
    // Advanced
    Delphi,
    NodeJs,
};

enum class SyntaxFamily : int {
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

enum class DataWidth : int {
    Byte = 1,
    Word = 2,
    Dword = 4,
    Qword = 8
};

enum class Endianness : int {
    LittleEndian = 1,
    BigEndian
};

struct LanguageDescriptor {
    LanguageId    id;
    SyntaxFamily  syntax;

    const char* displayName;
    const char* groupName;

    const char* byteType;
    const char* wordType;
    const char* dwordType;
    const char* qwordType;

    const char* defaultLiteralPrefix;
    const char* defaultLiteralSuffix;
    bool        uppercaseHex;
    bool        addLeadingZeroIfSuffixHex;

    int minDigitsByte;
    int minDigitsWord;
    int minDigitsDword;
    int minDigitsQword;

    const char* commentPrefix;
};

static const LanguageDescriptor g_LanguageDescriptors[] =
{
    {
        LanguageId::Masm, SyntaxFamily::Masm,
        "MASM/UASM/JWasm/ASMC", "Assemblers",
        "BYTE", "WORD", "DWORD", "QWORD",
        "0", "h", true, true,
        3, 4, 8, 16,
        ";"
    },
    {
        LanguageId::Nasm, SyntaxFamily::Nasm,
        "NASM/YASM", "Assemblers",
        "db", "dw", "dd", "dq",
        "0", "h", true, true,
        3, 4, 8, 16,
        ";"
    },
    {
        LanguageId::Fasm, SyntaxFamily::Fasm,
        "FASM", "Assemblers",
        "db", "dw", "dd", "dq",
        "0", "h", true, true,
        3, 4, 8, 16,
        ";"
    },
    {
        LanguageId::Gas, SyntaxFamily::Gas,
        "GAS/clang-as", "Assemblers",
        ".byte", ".word", ".long", ".quad",
        "0x", "", true, false,
        2, 4, 8, 16,
        "#"
    },

    {
        LanguageId::Cpp, SyntaxFamily::Cpp,
        "C/C++", "Native/Systems",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "0x", "", true, false,
        2, 4, 8, 16,
        "//"
    },
    {
        LanguageId::CSharp, SyntaxFamily::CSharp,
        "C#", "Native/Systems",
        "byte", "ushort", "uint", "ulong",
        "0x", "", true, false,
        2, 4, 8, 16,
        "//"
    },
    {
        LanguageId::Python, SyntaxFamily::Python,
        "Python", "Scripting/Tooling",
        "c_uint8", "c_uint16", "c_uint32", "c_uint64",
        "0x", "", true, false,
        2, 4, 8, 16,
        "#"
    },

    {
        LanguageId::Delphi, SyntaxFamily::Delphi,
        "Delphi/FreePascal/Lazarus", "Advanced",
        "Byte", "Word", "DWord", "QWord",
        "$", "", true, false,
        2, 4, 8, 16,
        "//"
    },
    {
        LanguageId::NodeJs, SyntaxFamily::NodeJs,
        "Node.js", "Advanced",
        "Uint8Array", "Uint16Array", "Uint32Array", "BigUint64Array",
        "0x", "", true, false,
        2, 4, 8, 16,
        "//"
    }
};

struct DumpFormatOptions
{
    LanguageId  languageId = LanguageId::Masm;
    DataWidth   dataWidth = DataWidth::Byte;
    Endianness  endianness = Endianness::LittleEndian;
    int         elementsPerLine = 16;

    QString     symbolName = QStringLiteral("Table");
    QString     customLiteralPrefix;
    QString     customLiteralSuffix;

    bool        includeSection = true;
    bool        padIncompleteTail = true;
    bool        useCustomLiteralDecorators = false;
    bool        forceUppercaseHex = false;
    bool        uppercaseHex = true;
    bool        cppStaticConst = true;
    bool        csharpStaticReadonly = false;
    bool        pythonIncludeCtypesImport = true;
};

namespace DumpFormatter {

    static inline int BytesPerElement(DataWidth w)
    {
        return static_cast<int>(w);
    }

    static inline bool IsAsmSyntax(SyntaxFamily f)
    {
        return f == SyntaxFamily::Masm ||
            f == SyntaxFamily::Nasm ||
            f == SyntaxFamily::Fasm ||
            f == SyntaxFamily::Gas;
    }

    static inline const LanguageDescriptor* FindLanguage(LanguageId id)
    {
        for (const auto& lang : g_LanguageDescriptors) {
            if (lang.id == id)
                return &lang;
        }
        return nullptr;
    }

    static inline QString Q(const char* s)
    {
        return QString::fromLatin1(s ? s : "");
    }

    static inline QString TypeName(const LanguageDescriptor& lang, DataWidth w)
    {
        switch (w)
        {
            case DataWidth::Byte:
                return Q(lang.byteType);
            case DataWidth::Word:
                return Q(lang.wordType);
            case DataWidth::Dword:
                return Q(lang.dwordType);
            case DataWidth::Qword:
                return Q(lang.qwordType);
        }
        return QString();
    }

    static inline int MinDigits(const LanguageDescriptor& lang, DataWidth w)
    {
        switch (w)
        {
            case DataWidth::Byte:
                return lang.minDigitsByte;
            case DataWidth::Word:
                return lang.minDigitsWord;
            case DataWidth::Dword:
                return lang.minDigitsDword;
            case DataWidth::Qword:
                return lang.minDigitsQword;
        }
        return 2;
    }

    static inline QString NodeTypedArrayName(DataWidth w)
    {
        switch (w)
        {
            case DataWidth::Byte:
                return QStringLiteral("Uint8Array");
            case DataWidth::Word:
                return QStringLiteral("Uint16Array");
            case DataWidth::Dword:
                return QStringLiteral("Uint32Array");
            case DataWidth::Qword:
                return QStringLiteral("BigUint64Array");
        }
        return QStringLiteral("Uint8Array");
    }

    static inline QString BuildPythonImportLine(const QString& typeName)
    {
        return QStringLiteral("\nfrom ctypes import %1\n").arg(typeName);
    }

    static inline QString CommentPrefix(const LanguageDescriptor& lang)
    {
        return Q(lang.commentPrefix);
    }

    static inline bool IsHexAlpha(QChar c)
    {
        return (c >= QLatin1Char('A') && c <= QLatin1Char('F')) ||
            (c >= QLatin1Char('a') && c <= QLatin1Char('f'));
    }

    static inline quint64 ReadValue(
        const QByteArray& data,
        int offset,
        int widthBytes,
        Endianness endianness,
        int* validBytesOut)
    {
        quint8 chunk[8] = {};
        const int remain = qMax(0, data.size() - offset);
        const int validBytes = qMin(widthBytes, remain);

        for (int i = 0; i < validBytes; ++i)
            chunk[i] = static_cast<quint8>(data[offset + i]);

        if (validBytesOut)
            *validBytesOut = validBytes;

        quint64 value = 0;

        if (endianness == Endianness::LittleEndian)
        {
            for (int i = 0; i < widthBytes; ++i)
                value |= (quint64(chunk[i]) << (i * 8));
        }
        else
        {
            for (int i = 0; i < widthBytes; ++i)
                value = (value << 8) | quint64(chunk[i]);
        }
        return value;
    }

    static inline QString FormatHexValue(
        quint64 value,
        DataWidth width,
        const LanguageDescriptor& lang,
        const DumpFormatOptions& opt)
    {
        const QString prefix = opt.useCustomLiteralDecorators
            ? opt.customLiteralPrefix
            : Q(lang.defaultLiteralPrefix);

        const QString suffix = opt.useCustomLiteralDecorators
            ? opt.customLiteralSuffix
            : Q(lang.defaultLiteralSuffix);

        const bool uppercase = opt.forceUppercaseHex
            ? opt.uppercaseHex
            : lang.uppercaseHex;

        QString digits = QString::number(value, 16);
        if (uppercase)
            digits = digits.toUpper();
        else
            digits = digits.toLower();

        const int minDigits = MinDigits(lang, width);
        if (digits.size() < minDigits)
            digits.prepend(QString(minDigits - digits.size(), QLatin1Char('0')));

        if (!suffix.isEmpty() && prefix.isEmpty() && lang.addLeadingZeroIfSuffixHex)
        {
            if (!digits.isEmpty() && IsHexAlpha(digits[0]))
                digits.prepend(QLatin1Char('0'));
        }

        QString literal = prefix + digits + suffix;

        if (lang.syntax == SyntaxFamily::NodeJs && width == DataWidth::Qword)
        {
            if (!literal.toLower().endsWith(QLatin1Char('n')))
                literal += QLatin1Char('n');
        }
        return literal;
    }

    static inline QString BuildHeader(
        const LanguageDescriptor& lang,
        const DumpFormatOptions& opt,
        int elementCount)
    {
        const QString typeName = TypeName(lang, opt.dataWidth);
        QString out;

        switch (lang.syntax)
        {
            case SyntaxFamily::Masm:
            {
                if (opt.includeSection)
                    out += QStringLiteral("\n.data\n\n");
                out += QStringLiteral("%1 \\\n").arg(opt.symbolName);
                break;
            }
            case SyntaxFamily::Nasm:
            {
                if (opt.includeSection)
                    out += QStringLiteral("\nsection .data\n\n");
                out += QStringLiteral("%1:\n").arg(opt.symbolName);
                break;
            }
            case SyntaxFamily::Fasm:
            {
                if (opt.includeSection)
                    out += QStringLiteral("\nsection '.data' data readable writeable\n\n");
                out += QStringLiteral("%1:\n").arg(opt.symbolName);
                break;
            }
            case SyntaxFamily::Gas:
            {
                if (opt.includeSection)
                    out += QStringLiteral("\n.section .data\n\n");
                out += QStringLiteral("%1:\n").arg(opt.symbolName);
                break;
            }
            case SyntaxFamily::Cpp:
            {
                out += opt.cppStaticConst
                    ? QStringLiteral("\nstatic const %1 %2[] =\n{\n").arg(typeName, opt.symbolName)
                    : QStringLiteral("\nconst %1 %2[] =\n{\n").arg(typeName, opt.symbolName);
                break;
            }
            case SyntaxFamily::CSharp:
            {
                if (opt.csharpStaticReadonly)
                    out += QStringLiteral("\nstatic readonly %1[] %2 =\n{\n").arg(typeName, opt.symbolName);
                else
                    out += QStringLiteral("\n%1[] %2 =\n{\n").arg(typeName, opt.symbolName);
                break;
            }
            case SyntaxFamily::Python:
            {
                if (opt.pythonIncludeCtypesImport)
                    out += BuildPythonImportLine(typeName);

                out += QStringLiteral("\n%1 = (%2 * %3)(\n")
                    .arg(opt.symbolName)
                    .arg(typeName)
                    .arg(elementCount);
                break;
            }
            case SyntaxFamily::Delphi:
            {
                const int upperBound = qMax(0, elementCount - 1);
                out += QStringLiteral("\nconst %1: array[0..%2] of %3 = (\n")
                    .arg(opt.symbolName)
                    .arg(upperBound)
                    .arg(typeName);
                break;
            }
            case SyntaxFamily::NodeJs:
            {
                out += QStringLiteral("\nconst %1 = new %2([\n")
                    .arg(opt.symbolName)
                    .arg(NodeTypedArrayName(opt.dataWidth));
                break;
            }
        }
        return out;
    }

    static inline QString BuildFooter(
        const LanguageDescriptor& lang,
        const DumpFormatOptions& opt)
    {
        Q_UNUSED(opt);

        switch (lang.syntax)
        {
            case SyntaxFamily::Masm:
            case SyntaxFamily::Nasm:
            case SyntaxFamily::Fasm:
            case SyntaxFamily::Gas:
            {
                return QString();
            }
            case SyntaxFamily::Cpp:
            case SyntaxFamily::CSharp:
            {
                return QStringLiteral("};\n");
            }
            case SyntaxFamily::Python:
            {
                return QStringLiteral(")\n");
            }
            case SyntaxFamily::Delphi:
            {
                return QStringLiteral("  );\n");
            }
            case SyntaxFamily::NodeJs:
            {
                return QStringLiteral("]);\n");
            }
        }
        return QString();
    }

    static inline QString BuildLinePrefix(
        const LanguageDescriptor& lang,
        const DumpFormatOptions& opt)
    {
        const QString typeName = TypeName(lang, opt.dataWidth);

        switch (lang.syntax)
        {
            case SyntaxFamily::Masm:
            case SyntaxFamily::Nasm:
            case SyntaxFamily::Fasm:
            case SyntaxFamily::Gas:
            {
                return typeName + QLatin1Char(' ');
            }
            case SyntaxFamily::Cpp:
            case SyntaxFamily::CSharp:
            case SyntaxFamily::Python:
            case SyntaxFamily::Delphi:
            case SyntaxFamily::NodeJs:
            {
                return QStringLiteral("    ");
            }
        }
        return QString();
    }

    static inline QString BuildCommentLine(
        const LanguageDescriptor& lang,
        const QString& text,
        bool indented = false)
    {
        const QString prefix = CommentPrefix(lang);
        return (indented ? QStringLiteral("    ") : QString()) + prefix + QLatin1Char(' ') + text + QLatin1Char('\n');
    }

    static inline QString JoinAsmLine(const QStringList& values)
    {
        return values.join(QStringLiteral(", "));
    }

    static inline QString JoinContainerLine(
        const QStringList& values,
        int globalStartIndex,
        int totalCount)
    {
        QStringList parts;
        parts.reserve(values.size());

        for (int i = 0; i < values.size(); ++i)
        {
            const int globalIndex = globalStartIndex + i;
            QString v = values[i];
            if (globalIndex + 1 < totalCount)
                v += QLatin1Char(',');
            parts.push_back(v);
        }
        return parts.join(QStringLiteral(" "));
    }

    static inline QString FormatDump(
        const QByteArray& data,
        const DumpFormatOptions& opt)
    {
        const LanguageDescriptor* lang = FindLanguage(opt.languageId);
        if (!lang)
            return QString();

        const int widthBytes = BytesPerElement(opt.dataWidth);
        const int perLine = qBound(4, opt.elementsPerLine, 16);

        const int totalElements = (data.isEmpty() || widthBytes <= 0)
            ? 0
            : ((data.size() + widthBytes - 1) / widthBytes);

        QStringList formattedValues;
        formattedValues.reserve(totalElements);

        int paddedTailBytes = 0;

        for (int offset = 0; offset < data.size(); offset += widthBytes)
        {
            int validBytes = 0;
            const quint64 value = ReadValue(data, offset, widthBytes, opt.endianness, &validBytes);

            if (validBytes < widthBytes)
            {
                if (!opt.padIncompleteTail)
                    break;

                paddedTailBytes = widthBytes - validBytes;
            }
            formattedValues.push_back(FormatHexValue(value, opt.dataWidth, *lang, opt));
        }

        const int finalElementCount = formattedValues.size();

        QString out = BuildHeader(*lang, opt, finalElementCount);

        if (paddedTailBytes > 0)
        {
            const QString msg = QStringLiteral("NOTE: last %1 byte(s) padded with 00 to complete %2.")
                .arg(paddedTailBytes)
                .arg(TypeName(*lang, opt.dataWidth));
            out += BuildCommentLine(*lang, msg, !IsAsmSyntax(lang->syntax));
        }

        if (finalElementCount == 0)
        {
            if (!IsAsmSyntax(lang->syntax))
                out += BuildCommentLine(*lang, QStringLiteral("empty dump"), true);
            else
                out += BuildCommentLine(*lang, QStringLiteral("empty dump"), false);

            out += BuildFooter(*lang, opt);
            return out;
        }

        const QString linePrefix = BuildLinePrefix(*lang, opt);

        for (int i = 0; i < finalElementCount; i += perLine)
        {
            const int count = qMin(perLine, finalElementCount - i);

            QStringList lineValues;
            lineValues.reserve(count);

            for (int j = 0; j < count; ++j)
                lineValues.push_back(formattedValues[i + j]);

            if (IsAsmSyntax(lang->syntax))
                out += linePrefix + JoinAsmLine(lineValues) + QLatin1Char('\n');
            else
                out += linePrefix + JoinContainerLine(lineValues, i, finalElementCount) + QLatin1Char('\n');
        }
        out += BuildFooter(*lang, opt);
        return out;
    }

    static inline QString FormatDump(
        const void* ptr,
        int size,
        const DumpFormatOptions& opt)
    {
        if (!ptr || size <= 0)
            return FormatDump(QByteArray(), opt);

        const QByteArray data(static_cast<const char*>(ptr), size);
        return FormatDump(data, opt);
    }
}