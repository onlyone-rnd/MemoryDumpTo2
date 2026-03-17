#pragma once

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>
#include <QtGui/QColor>

enum class SyntaxLanguage : int;
class SyntaxHighlighter; // forward
class LineNumberArea; // forward

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget* parent = nullptr);

    int  lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    void setLineNumberColors(
        const QColor& text,
        const QColor& background);

    void setSyntaxLanguage(SyntaxLanguage language);

    bool hasCustomSelection() const noexcept;
    QString selectedCustomText() const;

public slots:
    void copySelectedText();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int /*newBlockCount*/);
    void updateLineNumberArea(const QRect& rect, int dy);
    void highlightCurrentLine();

private:
    void rebuildExtraSelections();
    void clearCustomSelection();
    int calcLineNumberDigits(int blockCount) const;
    void setCustomSelection(int anchor, int position);
    int  documentPositionFromPoint(const QPoint& pt) const;
    QTextCursor makeCursorAt(int pos) const;
    QTextCursor makeSelectionCursor(int anchor, int position) const;
    QColor customSelectionBackground() const;

private:
    LineNumberArea* m_lineNumberArea = nullptr;
    SyntaxHighlighter* m_highlighter = nullptr;
    QColor m_lnText;
    QColor m_lnBg;
    bool is_dark = false;
    bool m_mouseSelecting = false;
    bool m_hasSelection = false;
    int  m_lineNumberRightGap = 6;
    int  m_selAnchor = -1;
    int  m_selPosition = -1;
    friend class LineNumberArea;
};

class LineNumberArea final : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor* editor)
        : QWidget(editor), m_editor(editor) {
    }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    CodeEditor* m_editor = nullptr;
};