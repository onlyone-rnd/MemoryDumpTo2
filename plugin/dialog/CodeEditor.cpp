
#include "CodeEditor.h"
#include "SyntaxHighlighter.h"

#include <QtGui/QPainter>
#include <QtGui/QTextBlock>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    m_editor->lineNumberAreaPaintEvent(event);
}

bool isDark();

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent),
    m_lineNumberArea(new LineNumberArea(this))
{
    is_dark = isDark();

    setFocusPolicy(Qt::StrongFocus);
    setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    if (!is_dark)
    {
        viewport()->setAutoFillBackground(true);
        viewport()->setStyleSheet("background-color: #FFF8F0;");
    }

    document()->setDocumentMargin(0);

    m_highlighter = new SyntaxHighlighter(document(), is_dark);
    m_highlighter->setLanguage(SyntaxLanguage::Masm); // default language

    connect(this, &QPlainTextEdit::blockCountChanged,
        this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
        this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
        this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::setSyntaxLanguage(SyntaxLanguage language)
{
    if (m_highlighter)
        m_highlighter->setLanguage(language);
}

bool CodeEditor::hasCustomSelection() const noexcept
{
    return m_hasSelection && m_selAnchor >= 0 &&
        m_selPosition >= 0 && m_selAnchor != m_selPosition;
}

QString CodeEditor::selectedCustomText() const
{
    if (!hasCustomSelection())
        return QString();

    QTextCursor c = makeSelectionCursor(m_selAnchor, m_selPosition);
    return c.selectedText().replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
}

void CodeEditor::copySelectedText()
{
    const QString text = selectedCustomText();
    if (!text.isEmpty())
        QApplication::clipboard()->setText(text);
}

void CodeEditor::clearCustomSelection()
{
    m_hasSelection = false;
    m_mouseSelecting = false;
    m_selAnchor = -1;
    m_selPosition = -1;
    rebuildExtraSelections();
}

void CodeEditor::setCustomSelection(int anchor, int position)
{
    m_hasSelection = true;
    m_selAnchor = qMax(0, anchor);
    m_selPosition = qMax(0, position);
    rebuildExtraSelections();
}

int CodeEditor::documentPositionFromPoint(const QPoint& pt) const
{
    return cursorForPosition(pt).position();
}

QTextCursor CodeEditor::makeCursorAt(int pos) const
{
    QTextCursor c(document());
    c.setPosition(qBound(0, pos, document()->characterCount() - 1));
    return c;
}

QTextCursor CodeEditor::makeSelectionCursor(int anchor, int position) const
{
    QTextCursor c(document());
    const int a = qBound(0, anchor, document()->characterCount() - 1);
    const int b = qBound(0, position, document()->characterCount() - 1);
    c.setPosition(a);
    c.setPosition(b, QTextCursor::KeepAnchor);
    return c;
}

QColor CodeEditor::customSelectionBackground() const
{
    if (is_dark)
        return QColor("#414141");   // dark theme
    return QColor("#C0C0C0");       // light theme
}

void CodeEditor::rebuildExtraSelections()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!hasCustomSelection())
    {
        QTextEdit::ExtraSelection lineSel;
        lineSel.format.setProperty(QTextFormat::FullWidthSelection, true);

        if (hasFocus())
            lineSel.format.setBackground(is_dark ? QColor("#414141") : QColor("#C0C0C0"));

        lineSel.cursor = textCursor();
        lineSel.cursor.clearSelection();
        extraSelections.append(lineSel);
    }

    if (hasCustomSelection())
    {
        const QTextCursor range = makeSelectionCursor(m_selAnchor, m_selPosition);
        const int selStart = range.selectionStart();
        const int selEnd = range.selectionEnd();

        QTextBlock block = document()->findBlock(selStart);
        QTextBlock last = document()->findBlock(qMax(0, selEnd - 1));

        while (block.isValid())
        {
            QTextEdit::ExtraSelection sel;
            sel.format.setBackground(customSelectionBackground());
            sel.format.setProperty(QTextFormat::FullWidthSelection, true);

            QTextCursor lineCursor(block);
            lineCursor.clearSelection();
            sel.cursor = lineCursor;

            extraSelections.append(sel);

            if (block == last)
                break;

            block = block.next();
        }
    }
    setExtraSelections(extraSelections);

    if (m_lineNumberArea)
        m_lineNumberArea->update();

    viewport()->update();
}

void CodeEditor::paintEvent(QPaintEvent* event)
{
    QPlainTextEdit::paintEvent(event);

    if (!hasFocus())
        return;

    if (hasCustomSelection())
        return;

    QPainter p(viewport());
    const QRect cr = cursorRect();
    QRect lineRect(0, cr.y() - 1, viewport()->width() - 1, cr.height() + 1);
    QColor color = is_dark ? QColor("#414141") : QColor("#C0C0C0");
    QPen pen(color);
    pen.setWidth(1);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(lineRect);
}

int CodeEditor::calcLineNumberDigits(int blockCount) const
{
    int digits = 1;
    int max = qMax(1, blockCount);
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }
    return digits;
}

int CodeEditor::lineNumberAreaWidth() const
{
    const int digits = calcLineNumberDigits(blockCount());
    const int gutterWidth = 6 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return gutterWidth + m_lineNumberRightGap;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
        lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    rebuildExtraSelections();
}

