---
"@capacitor/background-runner": patch
---

(Android): Fixes a crash at startup in release builds with R8 enabled. The JS engine's classes are now kept, since the native engine finds them by name.
