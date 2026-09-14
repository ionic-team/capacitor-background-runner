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

const startMarker = '// BEGIN @capacitor/background-runner';
const endMarker = '// END @capacitor/background-runner';
const legacyDirsLine = /^.*dirs\s+'[^']*@capacitor\/background-runner\/android\/src\/main\/libs'.*\r?\n/gm;

const libsPath = path.resolve(__dirname, '../android/src/main/libs');

if (!fs.existsSync(libsPath)) {
  console.warn(`${msgPrefix} android libs directory not found at ${libsPath}`);
  return;
}

const libsDir = path
  .relative(path.dirname(path.resolve(androidGradlePath)), libsPath)
  .split(path.sep)
  .join('/');

let gradleFile = fs.readFileSync(androidGradlePath).toString('utf-8');

const blockStart = gradleFile.indexOf(startMarker);
const blockEnd = gradleFile.indexOf(endMarker);
if (blockStart !== -1 && blockEnd > blockStart) {
  gradleFile = gradleFile.substring(0, blockStart) + gradleFile.substring(blockEnd + endMarker.length);
}

gradleFile = gradleFile.replace(legacyDirsLine, '');

const block = [
  startMarker,
  'repositories {',
  '    flatDir {',
  `        dirs '${libsDir}'`,
  '    }',
  '}',
  endMarker,
].join('\n');

fs.writeFileSync(androidGradlePath, `${gradleFile.trimEnd()}\n\n${block}\n`);

// remove old version of aar
const oldReleaseAARPath = path.join(workingDir, 'android/src/main/libs/android-js-engine-release.aar');
if (fs.existsSync(oldReleaseAARPath)) {
  fs.rmSync(oldReleaseAARPath);
}

console.log(`${msgPrefix} installed lib path to gradle repositories`);