void CodeEditor::wheelEvent(QWheelEvent* event)
{
    const int delta = event->angleDelta().y();
    if (delta == 0)
    {
        QPlainTextEdit::wheelEvent(event);
        return;
    }

    setFocus(Qt::MouseFocusReason);

    m_mouseSelecting = false;
    m_hasSelection = false;
    m_selAnchor = -1;
    m_selPosition = -1;

    QTextCursor c = textCursor();

    int steps = qAbs(delta) / 120;
    if (steps <= 0)
        steps = 1;

    const QTextCursor::MoveOperation op =
        (delta > 0) ? QTextCursor::Up : QTextCursor::Down;

    for (int i = 0; i < steps; ++i)
        c.movePosition(op);

    setTextCursor(c);
    ensureCursorVisible();
    rebuildExtraSelections();
    event->accept();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_lineNumberArea);

    const int digits = calcLineNumberDigits(blockCount());
    const int gutterWidth = 6 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    const int totalWidth = gutterWidth + m_lineNumberRightGap;

    const QColor gutterBg = m_lnBg.isValid()
        ? m_lnBg
        : palette().color(QPalette::Window);

    const QColor editorBg = viewport()->palette().color(QPalette::Base);

    const QColor fg = m_lnText.isValid()
        ? m_lnText
        : palette().color(QPalette::Text);

    const int currentBlockNumber = textCursor().blockNumber();

    QColor curFg = fg;
    curFg = fg.lighter(255);

    int selFirstBlock = -1;
    int selLastBlock = -1;

    if (hasCustomSelection())
    {
        const QTextCursor range = makeSelectionCursor(m_selAnchor, m_selPosition);
        selFirstBlock = document()->findBlock(range.selectionStart()).blockNumber();
        selLastBlock = document()->findBlock(qMax(0, range.selectionEnd() - 1)).blockNumber();
    }

    painter.fillRect(0, event->rect().top(), gutterWidth, event->rect().height(), gutterBg);
    painter.fillRect(gutterWidth, event->rect().top(), totalWidth - gutterWidth, event->rect().height(), editorBg);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            const int rowHeight = bottom - top;
            const int h = fontMetrics().height();

            painter.fillRect(0, top, gutterWidth, rowHeight, gutterBg);
            QColor gapColor = editorBg;

            if (hasCustomSelection())
            {
                if (blockNumber >= selFirstBlock && blockNumber <= selLastBlock)
                    gapColor = customSelectionBackground();
            }
            else if (hasFocus() && blockNumber == currentBlockNumber)
                gapColor = is_dark ? QColor("#414141") : QColor("#C0C0C0");

            painter.fillRect(gutterWidth, top, totalWidth - gutterWidth, rowHeight, gapColor);

            const QString number = QString::number(blockNumber + 1);

            painter.setPen(blockNumber == currentBlockNumber ? curFg : fg);
            painter.drawText(
                0,
                top,
                gutterWidth - 3,
                h,
                Qt::AlignRight | Qt::AlignVCenter,
                number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::setLineNumberColors(
    const QColor& text,
    const QColor& background)
{
    m_lnText = text;
    m_lnBg = background;
    if (m_lineNumberArea)
        m_lineNumberArea->update();
    update();
}

void CodeEditor::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        setFocus(Qt::MouseFocusReason);
        const int pos = documentPositionFromPoint(event->pos());
        m_mouseSelecting = true;
        m_hasSelection = true;
        m_selAnchor = pos;
        m_selPosition = pos;
        QTextCursor c = makeCursorAt(pos);
        setTextCursor(c);
        rebuildExtraSelections();
        event->accept();
        return;
    }
    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mouseSelecting)
    {
        const int pos = documentPositionFromPoint(event->pos());
        m_selPosition = pos;
        QTextCursor c = makeCursorAt(pos);
        setTextCursor(c);
        ensureCursorVisible();
        rebuildExtraSelections();
        event->accept();
        return;
    }
    QPlainTextEdit::mouseMoveEvent(event);
}

void CodeEditor::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mouseSelecting)
    {
        m_mouseSelecting = false;
        m_selPosition = documentPositionFromPoint(event->pos());
        QTextCursor c = makeCursorAt(m_selPosition);
        setTextCursor(c);
        rebuildExtraSelections();
        event->accept();
        return;
    }
    QPlainTextEdit::mouseReleaseEvent(event);
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
    {
        copySelectedText();
        return;
    }

    if (event->matches(QKeySequence::SelectAll))
    {
        m_hasSelection = true;
        m_selAnchor = 0;
        m_selPosition = qMax(0, document()->characterCount() - 1);
        setTextCursor(makeCursorAt(m_selPosition));
        rebuildExtraSelections();
        return;
    }

    auto clearSelectionOnly = [this]()
    {
        m_mouseSelecting = false;
        m_hasSelection = false;
        m_selAnchor = -1;
        m_selPosition = -1;
    };

    switch (event->key())
    {
        case Qt::Key_Up:
            clearSelectionOnly();
            moveCursor(QTextCursor::Up);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_Down:
            clearSelectionOnly();
            moveCursor(QTextCursor::Down);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_Left:
            clearSelectionOnly();
            moveCursor(QTextCursor::Left);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_Right:
            clearSelectionOnly();
            moveCursor(QTextCursor::Right);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_Home:
            clearSelectionOnly();
            moveCursor(QTextCursor::StartOfLine);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_End:
            clearSelectionOnly();
            moveCursor(QTextCursor::EndOfLine);
            ensureCursorVisible();
            rebuildExtraSelections();
            return;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            clearSelectionOnly();
            QPlainTextEdit::keyPressEvent(event);
            rebuildExtraSelections();
            return;

        default:
            break;
    }
    QPlainTextEdit::keyPressEvent(event);
    rebuildExtraSelections();
}
