#pragma once

#include <QtWidgets/QComboBox>

class ComboBoxEx final : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxEx(QWidget* parent = nullptr);

    int firstSelectableIndex() const;
    int lastSelectableIndex() const;
    int nextSelectableIndex(int start, int step) const;
    bool isSelectableIndex(int index) const;

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};
