#!/usr/bin/env python3
"""Writes a synthetic transfer history, for the RAM sandbox of scripts/testing/run.sh.

Usage: demo-history.py <history.json>
Times are relative to now, so the list always looks recent. The paths and devices are made up.
"""
import json
import sys
import time

MIN, HOUR, DAY = 60, 3600, 86400

# (age in seconds, kind, state, device, payload_kind, files, text, total_bytes, trusted)
ROWS = [
    (4 * MIN, "inbound", "Finished", "Pixel 7", "Files", ["/home/demo/Downloads/IMG_2041.jpg", "/home/demo/Downloads/IMG_2042.jpg"], "", 7_340_032, True),
    (22 * MIN, "outbound", "Finished", "Galaxy A55", "Files", ["/home/demo/Documents/invoice-march.pdf"], "", 412_000, False),
    (1 * HOUR, "inbound", "Finished", "Pixel 7", "Url", [], "https://kde.org/plasma-desktop/", 0, True),
    (3 * HOUR, "outbound", "Disconnected", "Galaxy A55", "Files", ["/home/demo/Videos/holiday.mp4"], "", 482_344_960, False),
    (5 * HOUR, "inbound", "Rejected", "Unknown phone", "Files", ["/tmp/promo.apk"], "", 18_874_368, False),
    (9 * HOUR, "outbound", "Finished", "Tux Laptop", "Text", [], "Meeting notes: ship the filters on Friday.", 0, True),
    (1 * DAY, "inbound", "Finished", "Tux Laptop", "Files", ["/home/demo/Downloads/slides.odp", "/home/demo/Downloads/notes.txt", "/home/demo/Downloads/logo.svg"], "", 2_516_582, True),
    (1 * DAY + 3 * HOUR, "outbound", "Cancelled", "Pixel 7", "Files", ["/home/demo/Music/album.flac"], "", 38_797_312, False),
    (2 * DAY, "inbound", "Finished", "Galaxy A55", "WiFi", [], "HomeNetwork-5G", 0, False),
    (3 * DAY, "inbound", "Finished", "Pixel 7", "Text", [], "Remember to buy milk, eggs and coffee.", 0, True),
    (4 * DAY, "outbound", "Rejected", "Living room tablet", "Files", ["/home/demo/Pictures/wallpaper.png"], "", 3_145_728, False),
    (6 * DAY, "inbound", "Disconnected", "Galaxy A55", "Files", ["/home/demo/Downloads/backup.zip"], "", 734_003_200, False),
    (9 * DAY, "outbound", "Finished", "Pixel 7", "Files", [f"/home/demo/Pictures/trip/photo-{i:02d}.jpg" for i in range(1, 13)], "", 52_428_800, True),
    (14 * DAY, "inbound", "Finished", "Tux Laptop", "Url", [], "https://github.com/the-beper/KuickShare", 0, True),
]


def main() -> None:
    now = int(time.time())
    entries = []
    for i, (age, kind, state, device, payload, files, text, size, trusted) in enumerate(ROWS):
        when = now - age
        entries.append({
            "uid": when * 1000 + i,
            "time": when,
            "kind": kind,
            "state": state,
            "device": device,
            "payload_kind": payload,
            "files": files,
            "text": text,
            "total_bytes": size,
            "trusted": trusted,
        })
    with open(sys.argv[1], "w", encoding="utf-8") as out:
        json.dump(entries, out)


if __name__ == "__main__":
    main()
