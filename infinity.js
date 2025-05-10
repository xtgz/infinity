const { exec } = require('child_process');

// Jalankan mine_infinity.py menggunakan Python
exec('python3 mine_infinity.py', (error, stdout, stderr) => {
  if (error) {
    console.error(`Error: ${error.message}`);
    return;
  }
  if (stderr) {
    console.error(`Stderr: ${stderr}`);
    return;
  }
  console.log(`Output: ${stdout}`);
});
