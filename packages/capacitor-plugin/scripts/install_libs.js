const fs = require('fs');
const path = require('path');

const msgPrefix = '[@capacitor/background-runner]';

let workingDir = process.env.INIT_CWD;
if (!workingDir) {
  workingDir = '.';
}

if (!fs.existsSync(path.join(workingDir, 'android'))) {
  console.warn(
    `${msgPrefix} cannot install android-js-engine library: @capacitor/android not installed, or in wrong parent directory.`,
  );
  return;
}

const androidGradlePath = path.join(workingDir, 'android/app/build.gradle');

if (!fs.existsSync(androidGradlePath)) {
  console.warn(`${msgPrefix} android build.gradle file not found`);
  return;
}

const searchText =
  "dirs '../capacitor-cordova-android-plugins/src/main/libs', 'libs'";
const insertText =
  "\n\t\tdirs '../../node_modules/@capacitor/background-runner/android/src/main/libs', 'libs'";

let gradleFile = fs.readFileSync(androidGradlePath).toString('utf-8');
if (gradleFile.indexOf(insertText) != -1) {
  return;
}

let insertIndex = gradleFile.indexOf(searchText) + searchText.length;
gradleFile =
  gradleFile.substring(0, insertIndex) +
  insertText +
  gradleFile.substring(insertIndex);

fs.writeFileSync(androidGradlePath, gradleFile);

// remove old version of aar
const oldReleaseAARPath = path.join(
  workingDir,
  'android/src/main/libs/android-js-engine-release.aar',
);
if (fs.existsSync(oldReleaseAARPath)) {
  fs.rmSync(oldReleaseAARPath);
}

console.log(`${msgPrefix} installed lib path to gradle repositories`);
