#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantMap>


#include <QFileSystemWatcher>

/// Tells when the package this process runs from was replaced by a newer one while it kept running (Flatpak, AppImage).
class UpdateWatcher : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWatcher(QObject *parent = nullptr);

    /// Starts a new instance of the installed version with --replace, and --background if asked.
    static bool startReplacement(bool background);

    /// Identity of a file for change detection: inode and modification time.
    static QString fileSignature(const QString &path);

signals:
    void updateInstalled();

private slots:
    void onFlatpakUpdate(const QVariantMap &info);

private:
    void watchAppImage();

    QString m_appImage;
    QString m_signature;
    QFileSystemWatcher m_files;
    QTimer m_settle;
};
