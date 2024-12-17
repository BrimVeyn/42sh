const express = require('express');
const cors = require('cors');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 4000; // Port for the backend server

// Middleware
app.use(cors()); // Allow requests from any origin

// Endpoint to serve log.json
app.get('/api/logs', (req, res) => {
	console.log("Received request to load log.json")
    const filePath = path.join(__dirname, '/data/log.json');

    fs.readFile(filePath, 'utf-8', (err, data) => {
        if (err) {
            console.error('Error reading file:', err);
            return res.status(500).json({ error: 'Failed to read the log file.' });
        }
        try {
            const jsonData = JSON.parse(data); // Parse the file content to JSON
            res.json(jsonData); // Send JSON response
        } catch (parseError) {
            console.error('Error parsing JSON:', parseError);
            res.status(500).json({ error: 'Invalid JSON content in log file.' });
        }
    });
});

// Endpoint to serv inputs,outputs and errors
app.get('/api/files/:filePath', (req, res) => {

	console.log("Received request for file path:", req.params);
    // Get the file path from the request and sanitize it (e.g., URL decode)
    const { filePath } = req.params;

    // Resolve the full file path safely
    const safePath = path.resolve(__dirname, filePath);

    // Ensure the file exists and send it
    fs.readFile(safePath, 'utf-8', (err, data) => {
        if (err) {
            console.error("Error reading file:", err);
            return res.status(500).json({ error: 'Error reading the file' });
        }
        res.json(data);  // Send file content as JSON
    });
});

// Start the server
app.listen(PORT, () => {
	console.log(`Backend running at http://localhost:${PORT}`);
});
