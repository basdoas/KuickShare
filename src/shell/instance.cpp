#include "instance.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QElapsedTimer>
#include <QThread>

namespace
{
    const QString shellService = QStringLiteral("org.kde.kuickshare");
    const QString coreService = QStringLiteral("io.github.the_beper.KuickShare");

    bool registered(const QString &name)
    {
        const QDBusReply<bool> reply = QDBusConnection::sessionBus().interface()->isServiceRegistered(name);
        return reply.isValid() && reply.value();
    }

    bool waitGone(const QString &name, int timeoutMs)
    {
        QElapsedTimer timer;
        timer.start();
        while (registered(name))
        {
            if (timer.elapsed() > timeoutMs)
            {
                return false;
            }
            QThread::msleep(100);
        }
        return true;
    }
}

bool Instance::shouldReplace(const QString &running, const QString &own, bool force)
{
    return !running.isEmpty() && (force || running != own);
}

void Instance::replaceRunning(bool force)
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected() || !registered(shellService))
    {
        return;
    }
    auto get = QDBusMessage::createMethodCall(shellService, QStringLiteral("/MainApplication"), QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    get << QStringLiteral("org.qtproject.Qt.QCoreApplication") << QStringLiteral("applicationVersion");
    const QDBusReply<QVariant> version = bus.call(get, QDBus::Block, 2000);
    if (!version.isValid() && !force)
    {
        // A busy shell of this very version must not be killed by a plain launch.
        return;
    }
    const QString running = version.isValid() ? version.value().toString() : QStringLiteral("unknown");
    if (!shouldReplace(running, QStringLiteral(KUICKSHARE_VERSION), force))
    {
        return;
    }
    // The old shell stops a core it started with SIGTERM and waits up to 3 s, so give it more than that.
    bus.call(QDBusMessage::createMethodCall(shellService, QStringLiteral("/MainApplication"), QStringLiteral("org.qtproject.Qt.QCoreApplication"), QStringLiteral("quit")), QDBus::Block, 2000);
    if (waitGone(shellService, 8000))
    {
        waitGone(coreService, 3000);
    }
}
