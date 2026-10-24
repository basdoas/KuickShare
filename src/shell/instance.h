#pragma once

#include <QString>

namespace Instance
{
    /// Whether a running instance (empty `running` = none) should be replaced by this one.
    bool shouldReplace(const QString &running, const QString &own, bool force);

    /// Asks a running instance of another version (or any, with `force`) to quit and waits until it and its core are gone.
    void replaceRunning(bool force);
}
