---
"@capacitor/background-runner": minor
---

(iOS): Fixes the iOS 14 and iOS 15 deprecation warnings in the geolocation and notifications APIs. The deprecated `CLLocationManager.authorizationStatus()` class method is replaced by the `authorizationStatus` instance property, and `summaryArgument` is no longer set on scheduled notifications because iOS ignores it since iOS 15. The `summaryArgument` notification option is deprecated and now has no effect.
