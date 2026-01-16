#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include <QWidget>
#include <QScreen>
#include <QStyle>
#include <QEvent>
#include <QKeyEvent>
#include <QCompleter>
#include <QStringListModel>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSortFilterProxyModel>
#include <QAbstractItemView>

class FlexibleFilterModel : public QSortFilterProxyModel {
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    void setFilterText(const QString &text) {
        m_filterText = text;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        QString text = sourceModel()->index(source_row, 0, source_parent).data().toString();
        if (m_filterText.isEmpty()) return true;

        const QStringList parts = m_filterText.split(' ', Qt::SkipEmptyParts);
        for (const QString &part : parts) {
            if (!text.contains(part, Qt::CaseInsensitive)) {
                return false;
            }
        }
        return true;
    }

private:
    QString m_filterText;
};

class QuickRun : public QWidget {
public:
    QuickRun() {
        // setup history file path
        m_historyPath = QDir::homePath() + "/.local/share/qrun_history";
        loadHistory();

        // remove title bar, minimal look
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        m_lineEdit = new QLineEdit(this);
        
        // large font and padding
        m_lineEdit->setStyleSheet("QLineEdit { font-size: 20px; padding: 10px; border: 3px solid white; border-radius: 10px; background: #232629; color: #eff0f1; }");
        m_lineEdit->setPlaceholderText("Command or folder");
        // warning: center text hide placeholder
        // m_lineEdit->setAlignment(Qt::AlignCenter);
        setMinimumWidth(300);
        
        // setup history auto-completion
        m_historyModel = new QStringListModel(m_history, this);
        m_proxyModel = new FlexibleFilterModel(this);
        m_proxyModel->setSourceModel(m_historyModel);

        m_completer = new QCompleter(m_proxyModel, this);
        m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        m_completer->setWrapAround(false);
        m_lineEdit->setCompleter(m_completer);

        connect(m_lineEdit, &QLineEdit::textEdited, this, [this](const QString &text){
            m_proxyModel->setFilterText(text);
        });

        connect(m_lineEdit, &QLineEdit::textChanged, this, [this](const QString &text){
            m_lineEdit->setAlignment(text.isEmpty() ? Qt::AlignLeft : Qt::AlignCenter);
        });

        m_lineEdit->installEventFilter(this);

        layout->addWidget(m_lineEdit);
        setLayout(layout);

        connect(m_lineEdit, &QLineEdit::returnPressed, this, &QuickRun::handleExecution);
    }

    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == m_lineEdit && event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                if (!m_completer->popup()->isVisible()) {
                    m_proxyModel->setFilterText(m_lineEdit->text());
                    m_completer->complete();
                    return true;
                }
            }
            if (keyEvent->key() == Qt::Key_Escape) {
                if (m_completer->popup()->isVisible()) {
                    return false;
                }
                close();
                return true;
            }
        }
        return QWidget::eventFilter(obj, event);
    }

protected:
    // close if click outside the window
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::ActivationChange && !isActiveWindow()) {
            close();
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            close();
        } else {
            QWidget::keyPressEvent(event);
        }
    }

private:
    QLineEdit *m_lineEdit;
    QCompleter *m_completer;
    QStringListModel *m_historyModel;
    FlexibleFilterModel *m_proxyModel;
    QStringList m_history;
    QString m_historyPath;

    // load history from file to string list
    void loadHistory() {
        QFile file(m_historyPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    m_history << line;
                }
            }
            m_history.removeDuplicates();
            file.close();
        }
    }

    // save the new command to the history file
    void saveToHistory(const QString &cmd) {
        // move to top if exists, otherwise just add
        m_history.removeAll(cmd);
        m_history.prepend(cmd);

        // limit history size to 200 entries
        while (m_history.size() > 200) m_history.removeLast();

        QFile file(m_historyPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString &item : m_history) {
                out << item << "\n";
            }
            file.close();
        }
    }

    void handleExecution() {
        QString cmd = m_lineEdit->text().trimmed();
        if (cmd.isEmpty()) return;

        // save command before executing
        saveToHistory(cmd);

        QString home = QDir::homePath();
        if (QDir(cmd).exists()) {
            // it's a volume folder
            QProcess::startDetached("dolphin", {cmd});
        } else if (QDir(home + "/" + cmd).exists()) {
            // it's a local user folder
            QProcess::startDetached("dolphin", {home + "/" + cmd});
        } else {
            // it's a script or application
            QProcess::startDetached("bash", {"-i", "-c", cmd});
        }
        qApp->quit();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QuickRun w;
    w.show();
    
    // center the window
    w.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            w.size(),
            app.primaryScreen()->availableGeometry()
        )
    );

    return app.exec();
}