#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include <QWidget>
#include <QScreen>
#include <QStyle>
#include <QEvent>

class QuickRun : public QWidget {
public:
    QuickRun() {
        // remove title bar, minimal look
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        m_lineEdit = new QLineEdit(this);
        
        // large font and padding
        m_lineEdit->setStyleSheet("QLineEdit { font-size: 18px; padding: 8px; border: 2px solid #3daee9; border-radius: 4px; background: #232629; color: #eff0f1; }");
        m_lineEdit->setPlaceholderText("Enter a command or a folder name");
        
        layout->addWidget(m_lineEdit);
        setLayout(layout);

        connect(m_lineEdit, &QLineEdit::returnPressed, this, &QuickRun::handleExecution);
    }

protected:
    // close if click outside the window
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::ActivationChange && !isActiveWindow()) {
            close();
        }
    }

private:
    QLineEdit *m_lineEdit;

    void handleExecution() {
        QString cmd = m_lineEdit->text().trimmed();
        if (cmd.isEmpty()) return;

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
