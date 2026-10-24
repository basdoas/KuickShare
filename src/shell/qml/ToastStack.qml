pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

// Short coloured messages over the window: each one slides up from the bottom edge, older ones are pushed
// up, and every message slides back down by itself after its time (or when clicked).
Item {
    id: stack

    readonly property int maxVisible: 4
    property int lastId: 0

    // `type` is a Kirigami.MessageType.
    function show(type, text, timeout) {
        if (model.count >= maxVisible) {
            model.remove(model.count - 1);
        }
        model.insert(0, {
            uid: ++lastId,
            kind: type,
            body: text,
            timeout: timeout
        });
    }

    function dismiss(uid) {
        for (let i = 0; i < model.count; ++i) {
            if (model.get(i).uid === uid) {
                model.remove(i);
                return;
            }
        }
    }

    parent: QQC2.Overlay.overlay
    z: 100
    width: Math.round(parent.width * 0.7)
    height: list.contentHeight
    anchors.bottom: parent.bottom
    anchors.bottomMargin: Kirigami.Units.gridUnit * 2
    anchors.horizontalCenter: parent.horizontalCenter

    ListView {
        id: list

        anchors.fill: parent
        interactive: false
        spacing: Kirigami.Units.smallSpacing
        // The newest message (index 0) sits at the bottom.
        verticalLayoutDirection: ListView.BottomToTop

        model: ListModel {
            id: model
        }

        add: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "y"
                    from: ViewTransition.destination.y + Kirigami.Units.gridUnit * 4
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: Kirigami.Units.longDuration
                }
            }
        }
        displaced: Transition {
            NumberAnimation {
                property: "y"
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
        // The mirror of `add`: slides down and fades out.
        remove: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "y"
                    to: ViewTransition.item.y + Kirigami.Units.gridUnit * 4
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InCubic
                }
                NumberAnimation {
                    property: "opacity"
                    to: 0
                    duration: Kirigami.Units.longDuration
                }
            }
        }

        delegate: MessageBox {
            id: toast

            required property int uid
            required property int kind
            required property string body
            required property int timeout

            width: ListView.view.width
            type: kind
            text: body
            showCloseButton: false

            // A click closes the message before its time.
            TapHandler {
                onTapped: stack.dismiss(toast.uid)
            }

            Timer {
                running: toast.timeout > 0
                interval: toast.timeout
                onTriggered: stack.dismiss(toast.uid)
            }
        }
    }
}
