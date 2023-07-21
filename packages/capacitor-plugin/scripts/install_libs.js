const fs = require("fs");
const path = require('path');

console.log(process.env.INIT_CWD);

let workingDir = process.env.INIT_CWD;
if (!workingDir) {
    workingDir = ".";
}

if (!fs.existsSync(path.join(workingDir, "android"))) {
    throw new Error("cannot install android-js-engine library, not in correct parent directory.");
}

const releaseAARPath = path.join("android/src/main/libs/android-js-engine-release.aar");

const fullPath = path.join(workingDir, "android/app/libs")
fs.mkdirSync(fullPath);
fs.copyFileSync(releaseAARPath, path.join(fullPath, "android-js-engine-release.aar"))

console.log(`copied android-js-engine-release.aar to ${fullPath}`)


