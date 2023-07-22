const fs = require('fs');
const path = require('path');

let workingDir = process.env.INIT_CWD;
if (!workingDir) {
  workingDir = '.';
}

if (!fs.existsSync(path.join(workingDir, 'android'))) {
  console.warn(
    'cannot install android-js-engine library: @capacitor/android not installed, or in wrong parent directory.',
  );
  return;
}

const releaseAARPath = path.join(
  'android/src/main/libs/android-js-engine-release.aar',
);

const fullPath = path.join(workingDir, 'android/app/libs');
if (!fs.existsSync(fullPath)) {
  fs.mkdirSync(fullPath);
}

fs.copyFileSync(
  releaseAARPath,
  path.join(fullPath, 'android-js-engine-release.aar'),
);

console.log(`copied android-js-engine-release.aar to ${fullPath}`);
