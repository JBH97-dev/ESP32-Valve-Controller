const fs = require("fs");
const path = require("path");

const sourceFile = path.join(__dirname, "../frontend/build/index.html");
const targetDir  = path.join(__dirname, "../spiffs/web");
const targetFile = path.join(targetDir, "index.html");

// Ensure target directory exists
fs.mkdirSync(targetDir, { recursive: true });

// Remove old index.html if it exists
if (fs.existsSync(targetFile)) {
    fs.unlinkSync(targetFile);
    console.log(`🗑 Removed old ${targetFile}`);
}

// Copy new file
fs.copyFileSync(sourceFile, targetFile);
console.log(`✅ Copied ${sourceFile} → ${targetFile}`);
