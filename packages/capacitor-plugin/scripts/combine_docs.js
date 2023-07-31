const fs = require('fs');
const path = require('path');

let readme = fs.readFileSync('README.md').toString();
let api = fs.readFileSync('API.md').toString();

let start = readme.indexOf('<capacitor-api-docs>');
let end = readme.indexOf('</capacitor-api-docs>');

if (start == -1 || end == -1) {
  return;
}

start += 20;

api = api.replace('<docgen-api>', '').replace('</docgen-api>', '');

readme = readme.substring(0, start) + api + readme.substring(end);

fs.writeFileSync('README.md', readme);
