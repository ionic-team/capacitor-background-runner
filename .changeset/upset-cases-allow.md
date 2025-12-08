---
"@capacitor/background-runner": minor
---

Fixes an issue where CapacitorBackgroundRunner overwrites the UNUserNotificationCenter.current().delegate, causing conflicts with other plugins that rely on notification handling
