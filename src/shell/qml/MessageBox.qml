import QtQuick
import org.kde.kirigami as Kirigami

// An InlineMessage whose icon stays vertically centered. Kirigami moves it to the top of
// multi-line boxes through the "multi-line" state of its icon; that state is switched off here,
// which leaves the "single-line" fallback (vertical center). Relies on Kirigami internals: the
// icon is the only child with two states (the label has a "multi-line" state of its own that must stay).
// Message texts never end with a period.
Kirigami.InlineMessage {
    visible: true // hidden by default

    Component.onCompleted: {
        for (const item of contentItem.children) {
            if (item.states && item.states.length === 2 && item.states[0].name === "multi-line")
                item.states[0].when = false;
        }
    }
}
