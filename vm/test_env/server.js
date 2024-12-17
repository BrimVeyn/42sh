const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws');

const SCRIPT_PATH = './tester.sh'; // Path to the bash script
const PORT = 4000; // WebSocket port

const wss = new WebSocket.Server({ port: PORT });
console.log(`WebSocket server running on ws://localhost:${PORT}`);

wss.on('connection', (ws) => {
    console.log('Client connected to WebSocket');

    ws.send(JSON.stringify({ message: 'Connected to scheduler' }));
});

// Function to run the bash script
function runScript() {
    console.log('Running tester script...');
    exec(SCRIPT_PATH, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error running script: ${error.message}`);
            return;
        }

        console.log('Script executed successfully.');
        notifyClients();
    });
}

// Notify clients via WebSocket
function notifyClients() {
    const message = { message: 'Test results updated' };

    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(message));
        }
    });
}

// Run the script every 30 seconds
setInterval(runScript, 30000);

console.log('Scheduler started. Running script periodically...');
