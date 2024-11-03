#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFont>

class AnaPencere : public QMainWindow {
    Q_OBJECT

public:
    AnaPencere(QWidget *parent = nullptr)
    : QMainWindow(parent), gecenZaman(0), kayitYapiliyor(false) {

        QWidget *anaWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(anaWidget);

        baslatButonu = new QPushButton("Başlat", this);
        bitirButonu = new QPushButton("Bitir", this);
        zamanEtiketi = new QLabel("0:00:00", this);

        QFont font = zamanEtiketi->font();
        font.setPointSize(32);
        zamanEtiketi->setFont(font);
        zamanEtiketi->setAlignment(Qt::AlignCenter);

        layout->addWidget(baslatButonu);
        layout->addWidget(bitirButonu);
        layout->addWidget(zamanEtiketi);

        setCentralWidget(anaWidget);

        zamanlayici = new QTimer(this);
        ffmpegIslemi = new QProcess(this);

        connect(baslatButonu, &QPushButton::clicked, this, &AnaPencere::kaydiBaslat);
        connect(bitirButonu, &QPushButton::clicked, this, &AnaPencere::kaydiBitir);
        connect(zamanlayici, &QTimer::timeout, this, &AnaPencere::zamanGuncelle);

        loadStylesheet("styles.css");
    }

private:
    void loadStylesheet(const QString &dosyaAdi) {
        QFile dosya(dosyaAdi);
        if (dosya.open(QFile::ReadOnly)) {
            QString stil = QLatin1String(dosya.readAll());
            setStyleSheet(stil);
            dosya.close();
        }
    }

private slots:
    void kaydiBaslat() {
        if (kayitYapiliyor) return;

        gecenZaman = 0;
        kayitYapiliyor = true;
        baslatButonu->setEnabled(false);
        bitirButonu->setEnabled(true);
        zamanlayici->start(1000);
        zamanEtiketi->setText("0:00:00");

        QString klasorYolu = "kayıt";
        QDir klasor;
        if (!klasor.exists(klasorYolu)) {
            klasor.mkpath(klasorYolu);
        }

        QString zamanDamgasi = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        mevcutDosyaYolu = QString("%1/kayıt_%2.mp4").arg(klasorYolu, zamanDamgasi);

        QString komut = QString("ffmpeg -f x11grab -s 1920x1080 -i :0.0 -c:v libx264 -pix_fmt yuv420p -y %1").arg(mevcutDosyaYolu);
        ffmpegIslemi->start(komut);
    }

    void kaydiBitir() {
        if (!kayitYapiliyor) return;

        ffmpegIslemi->write("q");
        zamanlayici->stop();
        zamanEtiketi->setText("0:00:00");
        kayitYapiliyor = false;

        baslatButonu->setEnabled(true);
        bitirButonu->setEnabled(false);
    }

    void zamanGuncelle() {
        gecenZaman++;
        int saat = gecenZaman / 3600;
        int dakika = (gecenZaman % 3600) / 60;
        int saniye = gecenZaman % 60;
        zamanEtiketi->setText(QString("%1:%2:%3").arg(saat, 2, 10, QLatin1Char('0')).arg(dakika, 2, 10, QLatin1Char('0')).arg(saniye, 2, 10, QLatin1Char('0')));
    }

private:
    QPushButton *baslatButonu;
    QPushButton *bitirButonu;
    QLabel *zamanEtiketi;
    QTimer *zamanlayici;
    QProcess *ffmpegIslemi;
    int gecenZaman;
    bool kayitYapiliyor;
    QString mevcutDosyaYolu;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    AnaPencere w;
    w.setWindowTitle("Ekran Kaydı");
    w.resize(300, 200);
    w.show();
    return a.exec();
}

#include "main.moc"
