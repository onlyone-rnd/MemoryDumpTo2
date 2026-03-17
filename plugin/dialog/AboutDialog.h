#pragma once

#include <QtWidgets/QDialog>
#include <QDateTime>
#include <QLocale>

namespace Ui { class AboutDialog; }

class AboutDialog : public QDialog {
    
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog();

private:
    QDate CompileDate()
    {
        return QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy");
    }

    QString ReleaseString(const QDate& date)
    {
        static const char* months[] =
        {
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec"
        };
        return QString().sprintf("Released: %d %s %d", date.day(), months[date.month() - 1], date.year());
    }

    void AboutQt();
    void setColor();

private:
    Ui::AboutDialog* ui;
    bool m_dark = false;
};

void ShowAboutDialog(
    _In_ HWND hParent);