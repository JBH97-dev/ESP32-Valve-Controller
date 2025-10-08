const fs = require("fs");
const path = require("path");

const sourceFile = path.join(__dirname, "../frontend/build/index.html");
const targetDir  = path.join(__dirname, "../spiffs/web");
const targetFile = path.join(targetDir, "index.html");

const sourceStaticDir = path.join(__dirname, "../frontend/static");
const targetStaticDir = path.join(targetDir, "static");

// Ensure target directory exists
fs.mkdirSync(targetDir, { recursive: true });

// Remove old index.html if it exists
if (fs.existsSync(targetFile)) {
    fs.unlinkSync(targetFile);
    console.log(`🗑 Removed old ${targetFile}`);
}

// Copy new index.html
fs.copyFileSync(sourceFile, targetFile);
console.log(`✅ Copied ${sourceFile} → ${targetFile}`);

// Recursively copy static directory
function copyDir(src, dest) {
    fs.mkdirSync(dest, { recursive: true });
    for (const entry of fs.readdirSync(src, { withFileTypes: true })) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (entry.isDirectory()) {
            copyDir(srcPath, destPath);
        } else {
            fs.copyFileSync(srcPath, destPath);
            console.log(`✅ Copied ${srcPath} → ${destPath}`);
        }
    }
}

if (fs.existsSync(sourceStaticDir)) {
    copyDir(sourceStaticDir, targetStaticDir);
} else {
    console.log(`⚠️ Static directory not found: ${sourceStaticDir}`);
}
